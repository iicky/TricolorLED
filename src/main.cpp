/*
  ESP controller script for RGB LEDs.
*/

// Arduino libraries
// --------------------------------------------------
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Custom LED library
// --------------------------------------------------
#include <TricolorLED.h>

// Pins
// --------------------------------------------------
#define RED_PIN       D1
#define GREEN_PIN     D5
#define BLUE_PIN      D8

// PINS
// --------------------------------------------------
WiFiClient espClient;
TricolorLED rgb_led(RED_PIN, GREEN_PIN, BLUE_PIN);
void wifi_setup();
void ota_setup();


void setup() {

  // Serial setup
  Serial.begin(115200);
  Serial.println("");

  // Wait for LED
  delay(500);

  // Setup WiFi and OTA
  wifi_setup();
  ota_setup();

  // Change color
  rgb_led.set_color(0, 0, 255, 1);
  Serial.println(rgb_led.effect);

}

void loop() {

  // Reconnect to WiFi if diconnected
  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    wifi_setup();
    return;
  }

  // Check if any OTA updates
  ArduinoOTA.handle();

  // Fade in
  for (int fade = 3; fade <= 255; fade += 3) {
    float bright = fade / 255.0;
    rgb_led.set_color(rgb_led.red, rgb_led.green, rgb_led.blue, bright);
    delay(30);
  }

  // Fade out
  for (int fade = 255; fade >= 3; fade -= 3) {
    float bright = fade / 255.0;
    rgb_led.set_color(rgb_led.red, rgb_led.green, rgb_led.blue, bright);
    delay(30);
  }

}

void wifi_setup() {

  // Connect to Wi-Fi network
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("Connected to WiFi");

}

void ota_setup() {

  ArduinoOTA.setHostname(DEVICE);
  ArduinoOTA.setPassword(OTA_PASS);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();

}
