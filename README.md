# Smart-Bot 🤖💧

A Bluetooth-controlled robot that drives around and monitors water quality in real time. It streams live **temperature**, **pH**, and **turbidity** readings over Bluetooth while being driven remotely, making it useful for basic water-quality surveys, ponds, tanks, or educational demos.

---

## Features

- **Bluetooth remote control** — drive forward, backward, left, right, and stop using simple single-character commands (compatible with any Bluetooth serial terminal or a custom app, e.g. Arduino Bluetooth Controller).
- **Live environmental sensing**
  - Ambient temperature via **DHT11**
  - Water **pH** level via an analog pH sensor
  - Water **turbidity** (cloudiness) via an analog turbidity sensor
- **Dual DC motor drive** using a standard L298N-style motor driver.
- Sensor data is printed to the **Serial Monitor** (for debugging) and simultaneously broadcast over **Bluetooth** (for remote monitoring on a phone/PC).

---

## Hardware Required

| Component | Notes |
|---|---|
| Arduino Uno / Nano (or compatible) | Main controller |
| HC-05 / HC-06 Bluetooth module | Used with `SoftwareSerial` on pins 10 (RX), 11 (TX) |
| DHT11 Temperature Sensor | Connected to pin 2 |
| Analog Turbidity Sensor | Connected to A0 |
| Analog pH Sensor | Connected to A2 |
| L298N (or similar) Motor Driver | Controls two DC motors |
| 2x DC Motors + Chassis/Wheels | Robot base |
| Power supply (battery pack) | Separate supply recommended for motors |
| Jumper wires, breadboard, etc. | Wiring |

---

## Pin Configuration

### Bluetooth (SoftwareSerial)
| Function | Arduino Pin |
|---|---|
| RX | 10 |
| TX | 11 |

### Motor Driver
| Function | Arduino Pin |
|---|---|
| Enable A (speed, Motor 1) | 3 |
| Enable B (speed, Motor 2) | 5 |
| IN1 (Motor 1 direction) | 6 |
| IN2 (Motor 1 direction) | 7 |
| IN3 (Motor 2 direction) | 8 |
| IN4 (Motor 2 direction) | 9 |

### Sensors
| Sensor | Arduino Pin |
|---|---|
| DHT11 (Temperature) | 2 |
| Turbidity Sensor | A0 |
| pH Sensor | A2 |

> **Note:** Motor speed is fixed at startup (`analogWrite(enA, 200)` / `analogWrite(enB, 200)`), giving both motors a constant PWM speed of 200/255 whenever they're enabled.

---

## Bluetooth Command Reference

Send these single characters over Bluetooth (e.g., from a phone app) to control the bot:

| Command | Action |
|---|---|
| `F` | Move Forward |
| `B` | Move Backward |
| `L` | Turn Left |
| `R` | Turn Right |
| `S` | Stop |

---

## Sensor Data Output

Every **2 seconds**, the bot reads and reports:

- **Temperature (°C)** — from the DHT11 sensor
- **pH value** — calculated from the analog voltage of the pH probe, calibrated around a neutral point of 7 at 2.5V, and clamped to the 0–14 range
- **Turbidity** — raw analog reading (0–1023) from the turbidity sensor; higher values generally indicate clearer water depending on your specific sensor's calibration

This data is sent to both:
1. **Serial Monitor** (USB, 9600 baud) — for debugging on a PC
2. **Bluetooth** (9600 baud) — for remote monitoring, in the format:
   ```
   T:<temp>,pH:<phValue>,Tb:<turbidity>
   ```

---

## Setup Instructions

1. **Wire the hardware** according to the pin configuration table above.
2. **Install the required library** in the Arduino IDE:
   - `DHT sensor library` by Adafruit (also install the `Adafruit Unified Sensor` dependency if prompted)
   - `SoftwareSerial` is built into the Arduino IDE, no installation needed
3. **Upload the sketch** to your Arduino board.
4. **Pair your phone/PC** with the HC-05/HC-06 Bluetooth module (default pairing code is often `1234` or `0000`).
5. **Connect** using a Bluetooth serial terminal app or custom controller app at **9600 baud**.
6. **Drive the bot** using the `F`, `B`, `L`, `R`, `S` commands, and watch the live sensor readings stream in.

---

## Calibration Notes

- **pH formula**: `phVal = 7 + ((2.5 - voltagePH) * 3.5)` assumes the probe outputs 2.5V at pH 7. You may need to adjust the `3.5` slope and `2.5` reference voltage based on your specific pH sensor's calibration/datasheet for accurate readings.
- **Turbidity**: The raw analog value should be calibrated against known water samples (e.g., clear water vs. muddy water) to establish meaningful thresholds for your use case.
- **DHT11**: Has a relatively low accuracy (±2°C) and slow sampling rate — this is expected behavior, not a bug.

---

## Possible Improvements

- Add humidity reading (`dht.readHumidity()`) since the DHT11 supports it but it's currently unused.
- Add speed control commands over Bluetooth instead of a fixed PWM value.
- Add error handling for failed DHT11 reads (`isnan(tempC)` check).
- Log sensor data to an SD card for offline analysis.
- Add a real-time clock (RTC) module to timestamp readings.

---

## License

Free to use and modify for personal, educational, or hobby projects.
