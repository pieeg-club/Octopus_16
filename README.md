<div align="center">

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_logo.jpg" alt="Octopus 16" width="360">

**16 channels of EEG in a coin-sized sensor.**

Dry pogo-pin electrodes, two TI ADS131M08 converters and Bluetooth LE, on a 26 mm board.

[📖 Documentation](https://docs.pieeg.com/hardware/devices/Octopus_16/) ·
[🚀 How to Start](https://docs.pieeg.com/hardware/devices/Octopus_16/setup/) ·
[⚡ Flash firmware](https://octobridge.pieeg.com/flash) ·
[🛒 Buy](https://www.pieeg.com/hardware) ·
[💬 Discord](https://discord.gg/5Ps24p47)

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_render.jpg" alt="Octopus 16 with the XIAO ESP32-S3 mounted" width="380">

</div>

> [!WARNING]
> Octopus 16 is **not a medical device** and has not been certified by any regulator for use with the human body. It is a learning and experimentation tool. You use it at your own risk.

> [!CAUTION]
> **Power bank only while worn.** The board has **no isolation** from its power supply. Never wear it while it's plugged into a computer or a wall charger. USB from a computer is only for flashing, with the board off the head. Read the [Safety page](https://docs.pieeg.com/hardware/devices/Octopus_16/safety/) and the [liability notice](https://pieeg.com/liability/).

## At a glance

| | |
|---|---|
| **Channels** | 16, plus reference and ground, all in the same cluster |
| **Electrodes** | 18 dry spring-loaded pogo pins, ⌀ 4 × 11 mm, ~5.38 mm pitch |
| **Converters** | 2 × TI ADS131M08, 24-bit, simultaneous sampling, gain 4 |
| **Sample rate** | 250 SPS (8.192 MHz clock, OSR 16384) |
| **Board** | 26 mm round, 4 layers |
| **Controller** | Seeed Studio XIAO ESP32-S3 (not included) |
| **Wireless** | Bluetooth LE 5, appears as `Octopus-16-XXXX` |
| **Power** | Small 5 V power bank, ≤ 2 A, via the XIAO's USB-C |
| **Software** | [PiEEG Cloud](https://cloud.pieeg.com), [octoBridge](https://octobridge.pieeg.com/), JavaScript SDK, Python SDK (this repo) |

## How it works

```
 18 pogo pins ─► 2 × ADS131M08 ─► XIAO ESP32-S3 ─► Bluetooth LE
 (16 ch + REF + GND)                     ▲            │
                                         │            ├ cloud.pieeg.com
                 5 V power bank ─────────┘            ├ octobridge.pieeg.com
                                                      ├ JavaScript SDK
                                                      └ Python SDK
```

Because the reference and ground are **inside the same cluster** as the 16 signal pins, no ear clips or distant electrodes are needed. The whole montage sits on one spot of the scalp.

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_top.jpg" alt="Octopus 16, top side with XIAO sockets" width="280"> <img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_pins.jpg" alt="Octopus 16, pogo-pin side" width="280">

## Quick start

1. **Mount the XIAO ESP32-S3** on the Octopus 16 so the `5V / GND / 3V3` labels line up, and clip on the BLE antenna.
2. **Flash the firmware once** at [octobridge.pieeg.com/flash](https://octobridge.pieeg.com/flash) (Chrome or Edge, USB-C **data** cable, board off the head). It then appears as `Octopus-16-XXXX`.
3. **Power** the XIAO from a small **power bank**. The LED blinks slowly while it waits.
4. **Connect** in [cloud.pieeg.com](https://cloud.pieeg.com) → **PiEEG Devices** → **Octopus 16**, or in [octobridge.pieeg.com](https://octobridge.pieeg.com/connect).
5. **Put it on** over **Pz** (top-back of the head), press it straight down, hold it with a strap, close your eyes and watch alpha rise.

> [!IMPORTANT]
> A XIAO mounted **180° the wrong way** still flashes and shows up on Bluetooth, but sends **no EEG data** (the LED fast double-blinks). Check the `5V / GND / 3V3` labels line up.

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_xiao_mounted.jpg" alt="XIAO ESP32-S3 mounted the right way round, with the antenna" width="420">

Full walkthrough: **[docs.pieeg.com → Octopus 16 → How to Start](https://docs.pieeg.com/hardware/devices/Octopus_16/setup/)**

## Wearing it

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_wear_back.jpg" alt="Octopus 16 worn over the back of the head" width="260"> <img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_wear_strap.jpg" alt="Strap with power bank" width="260">

| Spot | 10-20 position | Good for |
|---|---|---|
| Top-back of the head | Pz | Alpha rhythm (eyes closed), the default spot |
| Back of the head | Oz, O1, O2 | Alpha, visual activity |
| Top of the head | Cz | Motor activity |
| Forehead | Fp1, Fp2 | Blinks and eye movement |

Part the hair under the cluster and press the board **straight down**. Good contact on all 18 pins is the biggest factor in signal quality.

## Firmware

| | |
|---|---|
| **Flash in the browser** | [octobridge.pieeg.com/flash](https://octobridge.pieeg.com/flash) (also [firmware.pieeg.com](https://firmware.pieeg.com/)). No port? Hold **BOOT**, tap **RESET**, release BOOT, install again. |
| **Source** (this repo) | [`Framework`](Framework): ADS131M08 driver and Arduino sketches for the XIAO ESP32-S3 (EEG + on-board FFT + Bluetooth / USB **gamepad** output) and XIAO ESP32-C3 |

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_flasher.jpg" alt="Octopus-16 firmware installer" width="450">

| LED pattern | State |
|---|---|
| Solid (brief) | Booting |
| Slow blink | Advertising, waiting for an app |
| Solid | Connected and streaming |
| Fast double-blink | ADC not detected: check the XIAO orientation |

## Software

| | Best for | Install |
|---|---|---|
| **[PiEEG Cloud](https://cloud.pieeg.com)** | Dashboards, spectra, experiences, Buddy AI | None (Chrome / Edge) |
| **[octoBridge](https://octobridge.pieeg.com/)** | Classrooms, block programs, the [OctoBio Kit](https://docs.pieeg.com/hardware/devices/OctoBio_Kit/) | None (Chrome / Edge) |
| **[JavaScript SDK](https://docs.pieeg.com/software/api/javascript-sdk/)** | Your own web app: `eeg.connectBLE({ device: 'octopus-16' })` | One script |
| **Python SDK** (this repo) | Live view with heatmap and FFT, recording to Excel | `pip install bleak` |

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_cloud.jpg" alt="Choosing Octopus 16 in the PiEEG Cloud Session Lobby" width="600">

### Python SDK

```bash
pip install bleak matplotlib numpy scipy openpyxl pandas

python GUI/SDK_Data_Visual.py                    # live 16 channels + heatmap + FFT
python GUI/SDK_Data_Visual.py --save data.csv    # also save to CSV
python GUI/SDK_Data_Visual.py --notch 60         # 60 Hz mains (default 50)
python GUI/SDK_Data_Visual.py --demo             # synthetic data, no board

python Dataset/Visualisation.py --duration 30 --out session.xlsx   # record raw data to Excel
```

The scripts find the board by its Bluetooth **service UUID**, so they work whatever name it advertises. [Python SDK demo video](https://www.youtube.com/watch?v=OMOJHC0NXXE).

### BLE protocol

| | |
|---|---|
| Service | `4fafc201-1fb5-459e-8fcc-c5c9c331914b` |
| Data (notify) | `beb5483e-36e1-4688-b7f5-ea07361b26a8` |
| Command (write + notify) | `c0de0001-36e1-4688-b7f5-ea07361b26a8` (ADC register bridge) |
| Packet | 51 bytes: `0xA0` · counter · ADC1 ch 1–8 (8 × 3 B) · ADC2 ch 9–16 (8 × 3 B) · `0xC0` |
| µV | `raw × (1.2 / 4 / 8388607) × 1e6` ≈ 0.0358 µV per count |

Details: [Specification](https://docs.pieeg.com/hardware/devices/Octopus_16/specification/).

## Example signals

**Blinking test**, sensor over Fp (raw and band-pass filtered)

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_blink.png" alt="Blink test with Octopus 16 over Fp" width="650">

**Alpha test**, eyes closed, sensor over Pz

<img src="https://raw.githubusercontent.com/pieeg-club/PiEEG-docs/main/public/images/hardware/2026/10/octopus16_alpha.jpg" alt="Alpha test with Octopus 16 over Pz" width="650">

**Datasets** in [`Dataset`](Dataset): `Blinking_Fp.xlsx`, `closed_eys_Pz.xlsx`, `open_yeys_Pz.xlsx` (16 channels, ~10 s each, raw ADC counts). [`FFT.py`](Dataset/FFT.py) plots the spectrum of one channel.

## Pogo pin care

- Clean the pin tips with isopropyl alcohol (IPA > 90%) and blow out dust with compressed air.
- Press the board **straight down** (90°). Never push or tilt the pins sideways under pressure.
- **Stuck pin:** a drop of IPA into the barrel, gently work the plunger with a non-metal tool, let it dry fully.
- **Never use oil or grease.** It attracts dirt and ruins the contact.

## What's in this repository

| Folder | Content |
|---|---|
| [`Framework`](Framework) | ADS131M08 driver, Arduino sketches for XIAO ESP32-S3 and ESP32-C3 |
| [`GUI`](GUI) | `SDK_Data_Visual.py`: live 16-channel view with heatmap and FFT |
| [`Dataset`](Dataset) | Blink and alpha recordings, recorder and FFT scripts |
| [`images`](images) | Images used in the original README |

## Documentation

| Page | |
|---|---|
| [Octopus 16 Intro](https://docs.pieeg.com/hardware/devices/Octopus_16/) | Overview, comparison with cap-based boards |
| [What's in the Box](https://docs.pieeg.com/hardware/devices/Octopus_16/required-equipment/) | XIAO, power bank, cable, strap |
| [How to Start](https://docs.pieeg.com/hardware/devices/Octopus_16/setup/) | Step-by-step setup and first signal checks |
| [Pins & Placement](https://docs.pieeg.com/hardware/devices/Octopus_16/connections/) | Pin array, channel positions, jumpers, XIAO pin map |
| [Software](https://docs.pieeg.com/hardware/devices/Octopus_16/software/) | PiEEG Cloud, octoBridge, JavaScript and Python SDKs |
| [Signals & Datasets](https://docs.pieeg.com/hardware/devices/Octopus_16/examples/) | Example recordings and datasets |
| [Firmware](https://docs.pieeg.com/hardware/devices/Octopus_16/firmware/) | Browser flashing, LED states, building from source |
| [Specification](https://docs.pieeg.com/hardware/devices/Octopus_16/specification/) | Hardware, BLE protocol, µV conversion |
| [Troubleshooting & Care](https://docs.pieeg.com/hardware/devices/Octopus_16/troubleshooting/) | Flashing, Bluetooth, signal fixes, pogo pin care |
| [Safety](https://docs.pieeg.com/hardware/devices/Octopus_16/safety/) | Power-bank-only operation and responsible use |

## Video

[Octopus 16: start measuring EEG](https://www.youtube.com/watch?v=Yz7GLRGZNk8) · 3D models on Thingiverse: [Octopus 16](https://www.thingiverse.com/thing:7386761) · [boxes](https://www.thingiverse.com/PiEEG/designs)

## Acknowledgements

Thank you to [Dmitry](https://github.com/dmitry-sukhoruchkin) for bringing this project to life.

## Contact

- 🌐 [pieeg.com](https://pieeg.com/)
- 📖 [docs.pieeg.com](https://docs.pieeg.com/)
- 💬 [Discord](https://discord.gg/5Ps24p47)
- ✉️ [pieeg@pieeg.com](mailto:pieeg@pieeg.com)
