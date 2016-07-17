/**
 * Workshop example for a simple internet button using the Losant IoT
 * platform.
 *
 * Visit https://www.losant.com/kit for full instructions.
 *
 * Copyright (c) 2016 Losant IoT. All rights reserved.
 * https://www.losant.com
 */

#include <ESP8266WiFi.h>
#include <Losant.h>

// WiFi credentials.
const char* WIFI_SSID = "crazy-ripper";
const char* WIFI_PASS = "Passw0rd";

// Losant credentials.
const char* LOSANT_DEVICE_ID = "578acbd26e6a550100c80397";
const char* LOSANT_ACCESS_KEY = "320b142c-da55-4d75-80a7-36498448133d";
const char* LOSANT_ACCESS_SECRET = "203200b251216fe05d0053b2eaa670ba07478f6b43d014df5cdad98972091007";

const int BUTTON_PIN = 4;

WiFiClientSecure wifiClient;

LosantDevice device(LOSANT_DEVICE_ID);

void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  Serial.print("Connecting to Losant...");

  device.connectSecure(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

  unsigned long connectionStart = millis();
  while(!device.connected()) {
    delay(500);
    Serial.print(".");

    // If we can't connect after 5 seconds, restart the board.
    if(millis() - connectionStart > 5000) {
      Serial.println("Failed to connect to Losant, restarting board.");
      ESP.restart();
    }
  }

  Serial.println("Connected!");
  Serial.println("This device is now ready for use!");
}

void setup() {
  Serial.begin(115200);

  // Giving it a little time because the serial monitor doesn't
  // immediately attach. Want the workshop that's running to
  // appear on each upload.
  delay(2000);

  Serial.println();
  Serial.println("Running Workshop 1 Firmware.");

  pinMode(BUTTON_PIN, INPUT);
  connect();
}

void buttonPressed() {
  Serial.println("Button Pressed!");

  // Losant uses a JSON protocol. Construct the simple state object.
  // { "button" : true }
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["PushButton"] = true;

  // Send the state to Losant.
  device.sendState(root);
}

int buttonState = 0;

void loop() {

  bool toReconnect = false;

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from WiFi");
    toReconnect = true;
  }

  if(!device.connected()) {
    Serial.println("Disconnected from Losant");
    Serial.println(device.mqttClient.state());
    toReconnect = true;
  }

  if(toReconnect) {
    connect();
  }

  device.loop();

  int currentRead = digitalRead(BUTTON_PIN);

  if(currentRead != buttonState) {
    buttonState = currentRead;
    if(buttonState) {
      buttonPressed();
    }
  }

  delay(100);
}
