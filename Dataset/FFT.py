

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, filtfilt

df = pd.read_excel("closed_eys_Pz.xlsx", sheet_name="Raw data")
df = df[pd.to_numeric(df["Time (s)"], errors="coerce").notna()].copy()
for c in df.columns:
    df[c] = pd.to_numeric(df[c], errors="coerce")
df = df.dropna().reset_index(drop=True)

time = df["Time (s)"].values
raw  = df["CH5"].values
fs   = 250.0

# Bandpass filter
nyq  = fs / 2
b, a = butter(4, [1/nyq, 30/nyq], btype="band")
filtered = filtfilt(b, a, raw)

# FFT of raw and filtered
n         = len(raw)
freqs     = np.fft.rfftfreq(n, d=1/fs)
fft_raw   = np.abs(np.fft.rfft(raw))      / n * 2
fft_filt  = np.abs(np.fft.rfft(filtered)) / n * 2

mask = freqs <= 60

fig, axes = plt.subplots(2, 2, figsize=(15, 8))
fig.suptitle("CH1 — time domain & frequency spectrum (0–60 Hz)", fontsize=13)

# Time: raw
axes[0,0].plot(time, raw, color="#457B9D", linewidth=0.7)
axes[0,0].set_title("Time domain — raw")
axes[0,0].set_ylabel("ADC")
axes[0,0].set_xlabel("Time (s)")
axes[0,0].grid(True, alpha=0.3)

# Time: filtered
axes[0,1].plot(time, filtered, color="#E63946", linewidth=0.7)
axes[0,1].set_title("Time domain — after 1–30 Hz bandpass")
axes[0,1].set_ylabel("ADC")
axes[0,1].set_xlabel("Time (s)")
axes[0,1].grid(True, alpha=0.3)

# FFT: raw
axes[1,0].plot(freqs[mask], fft_raw[mask], color="#457B9D", linewidth=0.9)
axes[1,0].set_title("FFT — raw (0–60 Hz)")
axes[1,0].set_ylabel("Amplitude")
axes[1,0].set_xlabel("Frequency (Hz)")
axes[1,0].set_xlim(0, 60)
axes[1,0].grid(True, alpha=0.3)

# FFT: filtered
axes[1,1].plot(freqs[mask], fft_filt[mask], color="#E63946", linewidth=0.9)
axes[1,1].set_title("FFT — after 1–30 Hz bandpass (0–60 Hz)")
axes[1,1].set_ylabel("Amplitude")
axes[1,1].set_xlabel("Frequency (Hz)")
axes[1,1].set_xlim(0, 60)
axes[1,1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig("ch1_time_fft.png", dpi=150)
plt.show()
print("Saved: ch1_time_fft.png")
