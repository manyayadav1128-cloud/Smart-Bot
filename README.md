## Smart-Bot 🤖📡


An ESP8266-based autonomous robot that navigates using infrared sensors (object avoidance + edge/line detection), measures its own speed, and publishes live **temperature**, **humidity**, and **speed** data to **Adafruit IO** over WiFi/MQTT for remote monitoring and dashboards.

---

## Features

- **Autonomous navigation**
  - Obstacle detection & avoidance (reverses and turns when an object is detected ahead)
  - Edge/line detection using two IR sensors for left/right/forward steering and stop-and-reverse at edges
- **Speed measurement** — calculates real-time speed (m/s) using an IR sensor and a known reference object length
- **Environmental monitoring** — reads ambient temperature and humidity via a DHT11 sensor
- **Cloud connectivity** — publishes temperature, humidity, and speed data to **Adafruit IO** via MQTT for live dashboards and logging
- **Dual DC motor drive** using an L298N-style motor driver

---

## Hardware Required

| Component | Notes |
|---|---|
| ESP8266 (NodeMCU / Wemos D1 Mini) | Main controller with built-in WiFi |
| DHT11 Temperature & Humidity Sensor | Connected to D4 |
| 4x IR Sensors | Object detection, edge detection (x2), speed measurement |
| L298N (or similar) Motor Driver | Controls two DC motors |
| 2x DC Motors + Chassis/Wheels | Robot base |
| Power supply (battery pack) | Separate supply recommended for motors |
| Jumper wires, breadboard, etc. | Wiring |

---

## Pin Configuration

### IR Sensors
| Function | ESP8266 Pin |
|---|---|
| IR1 – Object Detection (front) | D0 |
| IR2 – Left Edge Detection | D1 |
| IR3 – Right Edge Detection | D2 |
| IR4 – Speed Measurement | D3 |

### Motor Driver (L298N)
| Function | ESP8266 Pin |
|---|---|
| IN1 (Motor 1 direction) | D5 |
| IN2 (Motor 1 direction) | D6 |
| IN3 (Motor 2 direction) | D7 |
| IN4 (Motor 2 direction) | D8 |

### Sensors
| Sensor | ESP8266 Pin |
|---|---|
| DHT11 (Temperature/Humidity) | D4 |

---

## Cloud Setup (Adafruit IO)

This project publishes to three **Adafruit IO** feeds via MQTT:

| Feed Key | Data |
|---|---|
| `R1` | Temperature (°C) |
| `G1` | Humidity (%) |
| `S1` | Speed (m/s) |

### Before uploading, update these values in the code:

```cpp
#define WLAN_SSID       "ssid"        // Your WiFi network name
#define WLAN_PASS       "password"    // Your WiFi password

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  port number   // Typically 1883 (unencrypted) or 8883 (SSL)
#define AIO_USERNAME    "username"    // Your Adafruit IO username
#define AIO_KEY         "key"         // Your Adafruit IO key
```

1. Create a free account at [io.adafruit.com](https://io.adafruit.com).
2. Create three feeds named `R1`, `G1`, and `S1` (or update the feed names in the code to match your own).
3. Grab your **Username** and **Active Key** from the Adafruit IO dashboard and paste them into the sketch.
4. Optionally, build an Adafruit IO dashboard with gauges/charts for each feed to visualize data live.

---

## How It Works

### Navigation Logic

- **Object Detection (IR1):** If an object is detected ahead, the bot briefly stops, then reverses and turns before continuing. If clear, it drives forward.
- **Edge Detection (IR2 + IR3):**
  - Both sensors triggered → stop, then reverse
  - Left sensor triggered only → turn right
  - Right sensor triggered only → turn left
  - Neither triggered → drive forward

> **Note:** The obstacle-avoidance block and the edge-detection block both write to the same motor pins every loop iteration, with edge-detection logic running after (and effectively overriding) object-detection logic. Review and tune this priority order based on your desired behavior.

### Speed Measurement

- When an object breaks the IR4 beam, a start timestamp is recorded.
- When the object clears the beam, an end timestamp is recorded.
- Speed is calculated as `objectLength / timeSec`, where `objectLength` is a fixed reference distance (default: **0.10 m**) — adjust this constant to match your actual sensor/object setup.
- The result is published to the `S1` feed.

### Environmental Data

- Every loop cycle (~2 seconds), the DHT11 is read for temperature and humidity.
- If the read fails (`NaN`), it's logged and skipped for that cycle.
- Valid readings are published to the `R1` (temperature) and `G1` (humidity) feeds.

---

## Setup Instructions

1. **Install ESP8266 board support** in the Arduino IDE (via Boards Manager, if not already installed).
2. **Install the required libraries**:
   - `DHT sensor library` by Adafruit (+ `Adafruit Unified Sensor` dependency)
   - `Adafruit MQTT Library`
   - `ESP8266WiFi` (bundled with ESP8266 board package)
3. **Wire the hardware** according to the pin configuration tables above.
4. **Update WiFi and Adafruit IO credentials** in the code (see Cloud Setup above).
5. **Upload the sketch** to your ESP8266 board.
6. **Open the Serial Monitor** at 115200 baud to confirm WiFi connection and view live sensor data.
7. **Check your Adafruit IO dashboard** to see live temperature, humidity, and speed data streaming in.

---

## Calibration Notes

- **`objectLength`**: Must match the real-world width of whatever passes the speed sensor for accurate speed readings — recalibrate for your specific setup.
- **DHT11**: Has limited accuracy (±2°C, ±5% RH) and a slow sampling rate (~1 reading/sec) — this is expected sensor behavior, not a bug.
- **IR sensors**: Threshold sensitivity is usually adjustable via an onboard potentiometer — tune for your surface/lighting conditions.

---

## Possible Improvements

- Resolve the overlap between the object-detection and edge-detection motor control blocks so priority is explicit (e.g., using `else if` or a state machine).
- Add non-blocking timing (`millis()`-based) instead of `delay()` in navigation logic, so sensor reads and MQTT publishing stay responsive.
- Add a feed for obstacle/edge events to track navigation behavior remotely.
- Add OTA (Over-The-Air) update support for easier firmware updates.
- Add SSL/TLS (port 8883) for encrypted MQTT communication with Adafruit IO.

---

## License

Free to use and modify for personal, educational, or hobby projects.
