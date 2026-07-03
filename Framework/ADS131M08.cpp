#include "Arduino.h"
#include "ADS131M08.h"

#define settings SPISettings(1000000, MSBFIRST, SPI_MODE1) 

ADS131M08::ADS131M08() : csPin(0), drdyPin(0), clkPin(0), misoPin(0), mosiPin(0), resetPin(0)
{
  for( uint16_t i = 0U; i < 8; i++){
    fullScale.ch[i].f = 1.2; 
    pgaGain[i] = ADS131M08_PgaGain::PGA_1;
    resultFloat.ch[i].f = 0.0;
    resultRaw.ch[i].u[0] = 0U;
    resultRaw.ch[i].u[1] = 0U;
  }
}

uint32_t ADS131M08::transfer24(uint32_t data) {
    uint8_t b1 = SPI.transfer((data >> 16) & 0xFF);
    uint8_t b2 = SPI.transfer((data >> 8) & 0xFF);
    uint8_t b3 = SPI.transfer(data & 0xFF);
    return ((uint32_t)b1 << 16) | ((uint32_t)b2 << 8) | b3;
}

uint8_t ADS131M08::writeRegister(uint8_t address, uint16_t value)
{
  uint32_t cmd_word = ((uint32_t)CMD_WRITE_REG | (address << 7)) << 8; 
  uint32_t data_word = ((uint32_t)value) << 8; 
  
  SPI.beginTransaction(settings);
  digitalWrite(csPin, LOW);
  delayMicroseconds(1); 

  transfer24(cmd_word);
  transfer24(data_word);

  for(int i = 0; i < 8; i++) { 
      transfer24(0x000000); 
  }

  delayMicroseconds(1); 
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  
  return 1; 
}

void ADS131M08::writeRegisterMasked(uint8_t address, uint16_t value, uint16_t mask)
{
  uint16_t register_contents = readRegister(address);
  register_contents = (register_contents & ~mask) | (value & mask); 
  writeRegister(address, register_contents);
}

uint16_t ADS131M08::readRegister(uint8_t address)
{
  uint32_t cmd_word = ((uint32_t)CMD_READ_REG | (address << 7)) << 8; 
  uint32_t dummy_word = 0x000000;
  
  SPI.beginTransaction(settings);
  digitalWrite(csPin, LOW);
  delayMicroseconds(1); 

  transfer24(cmd_word); 
  for (int i = 0; i < 9; i++) { 
      transfer24(dummy_word);
  }
  
  delayMicroseconds(1); 
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();

  delayMicroseconds(50); 

  SPI.beginTransaction(settings);
  digitalWrite(csPin, LOW);
  delayMicroseconds(1); 

  uint32_t received_data_word = transfer24(dummy_word); 

  for (int i = 0; i < 9; i++) { 
      transfer24(dummy_word);
  }

  delayMicroseconds(1); 
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();

  return (uint16_t)(received_data_word >> 8);
}

void ADS131M08::begin(uint8_t clk_pin, uint8_t miso_pin, uint8_t mosi_pin, uint8_t cs_pin, uint8_t drdy_pin, uint8_t reset_pin)
{
  this->csPin = cs_pin;
  this->drdyPin = drdy_pin;
  this->clkPin = clk_pin;
  this->misoPin = miso_pin;
  this->mosiPin = mosi_pin;
  this->resetPin = reset_pin;

  // УДАЛЕНО: this->spi.begin(...);
  // Мы инициализируем SPI в основном скетче!

  pinMode(this->csPin, OUTPUT);
  digitalWrite(this->csPin, HIGH);
  
  pinMode(this->resetPin, OUTPUT);
  
  if (this->drdyPin != 255 && this->drdyPin != -1) {
      pinMode(this->drdyPin, INPUT);
  }
}

int8_t ADS131M08::isDataReadySoft(byte channel)
{
  uint16_t status_reg = readRegister(REG_STATUS);
  switch(channel) {
    case 0: return (status_reg & REGMASK_STATUS_DRDY0) ? 1 : 0;
    case 1: return (status_reg & REGMASK_STATUS_DRDY1) ? 1 : 0;
    case 2: return (status_reg & REGMASK_STATUS_DRDY2) ? 1 : 0;
    case 3: return (status_reg & REGMASK_STATUS_DRDY3) ? 1 : 0;
    case 4: return (status_reg & REGMASK_STATUS_DRDY4) ? 1 : 0;
    case 5: return (status_reg & REGMASK_STATUS_DRDY5) ? 1 : 0;
    case 6: return (status_reg & REGMASK_STATUS_DRDY6) ? 1 : 0;
    case 7: return (status_reg & REGMASK_STATUS_DRDY7) ? 1 : 0;
    default: return -1; 
  }
}

