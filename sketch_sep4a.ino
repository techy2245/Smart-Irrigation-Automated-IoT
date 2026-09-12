/*
 * Smart Irrigation System
 *
 * Copyright (C) 2026 Shaun Stanley
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Third-party libraries used by this project retain their respective
 * copyrights and licenses.
 */
// ===================================================
// 1. Blynk Configuration
// ===================================================
#define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// ===================================================
// 2. Wi-Fi Credentials
// ===================================================
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// ===================================================
// 3. OpenWeatherMap Details
// ===================================================
String apiKey = "YOUR_OPENWEATHERMAP_API_KEY";
String city = "YOUR_CITY,YOUR_COUNTRY_CODE"; // e.g., "Bengaluru,IN"

// ===================================================
// 4. Pump (Transistor) & LED Setup
// ===================================================
#define RELAY_PIN D1
// Transistors turn ON with a HIGH signal
#define PUMP_ON HIGH
#define PUMP_OFF LOW

// ESP8266 Built-in blue LED
#define LED_PIN LED_BUILTIN 
// The built-in LED works backwards: LOW turns it on, HIGH turns it off
#define LED_ON LOW
#define LED_OFF HIGH

// ===================================================
// Global variables
// ===================================================
String weatherCondition = "";
int moisturePercent = 0;
bool isSystemActive = false;  // Default is OFF until you flip the switch in Blynk
BlynkTimer timer;

// ===================================================
// Helper Function: Controls Pump and LED together
// ===================================================
void setPump(bool turnOn) {
  if (turnOn == true) {
    digitalWrite(RELAY_PIN, PUMP_ON);
    digitalWrite(LED_PIN, LED_ON);
  } else {
    digitalWrite(RELAY_PIN, PUMP_OFF);
    digitalWrite(LED_PIN, LED_OFF);
  }
}

// ===================================================
// Master Auto/Manual Switch (V0 in Blynk app)
// ===================================================
BLYNK_WRITE(V0) {
  int switchState = param.asInt();

  if (switchState == 1) {
    isSystemActive = true;
    Serial.println("App command: Master Switch ON! AI logic taking control.");
    evaluateField(); // Instantly run the AI logic so you don't have to wait for the timer
  } else {
    isSystemActive = false;   
    setPump(false); // Instantly force the pump and LED off
    Serial.println("App command: Master Switch OFF. System disabled.");
  }
}

// ===================================================
// Fetch current weather from OpenWeatherMap
// ===================================================
void checkWeather() {
  WiFiClient client;
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

  Serial.println("Contacting Weather Satellite...");
  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      weatherCondition = doc["weather"][0]["main"].as<String>();
      Serial.println("Current Weather: " + weatherCondition);
    } else {
      Serial.println("Weather API Error: could not parse response.");
    }
  } else {
    Serial.println("Weather API Error: HTTP request failed.");
  }
  http.end();
}

// ===================================================
// Read soil moisture + AI Decision Engine
// ===================================================
void evaluateField() {
  int rawMoisture = analogRead(A0);

  // Higher raw value = drier soil (adjust after calibrating your sensor)
  moisturePercent = map(rawMoisture, 1023, 0, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Blynk.virtualWrite(V1, moisturePercent);

  Serial.print("Soil Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  // --- GATEKEEPER CHECK ---
  // If the Master Switch is off, kill the pump and stop reading code here.
  if (isSystemActive == false) {
    setPump(false);
    Serial.println("Master Switch is OFF — auto logic paused, pump is disabled.");
    Serial.println("----------------------\n");
    return;
  }

  // --- Automatic decision engine ---
  if (moisturePercent > 60) {
    setPump(false);
    Serial.println("Auto-Check: Soil is WET. Pump OFF.");
  }
  else if (moisturePercent <= 60 && 
            (weatherCondition == "Rain" || weatherCondition == "Drizzle" || weatherCondition == "Thunderstorm")) {
    setPump(false);
    Serial.println("Auto-Check: Soil DRY, but rain is coming. Pump OFF.");
  }
  else {
    setPump(true);
    Serial.println("Auto-Check: Soil DRY & clear skies. Pump ON.");
  }

  Serial.println("----------------------\n");
}

// ===================================================
// Setup
// ===================================================
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Pump and LED start OFF at boot
  setPump(false);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("\n--- System Booting ---");
  
  // Sync the Blynk app state on boot in case the ESP restarted
  Blynk.syncVirtual(V0); 
  
  checkWeather();
  evaluateField();

  timer.setInterval(2000L, evaluateField);      // check soil every 2 sec
  timer.setInterval(600000L, checkWeather);     // refresh weather every 10 min
}

// ===================================================
// Loop
// ===================================================
void loop() {
  Blynk.run();
  timer.run();
}