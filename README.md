# Smart-Irrigation-Automated-IoT
1st Place winner for Working Model at Christ Junior College Cosmos v10.0 (Computer Science Dept). An IoT irrigation system that automates water delivery using ESP8266 hardware, soil moisture sensors, and live OpenWeatherMap data.
# 🌿 Smart Irrigation System (IoT Edition)
An automated, solar-powered agricultural model that monitors soil moisture and pulls live weather data to water crops intelligently. Built with an ESP8266 and integrated with a custom Blynk IoT dashboard.

## 🏆 Project Status: 1st Place Science Model
This repository contains the hardware architecture and software logic for our award-winning smart agriculture prototype. 

## ⚙️ Core Features (v1.0)
- **Live Weather API Integration:** Connects to OpenWeatherMap to check current conditions. If soil is dry but rain is detected, the system intelligently pauses irrigation to conserve water.
- **Hardware Protection Topology:** Custom bypass wiring with a transistor/relay to safely handle high-current motor spikes from the water pump.
- **Master Override Switch:** Cloud-connected IoT gatekeeper via the Blynk app to manually kill or activate the system from anywhere in the world.

## 🛠️ Hardware Requirements
- NodeMCU ESP8266 (ESP-12E)
- Analog Soil Moisture Sensor
- Submersible Water Pump (5V)
- TIP122 Transistor / Relay Module (for pump isolation)
- 18650 Battery & Solar Panel
- TP4056 Battery Charger & MT3608 Voltage Booster

## 🔌 Circuit & Wiring Schematic
*(Drag and drop your wiring diagram photo here)*

## 💻 Software Installation
1. Install the **Arduino IDE** and add the ESP8266 board manager.
2. Install the following required libraries via the Library Manager:
   - `BlynkSimpleEsp8266`
   - `ArduinoJson`
3. Open `sketch_sep4a.ino` and update the Wi-Fi credentials, Blynk Auth Token, and OpenWeatherMap API Key.
4. Upload to the ESP8266 on **115200 baud rate**.

## 📱 Blynk App Setup
- **V0 (Button/Switch):** Master Auto/Manual Mode (0 = Kill Switch, 1 = AI Auto Mode)
- **V1 (Gauge):** Live Soil Moisture Percentage (0-100%)

## 🚀 Future Roadmap (v2.0: Machine Learning AI)
We are actively developing our next major update, which will replace the basic weather API with **Predictive Machine Learning AI**. The future system will analyze historical weather patterns, soil evaporation rates, and crop types to predict exact irrigation times, creating a fully autonomous, data-driven farming ecosystem.

## 📜 License
This project is dual-licensed. You may choose to use it under the terms of the **MIT License** OR the **GNU General Public License v3.0 (GPLv3)**. See the `LICENSE-MIT` and `LICENSE-GPL` files in this repository for full details.