bool ADS131M08::isResetStatus(void) { return (readRegister(REG_STATUS) & REGMASK_STATUS_RESET); }
bool ADS131M08::isLockSPI(void) { return (readRegister(REG_STATUS) & REGMASK_STATUS_LOCK); }
bool ADS131M08::setDrdyFormat(uint8_t drdyFormat) { if (drdyFormat > 1) return false; writeRegisterMasked(REG_MODE, drdyFormat, REGMASK_MODE_DRDY_FMT); return true; }
bool ADS131M08::setDrdyStateWhenUnavailable(uint8_t drdyState) { if (drdyState > 1) return false; writeRegisterMasked(REG_MODE, (drdyState == DRDY_STATE_HI_Z) ? REGMASK_MODE_DRDY_HiZ : 0, REGMASK_MODE_DRDY_HiZ); return true; }
bool ADS131M08::setPowerMode(uint8_t powerMode) { if (powerMode > 3) return false; writeRegisterMasked(REG_CLOCK, powerMode, REGMASK_CLOCK_PWR); return true; }
bool ADS131M08::setOsr(uint16_t osr) { if (osr > 7) return false; writeRegisterMasked(REG_CLOCK, osr << 2 , REGMASK_CLOCK_OSR); return true; }
void ADS131M08::setFullScale(uint8_t channel, float scale) { if (channel > 7) return; this->fullScale.ch[channel].f = scale; }
float ADS131M08::getFullScale(uint8_t channel) { if (channel > 7) return 0.0; return this->fullScale.ch[channel].f; }

void ADS131M08::reset()
{
  digitalWrite(this->resetPin, LOW);
  delay(10); 
  digitalWrite(this->resetPin, HIGH);
  delay(15); 
  delay(100); // Можно уменьшить до 100мс
}

bool ADS131M08::setChannelEnable(uint8_t channel, uint16_t enable) {
  if (channel > 7) return false;
  uint16_t channel_bit_shift = 8 + channel; 
  uint16_t value_to_write = (enable & 0x01) << channel_bit_shift; 
  uint16_t mask = 1 << channel_bit_shift; 
  writeRegisterMasked(REG_CLOCK, value_to_write, mask);
  return true;
}

bool ADS131M08::setChannelPGA(uint8_t channel, ADS131M08_PgaGain pga) { 
  uint16_t pgaCode = (uint16_t) pga;
  if (channel > 7) return false;
  
  uint8_t reg_address;
  uint16_t value_to_write;
  uint16_t mask;

  if (channel <= 3) {
      reg_address = REG_GAIN1;
      value_to_write = (pgaCode & 0x07) << (channel * 4); 
      mask = (uint16_t)0x0007 << (channel * 4); 
  } else { 
      reg_address = REG_GAIN2;
      value_to_write = (pgaCode & 0x07) << ((channel - 4) * 4); 
      mask = (uint16_t)0x0007 << ((channel - 4) * 4);
  }

  writeRegisterMasked(reg_address, value_to_write, mask);
  this->pgaGain[channel] = pga;
  return true;
}

ADS131M08_PgaGain ADS131M08::getChannelPGA(uint8_t channel) { if(channel > 7) return ADS131M08_PgaGain::PGA_INVALID; return this->pgaGain[channel]; }
void ADS131M08::setGlobalChop(uint16_t global_chop) { writeRegisterMasked(REG_CFG, (global_chop & 0x01) << 8, REGMASK_CFG_GC_EN); }
void ADS131M08::setGlobalChopDelay(uint16_t delay) { writeRegisterMasked(REG_CFG, (delay & 0x0F) << 9, REGMASK_CFG_GC_DLY); }
bool ADS131M08::setInputChannelSelection(uint8_t channel, uint8_t input) { if (channel > 7) return false; uint8_t reg_address = REG_CH0_CFG + (channel * 5); writeRegisterMasked(reg_address, (input & 0x03), REGMASK_CHX_CFG_MUX); return true; }
bool ADS131M08::setChannelOffsetCalibration(uint8_t channel, int32_t offset) { if (channel > 7) return false; uint8_t reg_ocal_msb = REG_CH0_OCAL_MSB + (channel * 5); uint8_t reg_ocal_lsb = REG_CH0_OCAL_LSB + (channel * 5); writeRegisterMasked(reg_ocal_msb, (offset >> 8) & 0xFFFF, 0xFFFF); writeRegisterMasked(reg_ocal_lsb, (offset & 0xFF) << 8, REGMASK_CHX_OCAL0_LSB); return true; }
bool ADS131M08::setChannelGainCalibration(uint8_t channel, uint32_t gain) { if (channel > 7) return false; uint8_t reg_gcal_msb = REG_CH0_GCAL_MSB + (channel * 5); uint8_t reg_gcal_lsb = REG_CH0_GCAL_LSB + (channel * 5); writeRegisterMasked(reg_gcal_msb, (gain >> 8) & 0xFFFF, 0xFFFF); writeRegisterMasked(reg_gcal_lsb, (gain & 0xFF) << 8, REGMASK_CHX_GCAL0_LSB); return true; }

