// ============================================================================
//  Octopos 16-ch raw EEG streamer  —  ESP32-C3 variant
//  Adapted from the working ESP32-S3 script. ONLY BLE lines were changed.
//
//  BLE changes vs. S3 reference:
//    1. setPower(): ESP_PWR_LVL_P3 (1.x enum) -> setPower(3) (2.x dBm form).
//       The enum only compiled on S3 by an implicit int cast; on the C3 that
//       accidental value can be wrong/invalid. 3 == +3 dBm, matching intent.
//
//  Everything else (SPI, ADC config, packet format, register bridge, pins) is
//  byte-for-byte identical to the S3 script — NimBLE-Arduino is the same API
//  on both chips.
//
//  NOTE (not BLE, but read before flashing a C3):
//    * XIAO-C3 GPIO8/GPIO9 are strapping pins. PIN_SCLK=D8 and PIN_MISO=D9 map
//      to those on a XIAO-C3; if boot becomes unreliable, that's why.
//    * The C3 is single-core: the DRDY ISR, loop(), and the BLE host all share
//      one core. If you see disconnects/lag under load, relax the connection
//      interval (see updateConnParams below) before blaming the radio.
// ============================================================================

#include <Arduino.h>
#include <SPI.h>
#include "ADS131M08.h"
#include <NimBLEDevice.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define DATA_CHAR_UUID      "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CMD_CHAR_UUID       "c0de0001-36e1-4688-b7f5-ea07361b26a8" // Характеристика прозрачного моста

// Пины
#define PIN_CS1    D0
#define PIN_DRDY1  D1  // 
#define PIN_RESET  D2  // 
#define PIN_CS2    D3
#define PIN_SCLK   D8
#define PIN_MISO   D9
#define PIN_MOSI   D10
#define PIN_CLKOUT D6


ADS131M08 adc1;
ADS131M08 adc2;

NimBLECharacteristic* pDataCharacteristic = nullptr;
NimBLECharacteristic* pCmdCharacteristic = nullptr;

volatile bool deviceConnected = false;
volatile bool drdyTriggered = false;


uint8_t customPacket[51];
uint8_t sampleCounter = 0;

volatile bool has_pending_read = false;
volatile bool has_pending_write = false;
volatile uint8_t  reg_addr = 0;
volatile uint16_t reg_val = 0;
volatile uint16_t reg_mask = 0;
volatile bool use_mask = false;
volatile bool needs_adv_restart = false;

void IRAM_ATTR onDrdy() { drdyTriggered = true; }


class CmdCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pChar, NimBLEConnInfo& connInfo) override {
        std::string rx = pChar->getValue();
        if (rx.length() == 1) {
            reg_addr = rx[0];
            has_pending_read = true;
        } else if (rx.length() == 3) {
            reg_addr = rx[0];
            reg_val = (rx[1] << 8) | rx[2];
            use_mask = false;
            has_pending_write = true;
        } else if (rx.length() == 5) {
            reg_addr = rx[0];
            reg_val = (rx[1] << 8) | rx[2];
            reg_mask = (rx[3] << 8) | rx[4];
            use_mask = true;
            has_pending_write = true;
        }
    }
};

class MyServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        deviceConnected = true;
        // C3 tip: if the single core can't sustain 7.5ms + notify traffic and the
        // link drops, widen the max here, e.g. updateConnParams(handle, 6, 12, 0, 100).
        pServer->updateConnParams(connInfo.getConnHandle(), 6, 6, 0, 100);
        Serial.println("BLE Connected!");
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        deviceConnected = false;
        needs_adv_restart = true;
        Serial.printf("BLE Disconnected. Reason: %d\n", reason);
    }
};

