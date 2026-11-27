#include <Arduino.h>
#include <WiFi.h>
#include <Matter.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// --- FILL IN YOUR WI-FI DETAILS ---
const char* WIFI_SSID = "Wifi";
const char* WIFI_PASS = "WifiPass";
// ----------------------------------

const int RELAY_NORMAL   = 26;
const int RELAY_PHILIPS  = 27;
const uint16_t IR_TX_PIN = 4;

// Captured Atomberg raw IR timing arrays (38 kHz)
uint16_t rawFanPower[71] = {
  9078, 4432,  610, 546,  582, 552,  620, 514,  646, 488,  584, 550,
  586, 550,  618, 516,  620, 514,  618, 1624,  646, 1598,  644, 492,
  620, 514,  646, 1598,  646, 1576,  638, 1626,  620, 1624,  612, 1630,
  642, 492,  614, 520,  614, 522,  616, 1626,  616, 518,  614, 520,
  616, 1626,  614, 520,  614, 1630,  612, 1632,  608, 1634,  614, 520,
  612, 1632,  610, 1634,  608, 526,  584, 42008,  9078, 2216, 584
};

uint16_t rawFanSpeed1[67] = {
  9080, 4460,  614, 522,  614, 520,  610, 526,  614, 522,  608, 526,
  614, 522,  612, 522,  612, 522,  640, 1604,  608, 1636,  606, 528,
  618, 518,  614, 1630,  638, 1606,  612, 1632,  612, 1630,  612, 1630,
  582, 1662,  612, 522,  612, 1630,  642, 494,  582, 552,  640, 494,
  606, 1636,  614, 520,  606, 528,  612, 1632,  612, 522,  640, 1602,
  640, 1604,  612, 1632,  606, 528,  578
};

uint16_t rawFanSpeed2[71] = {
  9082, 4450,  642, 492,  610, 524,  616, 496,  664, 492,  618, 516,
  610, 524,  612, 522,  642, 492,  618, 1626,  618, 1624,  640, 496,
  612, 524,  644, 1598,  616, 1628,  616, 1628,  616, 1604,  666, 492,
  616, 520,  612, 500,  666, 470,  662, 1600,  618, 516,  614, 520,
  642, 1600,  616, 1628,  618, 1624,  640, 1604,  644, 1598,  644, 492,
  610, 1632,  614, 1628,  638, 496,  614, 41968,  9114, 2182, 640
};

uint16_t rawFanSpeed3[67] = {
  9080, 4458,  612, 524,  586, 548,  586, 548,  610, 524,  614, 520,
  610, 524,  586, 548,  586, 548,  586, 1656,  614, 1630,  586, 548,
  614, 520,  586, 1658,  586, 1656,  614, 1630,  612, 1632,  612, 522,
  614, 1630,  610, 524,  586, 1656,  610, 524,  584, 548,  586, 548,
  586, 1656,  586, 1656,  614, 522,  610, 1632,  586, 550,  586, 1656,
  586, 1656,  612, 1630,  612, 522,  586
};

uint16_t rawFanSpeed4[67] = {
  9076, 4454,  618, 518,  614, 520,  616, 520,  616, 516,  614, 522,
  618, 516,  616, 518,  616, 498,  636, 1628,  618, 1626,  618, 518,
  616, 518,  616, 1628,  616, 1606,  636, 1630,  618, 1626,  620, 1624,
  618, 1626,  614, 520,  618, 518,  618, 1626,  618, 518,  616, 516,
  588, 1656,  618, 516,  616, 518,  616, 1626,  590, 1654,  588, 526,
  638, 1604,  640, 1604,  638, 518,  616
};

uint16_t rawFanSpeed5[71] = {
  9070, 4456,  640, 494,  614, 522,  640, 494,  608, 524,  642, 494,
  640, 494,  586, 548,  640, 494,  610, 1634,  610, 1634,  640, 494,
  642, 492,  642, 1600,  616, 1626,  612, 1632,  616, 1626,  616, 518,
  610, 524,  614, 520,  616, 1626,  614, 520,  616, 518,  640, 494,
  612, 1632,  610, 1632,  614, 1628,  618, 1626,  612, 522,  642, 1602,
  616, 1626,  642, 1602,  614, 520,  616, 41964,  9082, 2212, 616
};

uint16_t rawFanBoost[71] = {
  9070, 4454,  614, 522,  612, 522,  610, 524,  610, 524,  612, 522,
  612, 522,  610, 524,  612, 522,  612, 1630,  614, 1630,  612, 522,
  614, 522,  612, 1628,  614, 1630,  612, 1630,  614, 1630,  612, 1630,
  614, 1630,  614, 1630,  612, 1630,  612, 524,  612, 524,  610, 522,
  612, 1630,  612, 524,  612, 522,  612, 522,  612, 522,  610, 1632,
  612, 1630,  614, 1632,  612, 522,  608, 41978,  9084, 2210, 640
};

