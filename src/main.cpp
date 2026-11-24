#include <Arduino.h>
#include <WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

// --- FILL IN YOUR DETAILS ---
#define WIFI_SSID          "Airtel_kama_4438"
#define WIFI_PASS          "70329kamal14438"
#define APP_KEY            "a16b3baa-6d92-4105-9e3c-fc533b87ab81"
#define APP_SECRET         "6272def4-27ce-440c-a08d-bd5ae256e4d9-8f8a3139-1331-4200-b9b5-702805d9045f"

#define NORMAL_LIGHT_ID    "6b054648814fdde88973b701"
#define PHILIPS_LIGHT_ID   "6b0546926d3b5ef5cdcc48c2"
#define PHILIPS_WARMTH_ID  "6b0546b26d3b5ef5cdcc4912"
#include <Arduino.h>
#include <WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

const int RELAY_NORMAL = 26;
const int RELAY_PHILIPS = 27;

bool philipsIsOn = false;
bool triggerWarmthPulse = false;

void setRelay(int pin, bool on) {
  if (pin == RELAY_PHILIPS) {
    // Active-HIGH logic for GPIO 27 (HIGH = ON, LOW = OFF)
    digitalWrite(pin, on ? HIGH : LOW);
  } else {
    // Active-LOW logic for GPIO 26 (LOW = ON, HIGH = OFF)
    digitalWrite(pin, on ? LOW : HIGH);
  }
}

bool onNormalLight(const String &deviceId, bool &state) {
  Serial.printf("Normal Light -> %s\r\n", state ? "ON" : "OFF");
  setRelay(RELAY_NORMAL, state);
  return true;
}

bool onPhilipsLight(const String &deviceId, bool &state) {
  Serial.printf("Philips Light -> %s\r\n", state ? "ON" : "OFF");
  philipsIsOn = state;
  setRelay(RELAY_PHILIPS, state);
  return true;
}

bool onPhilipsWarmth(const String &deviceId, bool &state) {
  if (state) {
    if (philipsIsOn) {
      Serial.println("Warmth pulse requested!");
      triggerWarmthPulse = true;
    } else {
      Serial.println("Ignored warmth pulse: Philips Light is currently OFF.");
    }
    state = false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_NORMAL, OUTPUT);
  pinMode(RELAY_PHILIPS, OUTPUT);
  setRelay(RELAY_NORMAL, false);
  setRelay(RELAY_PHILIPS, false);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");

  SinricProSwitch& normalSwitch = SinricPro[NORMAL_LIGHT_ID];
  normalSwitch.onPowerState(onNormalLight);

  SinricProSwitch& philipsSwitch = SinricPro[PHILIPS_LIGHT_ID];
  philipsSwitch.onPowerState(onPhilipsLight);

  SinricProSwitch& warmthSwitch = SinricPro[PHILIPS_WARMTH_ID];
  warmthSwitch.onPowerState(onPhilipsWarmth);

  SinricPro.onConnected([](){ Serial.println("Connected to Sinric Pro!"); });
  SinricPro.onDisconnected([](){ Serial.println("Disconnected from Sinric Pro!"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void loop() {
  SinricPro.handle();

  if (triggerWarmthPulse) {
    triggerWarmthPulse = false;
    setRelay(RELAY_PHILIPS, false);
    delay(300);
    setRelay(RELAY_PHILIPS, true);
    Serial.println("Philips Light warmth cycled!");
  }
}
