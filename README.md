# pid-motor-control-esp32

Minimal embedded closed-loop motor speed PID demo for **ESP32 (PlatformIO)**.
Produces CSV logs over serial and a simple step-response demo. Includes host-mode PID unit tests and a Python simulator.


---

## Run Simulation (no hardware)

```bash
python3 sim/sim.py
# → sim_output.csv, sim_plot.png
```
![alt text](image.png)

Custom PID:

```bash
python3 sim/sim.py --kp 0.005 --ki 0.03 --kd 0 --out my_log.csv --plot my_plot.png
```

---

## ESP32 on Windows

Build + upload:

```bash
pio run -e esp32dev -t upload
```

Serial monitor:

```bash
pio device monitor -e esp32dev -b 115200
```

Toggle step: press **BOOT (GPIO0)** or type `s`.

---

## ESP32 on WSL2

### 1. Attach USB

On Windows (PowerShell, Admin):

```powershell
usbipd wsl list
usbipd wsl attach --busid <busid> --distribution Ubuntu-22.04
```

Now device is `/dev/ttyUSB0`.

### 2. Build

```bash
pip install -U platformio   # once
pio run -e esp32dev
```

### 3. Flash

```bash
pio run -e esp32dev -t upload --upload-port /dev/ttyUSB0
```

### 4. Monitor

```bash
pio device monitor -e esp32dev -b 115200 --port /dev/ttyUSB0
```

### 5. Save Log

```bash
pio device monitor -e esp32dev -b 115200 --port /dev/ttyUSB0 | tee log.csv
```

---

## Plot Log

```python
import pandas as pd, matplotlib.pyplot as plt
df = pd.read_csv("log.csv")
t = df['time_ms']/1000
plt.plot(t, df['setpoint_rpm'], label="setpoint")
plt.plot(t, df['measured_rpm'], label="measured")
plt.xlabel("Time (s)"); plt.ylabel("RPM"); plt.legend(); plt.show()
```


