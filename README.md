
# 🏠 KotiHub – ESP32 Smart Home Dashboard

KotiHub is an ESP32-powered smart home information hub that displays real-time data about **indoor climate**, **local weather**, **electricity spot prices**, and **cryptocurrency markets**.

Data can be viewed either on an **onboard OLED display** or through a **responsive web dashboard** hosted directly on the ESP32.
<img width="1180" height="841" alt="image" src="https://github.com/user-attachments/assets/f793e157-ec9c-43cf-b49e-7adfc1caaace" />

---

## ✨ Features

- 🌡️ **Indoor climate** – Temperature & humidity (DHT11)
- ⛅ **Local weather** – Temperature & wind speed (Open-Meteo, Mikkeli)
- ⚡ **Electricity spot price** – Current hourly price (incl. VAT, UTC synced)
- 🪙 **Crypto tracking** – XRP, XLM, ETH, XDC, ADA (+24h change)
- 📱 **Web dashboard** – Responsive Glassmorphism UI (mobile & desktop)
- 📺 **OLED UI** – Multiple views + screensaver
- 🎮 **IR remote control** – Full device control

---

## 🛠️ Hardware

- ESP32 Development Board (ESP32-WROOM-32)
- 0.96" OLED Display (SSD1306, I2C)
- DHT11 Temperature & Humidity Sensor
- IR Receiver (e.g. VS1838B)
- Jumper wires & breadboard

---

## 🔌 Pinout

| Component   | ESP32 Pin | Notes |
|------------|-----------|-------|
| OLED SDA   | GPIO 21   | I2C Data |
| OLED SCL   | GPIO 22   | I2C Clock |
| DHT11 Data | GPIO 4    | Sensor |
| IR Receiver| GPIO 27   | IR Signal |
| VCC / GND  | 3.3V / GND| Power |

---

## 📂 Project Structure

```text
├── DHT11.ino      # Main application (ESP32 logic)
├── functions.h   # API fetching & data processing
└── dashboard.h   # Web UI (HTML/CSS/JS in PROGMEM)
````

### File Responsibilities

**DHT11.ino**

* Hardware setup & main loop
* OLED rendering
* IR command handling
* WebSocket server

**functions.h**

* Weather, electricity & crypto API requests
* JSON parsing
* NTP time synchronization

**dashboard.h**

* Web dashboard UI
* Glassmorphism styling
* Responsive layout
* Live WebSocket updates

---

## 🚀 Installation

### 1. Requirements

Install the following libraries via **Arduino Library Manager**:

* Adafruit GFX Library
* Adafruit SSD1306
* DHT sensor library
* ArduinoJson
* IRremote
* WebSockets (Markus Sattler)

---

### 2. WiFi Configuration

Edit `DHT11.ino`:

```cpp
WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
```

(Optional)
Change location in `functions.h` if not using Mikkeli (Open-Meteo).

---

### 3. Upload & Run

1. Connect ESP32 to PC
2. Select correct board & port
3. Upload sketch
4. Open Serial Monitor (115200 baud)
5. Copy printed IP address
6. Open IP in browser to access dashboard

---

## 🎮 Controls

### IR Remote

| Button       | Action                    |
| ------------ | ------------------------- |
| CH+ / CH-    | Change OLED view          |
| LEFT / RIGHT | Scroll cryptos            |
| OK           | Force data refresh        |
| POWER        | Toggle OLED (screensaver) |

### Web Dashboard

* Real-time data view
* **Refresh button** with loading animation & success indicator

---

## 🌐 APIs Used

- **Weather:**  
  [Open-Meteo – Mikkeli Forecast](https://api.open-meteo.com/v1/forecast?latitude=61.6886&longitude=27.2723&current_weather=true&windspeed_unit=ms)

- **Electricity:**  
  [Pörssisähkö – Latest Prices](https://api.porssisahko.net/v2/latest-prices.json)

- **Crypto:**  
  [CryptoCompare – Multi-Full Price Data](https://min-api.cryptocompare.com/data/pricemultifull?fsyms=XRP,XLM,ETH,XDC,ADA&tsyms=EUR)

*(No API keys required)*

---

## 🔧 Troubleshooting

* **Electricity price = 0** → Check NTP time sync (UTC)
* **IR not working** → Verify IR codes in Serial Monitor

---

