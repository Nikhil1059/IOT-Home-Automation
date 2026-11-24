#include <Arduino.h>
#include <WiFi.h>
#include <Matter.h>

// --- FILL IN YOUR WI-FI DETAILS ---
const char* WIFI_SSID = "xxx";
const char* WIFI_PASS = "xxx";
// ----------------------------------

const int RELAY_NORMAL  = 26;
const int RELAY_PHILIPS = 27;

bool philipsIsOn = false;
bool triggerWarmthPulse = false;

MatterOnOffLight  matterNormalLight;
MatterOnOffLight  matterPhilipsLight;
MatterOnOffPlugin matterPhilipsWarmth;

void setRelay(int pin, bool on) {
  if (pin == RELAY_PHILIPS) {
    digitalWrite(pin, on ? HIGH : LOW); // Active-HIGH on GPIO 27
  } else {
    digitalWrite(pin, on ? LOW : HIGH); // Active-LOW on GPIO 26
  }
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

  // 1. Main Light
  matterNormalLight.begin(false);
  matterNormalLight.onChange([](bool state) {
    setRelay(RELAY_NORMAL, state);
    Serial.printf("Main Light -> %s\n", state ? "ON" : "OFF");
    return true;
  });

  // 2. Mood Light (Philips 3-in-1)
  matterPhilipsLight.begin(false);
  matterPhilipsLight.onChange([](bool state) {
    philipsIsOn = state;
    setRelay(RELAY_PHILIPS, state);
    Serial.printf("Mood Light -> %s\n", state ? "ON" : "OFF");
    return true;
  });

  // 3. Change Colour Trigger
  matterPhilipsWarmth.begin(false);
  matterPhilipsWarmth.onChange([](bool state) {
    if (state) {
      triggerWarmthPulse = true;
    }
    return true;
  });

  // Start Matter stack (must be called after all endpoints begin)
  Matter.begin();

  if (!Matter.isDeviceCommissioned()) {
    Serial.println("\n=== PAIR WITH ECHO DOT (MATTER) ===");
    Serial.printf("Manual Pairing Code: %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR Code URL: %s\n", Matter.getOnboardingQRCodeUrl().c_str());
    Serial.println("===================================\n");
  } else {
    Serial.println("Matter already commissioned and ready!");
  }
}

void loop() {
  if (triggerWarmthPulse) {
    triggerWarmthPulse = false;

    if (philipsIsOn) {
      setRelay(RELAY_PHILIPS, false);
      delay(300);
      setRelay(RELAY_PHILIPS, true);
      Serial.println("Mood Light Colour Cycled!");
    } else {
      Serial.println("Ignored Change Colour: Mood Light is OFF.");
    }

    // Reset switch back to OFF after the callback finishes so Alexa can trigger it again
    matterPhilipsWarmth.setOnOff(false);
  }
}