void setup() {
    Serial.begin(115200);


    SPI.begin(PIN_SCLK, PIN_MISO, PIN_MOSI, -1);


    adc1.begin(PIN_SCLK, PIN_MISO, PIN_MOSI, PIN_CS1, PIN_DRDY1, PIN_RESET);
    adc2.begin(PIN_SCLK, PIN_MISO, PIN_MOSI, PIN_CS2, -1, PIN_RESET);

    adc1.reset();

    adc1.setOsr(OSR_16384);
    adc1.writeRegisterMasked(0x08, 0x0F, 0x000F); // DC Block > 1Hz
    adc1.writeRegister(REG_GAIN1, 0x2222); 
    adc1.writeRegister(REG_GAIN2, 0x2222);

    adc2.setOsr(OSR_16384);
    adc2.writeRegisterMasked(0x08, 0x0F, 0x000F); 
    adc2.writeRegister(REG_GAIN1, 0x2222); 
    adc2.writeRegister(REG_GAIN2, 0x2222);

    customPacket[0] = 0xA0;
    customPacket[50] = 0xC0;

    NimBLEDevice::init("Octopos");

    // >>> C3 / NimBLE-2.x CHANGE <<<
    // 1.x enum ESP_PWR_LVL_P3 replaced by explicit dBm int (2.x API).
    NimBLEDevice::setPower(3); // +3 dBm

    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    NimBLEService *pService = pServer->createService(SERVICE_UUID);

    pDataCharacteristic = pService->createCharacteristic(DATA_CHAR_UUID, NIMBLE_PROPERTY::NOTIFY);

    pCmdCharacteristic = pService->createCharacteristic(CMD_CHAR_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    pCmdCharacteristic->setCallbacks(new CmdCallbacks());

    pService->start();

    NimBLEDevice::getAdvertising()->addServiceUUID(SERVICE_UUID);
    NimBLEDevice::getAdvertising()->start();

    pinMode(PIN_DRDY1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_DRDY1), onDrdy, FALLING);
}

void loop() {

    if (needs_adv_restart) {
        delay(500);
        NimBLEDevice::startAdvertising();
        needs_adv_restart = false;
        Serial.println("Advertising restarted. Ready for reconnect.");
    }

    if (has_pending_read) {
        uint16_t val = adc1.readRegister(reg_addr);
        uint8_t tx[3] = { reg_addr, (uint8_t)(val >> 8), (uint8_t)(val & 0xFF) };
        pCmdCharacteristic->notify(tx, 3);
        Serial.printf("Read Reg 0x%02X -> 0x%04X\n", reg_addr, val);
        has_pending_read = false;
    }

    if (has_pending_write) {
        if (use_mask) {
            adc1.writeRegisterMasked(reg_addr, reg_val, reg_mask);
            adc2.writeRegisterMasked(reg_addr, reg_val, reg_mask);
            Serial.printf("Write Masked: 0x%02X, Val 0x%04X, Mask 0x%04X\n", reg_addr, reg_val, reg_mask);
        } else {
            adc1.writeRegister(reg_addr, reg_val);
            adc2.writeRegister(reg_addr, reg_val);
            Serial.printf("Write Direct: 0x%02X, Val 0x%04X\n", reg_addr, reg_val);
        }
        has_pending_write = false;
    }

    if (drdyTriggered) {
        drdyTriggered = false;

        AdcOutput raw1 = adc1.readAdcRaw();
        AdcOutput raw2 = adc2.readAdcRaw();

        if (deviceConnected) {
    
            customPacket[1] = sampleCounter++;

            for (int i = 0; i < 8; i++) {
                int32_t v = raw1.ch[i].i;
                customPacket[2 + i*3 + 0] = (v >> 16) & 0xFF;
                customPacket[2 + i*3 + 1] = (v >> 8) & 0xFF;
                customPacket[2 + i*3 + 2] = v & 0xFF;
            }

            for (int i = 0; i < 8; i++) {
                int32_t v = raw2.ch[i].i;
                customPacket[26 + i*3 + 0] = (v >> 16) & 0xFF;
                customPacket[26 + i*3 + 1] = (v >> 8) & 0xFF;
                customPacket[26 + i*3 + 2] = v & 0xFF;
            }

            pDataCharacteristic->notify(customPacket, 51);

        } else if (!sampleCounter){
            needs_adv_restart = true;
        }
    }
}