uint16_t rawFanLed[71] = {
  9076, 4434,  638, 518,  642, 470,  634, 522,  618, 516,  640, 472,
  636, 520,  644, 492,  616, 518,  640, 1604,  638, 1584,  662, 494,
  616, 518,  616, 1626,  642, 1578,  664, 1600,  616, 1606,  664, 1600,
  640, 1602,  612, 1630,  640, 494,  614, 1628,  616, 518,  612, 522,
  644, 1600,  642, 492,  620, 516,  618, 516,  642, 1602,  642, 494,
  644, 1598,  612, 1630,  616, 518,  638, 41952,  9090, 2206, 586
};

IRsend irsend(IR_TX_PIN);

bool philipsIsOn = false;
bool triggerWarmthPulse = false;

volatile bool pendingFanPowerCmd = false;
volatile bool pendingFanSpeedCmd = false;
volatile uint8_t targetFanSpeedPercent = 0;
volatile bool triggerFanLed = false;
volatile bool triggerFanBoost = false;

unsigned long lastIrSendMs = 0;
const unsigned long IR_COOLDOWN_MS = 1200;

MatterOnOffLight  matterNormalLight;
MatterOnOffLight  matterPhilipsLight;
MatterOnOffPlugin matterPhilipsWarmth;
MatterFan         matterFan;
MatterOnOffPlugin matterFanLed;
MatterOnOffPlugin matterFanBoost;

void setRelay(int pin, bool on) {
  if (pin == RELAY_PHILIPS) {
    digitalWrite(pin, on ? HIGH : LOW); // Active-HIGH on GPIO 27
  } else {
    digitalWrite(pin, on ? LOW : HIGH); // Active-LOW on GPIO 26
  }
}

bool canSendIrNow() {
  if (millis() - lastIrSendMs < IR_COOLDOWN_MS) {
    return false;
  }
  lastIrSendMs = millis();
  return true;
}

void sendRawFrame(const uint16_t* buf, uint16_t len, const char* label) {
  irsend.sendRaw(buf, len, 38);
  Serial.printf("IR Atomberg [%s] sent (%u pulses)\n", label, len);
}

void sendSpeedFromPercent(uint8_t percent) {
  if (percent <= 20) {
    sendRawFrame(rawFanSpeed1, 67, "Speed 1");
  } else if (percent <= 40) {
    sendRawFrame(rawFanSpeed2, 71, "Speed 2");
  } else if (percent <= 60) {
    sendRawFrame(rawFanSpeed3, 67, "Speed 3");
  } else if (percent <= 80) {
    sendRawFrame(rawFanSpeed4, 67, "Speed 4");
  } else {
    sendRawFrame(rawFanSpeed5, 71, "Speed 5");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_NORMAL, OUTPUT);
  pinMode(RELAY_PHILIPS, OUTPUT);
  setRelay(RELAY_NORMAL, false);
  setRelay(RELAY_PHILIPS, false);

  irsend.begin();

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

  // 4. Atomberg Fan Power + Speed 1-5
  matterFan.begin(0, MatterFan::FAN_MODE_OFF, MatterFan::FAN_MODE_SEQ_OFF_HIGH);
  matterFan.onChange([](MatterFan::FanMode_t mode, uint8_t percent) {
    pendingFanPowerCmd = true;
    if (mode != MatterFan::FAN_MODE_OFF && percent > 0) {
      targetFanSpeedPercent = percent;
      pendingFanSpeedCmd = true;
    }
    return true;
  });

  // 5. Fan LED Toggle Switch (triggers on both ON and OFF)
  matterFanLed.begin(false);
  matterFanLed.onChange([](bool state) {
    triggerFanLed = true;
    Serial.printf("Fan LED switch -> %s\n", state ? "ON" : "OFF");
    return true;
  });

  // 6. Fan Boost Toggle Switch (triggers on both ON and OFF)
  matterFanBoost.begin(false);
  matterFanBoost.onChange([](bool state) {
    triggerFanBoost = true;
    Serial.printf("Fan Boost switch -> %s\n", state ? "ON" : "OFF");
    return true;
  });

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

    matterPhilipsWarmth.setOnOff(false);
  }

  if (pendingFanPowerCmd) {
    pendingFanPowerCmd = false;
    if (canSendIrNow()) {
      sendRawFrame(rawFanPower, 71, "Power");
    }
  }

  if (pendingFanSpeedCmd) {
    pendingFanSpeedCmd = false;
    if (canSendIrNow()) {
      sendSpeedFromPercent(targetFanSpeedPercent);
    }
  }

  if (triggerFanLed) {
    triggerFanLed = false;
    if (canSendIrNow()) {
      sendRawFrame(rawFanLed, 71, "LED");
      delay(30);
      sendRawFrame(rawFanLed, 71, "LED");
    }
  }

  if (triggerFanBoost) {
    triggerFanBoost = false;
    if (canSendIrNow()) {
      sendRawFrame(rawFanBoost, 71, "Boost");
      delay(30);
      sendRawFrame(rawFanBoost, 71, "Boost");
    }
  }

  delay(10);
}