bool ADS131M08::isDataReady() {
  if (drdyPin == 255 || drdyPin == -1) return false;
  return (digitalRead(drdyPin) == LOW); 
}

uint16_t ADS131M08::getId() { return readRegister(REG_ID); }
uint16_t ADS131M08::getModeReg() { return readRegister(REG_MODE); }
uint16_t ADS131M08::getClockReg() { return readRegister(REG_CLOCK); }
uint16_t ADS131M08::getCfgReg() { return readRegister(REG_CFG); }

AdcOutput ADS131M08::readAdcRaw(void)
{
  const size_t frame_bytes = 10 * 3; 
  __attribute__((aligned(4))) uint8_t tx_buffer[frame_bytes] = {0}; 
  __attribute__((aligned(4))) uint8_t rx_buffer[frame_bytes];       

  SPI.beginTransaction(settings);
  digitalWrite(csPin, LOW);
  delayMicroseconds(1); 

#ifdef USE_DMA_FOR_ADC_READ
  delayMicroseconds(1); 
  SPI.transferBytes(tx_buffer, rx_buffer, frame_bytes);
#else
  for (size_t i = 0; i < frame_bytes; i++) {
      rx_buffer[i] = SPI.transfer(tx_buffer[i]);
  }
#endif

  delayMicroseconds(1); 
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();

  uint32_t received_word;
  received_word = ((uint32_t)rx_buffer[0] << 16) | ((uint32_t)rx_buffer[1] << 8) | rx_buffer[2];
  this->resultRaw.status = (uint16_t)(received_word >> 8); 

  for(int i = 0; i < 8; i++)
  {
    size_t byte_offset = 3 + (i * 3); 
    received_word = ((uint32_t)rx_buffer[byte_offset] << 16) | 
                    ((uint32_t)rx_buffer[byte_offset + 1] << 8) | 
                    rx_buffer[byte_offset + 2];
    
    int32_t val = (int32_t)received_word;
    if (val & 0x800000) { 
      val |= 0xFF000000; 
    } else {
      val &= 0x00FFFFFF; 
    }
    this->resultRaw.ch[i].i = val;
  }
  return this->resultRaw;
}

float ADS131M08::scaleResult(uint8_t num)
{
  if( num >= 8) return 0.0;
  float pga_val;
  switch(this->pgaGain[num]) {
      case ADS131M08_PgaGain::PGA_1: pga_val = 1.0; break;
      case ADS131M08_PgaGain::PGA_2: pga_val = 2.0; break;
      case ADS131M08_PgaGain::PGA_4: pga_val = 4.0; break;
      case ADS131M08_PgaGain::PGA_8: pga_val = 8.0; break;
      case ADS131M08_PgaGain::PGA_16: pga_val = 16.0; break;
      case ADS131M08_PgaGain::PGA_32: pga_val = 32.0; break;
      case ADS131M08_PgaGain::PGA_64: pga_val = 64.0; break;
      case ADS131M08_PgaGain::PGA_128: pga_val = 128.0; break;
      default: pga_val = 1.0; break; 
  }

  const float VREF = 1.2; 
  const float MAX_RAW_COUNT_POS = 8388607.0; 
  float lsb_value_volts = (VREF / pga_val) / MAX_RAW_COUNT_POS;
  return this->resultFloat.ch[num].f = (float)this->resultRaw.ch[num].i * lsb_value_volts;
}

AdcOutput ADS131M08::scaleResult(void) { this->resultFloat.status = this->resultRaw.status; for(int i = 0; i<8; i++) { this->scaleResult(i); } return this->resultFloat; }
AdcOutput ADS131M08::readAdcFloat(void) { this->readAdcRaw(); return this->scaleResult(); }