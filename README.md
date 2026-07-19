# Octopus 16

<p align="center">
  <img src="https://github.com/pieeg-club/Octopus_16/blob/main/images/img_7.jfif" width="70%" height="70%" alt="generals view">
</p>

**Octopus 16** packs sixteen active EEG electrodes, a common reference, and a ground into a single **26 mm circular PCB** — a coin-sized cluster of spring-loaded pogo pins. Because reference and ground live inside the same footprint as the active channels, **no distant electrodes are required**: the whole montage sits locally on the scalp and streams 16-channel data over **Bluetooth Low Energy 5** to a host, a USB dongle, or straight to the browser.

### General view
<p align="center">
  <img src="https://github.com/pieeg-club/Brain_Stethoscope/blob/main/images/1780357136663.jfif" width="25%" height="25%" alt="generals view">
</p>
<p align="center">
  <img src="https://github.com/pieeg-club/Brain_Stethoscope/blob/main/images/1780357136533.jfif" width="25%" height="25%" alt="generals view">
</p>

### Blinking test, Electrode in Fp position
<p align="center">
  <img src="https://github.com/pieeg-club/Brain_Stethoscope/blob/main/images/ch1_before_after.png" width="50%" height="50%" alt="generals view">
</p>

### Alpha test (closed), Electrode in Pz position
<p align="center">
  <img src="https://github.com/pieeg-club/Brain_Stethoscope/blob/main/images/Alph.jpeg" width="50%" height="50%" alt="generals view">
</p>

### Demo
YouTube [video](https://www.youtube.com/watch?v=OMOJHC0NXXE&t=2s)

#### Warnings
>[!WARNING]
> Octopus_16 is not medical device. You are fully responsible for your personal decision to purchase this device and, ultimately, for its safe use. Octopus_16 is not a medical device and has not been certified by any government regulatory agency for use with the human body. Use it at your own risk.
>[!CAUTION]
> The device must operate only from a battery - 5 V. Complete isolation from the mains power is required.! The device MUST not be connected to any kind of mains power, via USB or otherwise.
> Power supply - only battery 5V, please read the [liability](https://pieeg.com/liability/)

### How it works
16 Ch
Data transfer - BLE
250 samples per second
Connect ESP-32C6 to the data transfer

---

## Technical Specification

### Signal acquisition (analog front-end)

| Parameter | Value |
|---|---|
| **ADC** | 2 × Texas Instruments **ADS131M08** |
| **ADC type** | 24-bit, 8-channel, simultaneous-sampling, delta-sigma (ΔΣ) |
| **Channel count** | **16** (8 per ADC) |
| **Resolution** | **24-bit** per channel (transmitted as 3 bytes / channel) |
| **Programmable gain (PGA)** | Per-channel, ×1 – ×128 (set in firmware; default gain register `0x2222`) |
| **Input high-pass / DC block** | Integrated ADC DC-block filter enabled (≈ >1 Hz) |
| **ADC interface** | SPI (shared bus; independent chip-select per ADC, shared `RESET`, single `DRDY` from ADC 1 as sync master) |
| **Clock source** | External SMD oscillator, EuroQuartz **XO32** series (3.2 × 2.5 mm), **8.192 MHz** |
| **Oversampling ratio (OSR)** | 16384 |
| **Output data rate** | **250 SPS** — f_CLKIN / (2 × OSR) = 8.192 MHz / (2 × 16384) |

### Electrode array

| Parameter | Value |
|---|---|
| **Contact type** | Spring-loaded **pogo pins**, ⌀4 × 11 mm SMD |
| **Total pins** | **18** |
| → Active signal (+) | **16** channels |
| → Reference (−) | **1** (common `AINREF`) |
| → Ground | **1** (`GND`) |
| **Pin pitch (center-to-center)** | **≈ 5.38 mm** (uniform hexagonal packing; measured min 5.37 / max 5.39 mm) |
| **Array span** | ≈ 20.3 × 20.3 mm, all pins within a ~10.5 mm radius of center |
| **Reference / ground routing** | Selectable via on-board jumpers (`AINREF_POGOPIN_ON`, `AINREF_STACK_ON`, `GND_ON`, `STACK_1/2`) for stacking or single-board use |

### Board

| Parameter | Value |
|---|---|
| **Form factor** | Circular PCB, **26.0 mm** diameter |
| **Layers** | **4-layer** (F.Cu / In1.Cu / In2.Cu / B.Cu) |
| **Target placement** | Pz (parietal); works as a fully local montage anywhere on the scalp |
| **Enclosure** | 3D CAD included (`p70-2100045`, IGS + STP) |

### Compute & wireless

| Parameter | Value |
|---|---|
| **Main module** | Seeed Studio **XIAO ESP32-C6** (RISC-V, BLE 5) |
| **Alternate build** | **XIAO ESP32-S3** variant (adds native **USB / BLE HID gamepad** output for neurofeedback control) |
| **Radio** | **Bluetooth Low Energy 5**, **2M PHY**, TX power +3 dBm |
| **Data packet** | 51-byte custom frame: `0xA0` header + 1-byte counter + 24 B (ADC1) + 24 B (ADC2) + `0xC0` footer |
| **Throughput layout** | 16 channels × 3 bytes = 48 signal bytes per sample |

---

## Getting started  
Will be update very soon  
Youtube instructions   
Manuals    

## Placement

Place the 18-pin cluster over **Pz** (or your target site) with firm, even contact. Because reference and ground are inside the same cluster, no additional electrodes are needed. Good scalp contact on all pins is the single biggest factor in signal quality.
---

Soon will be available for order; drop your email [here](https://octopus.pieeg.com/)  
