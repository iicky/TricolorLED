/*
  ESP controller script for RGB LEDs.
*/

// Arduino libraries
// --------------------------------------------------
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <WiFiUdp.h>

// Custom LED library
// --------------------------------------------------
#include <TricolorLED.h>

// Pins
// --------------------------------------------------
#define RED_PIN       D1
#define GREEN_PIN     D5
#define BLUE_PIN      D8
#define IR_PIN        D4

// Prototypes
// --------------------------------------------------
void wifi_setup();
void ota_setup();
void remote_set();

WiFiClient espClient;
TricolorLED rgb_led(RED_PIN, GREEN_PIN, BLUE_PIN);
IRrecv irrecv(IR_PIN);
decode_results results;

void setup() {

  // Serial setup
  Serial.begin(115200);
  Serial.println("");

  // Setup IR
  irrecv.enableIRIn();

  // Wait for LED
  delay(500);

  // Setup WiFi and OTA
  wifi_setup();
  ota_setup();

  // Set initial color and state
  rgb_led.state = "ON";
  rgb_led.set_color(255, 255, 255, 255);

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

  // Check and execute IR commands
  if (irrecv.decode(&results)){
    remote_set();
    irrecv.resume();
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

void remote_set() {

  switch (results.value) {

    // Remote Row: 1
    case 0xFF906F:
      Serial.println("Brightness Up");
      rgb_led.change_brightness(5);
      rgb_led.print_state();
      break;
    case 0xFFB847:
      Serial.println("Brightness Down");
      rgb_led.change_brightness(-5);
      rgb_led.print_state();
      break;
    case 0xFFF807:
      Serial.println("Time");
      break;
    case 0xFFB04F:
      Serial.println("Power");
      if (rgb_led.state == "ON") {
        rgb_led.power_off();
      } else {
        rgb_led.power_on();
      }
      rgb_led.print_state();
      break;

    // Remote Row: 2
    case 0xFF9867:
      Serial.println("Red");
      rgb_led.set_color(255, 0, 0, rgb_led.bright);
      break;
    case 0xFFD827:
      Serial.println("Green");
      rgb_led.set_color(0, 128, 0, rgb_led.bright);
      break;
    case 0xFF8877:
      Serial.println("Navy");
      rgb_led.set_color(0, 0, 128, rgb_led.bright);
      break;
    case 0xFFA857:
      Serial.println("White");
      rgb_led.set_color(255, 255, 255, rgb_led.bright);
      break;

    // Remote Row: 3
    case 0xFFE817:
      Serial.println("Orange");
      rgb_led.set_color(255, 69, 0, rgb_led.bright);
      break;
    case 0xFF48B7:
      Serial.println("Lime Green");
      rgb_led.set_color(50, 205, 50, rgb_led.bright);
      break;
    case 0xFF6897:
      Serial.println("Blue");
      rgb_led.set_color(0, 0, 255, rgb_led.bright);
      break;
    case 0xFFB24D:
      Serial.println("Flash");
      break;

    // Remote Row: 4
    case 0xFF02FD:
      Serial.println("Golden Rod");
      rgb_led.set_color(218, 165, 32, rgb_led.bright);
      break;
    case 0xFF32CD:
      Serial.println("Lime");
      rgb_led.set_color(0, 255, 0, rgb_led.bright);
      break;
    case 0xFF20DF:
      Serial.println("Purple");
      rgb_led.set_color(128, 0, 128, rgb_led.bright);
      break;
    case 0xFF00FF:
      Serial.println("Strobe");
      break;

    // Remote Row: 5
    case 0xFF50AF:
      Serial.println("Yellow");
      rgb_led.set_color(255, 160, 0, rgb_led.bright);
      break;
    case 0xFF7887:
      Serial.println("Teal");
      rgb_led.set_color(0, 128, 128, rgb_led.bright);
      break;
    case 0xFF708F:
      Serial.println("Dodger Blue");
      rgb_led.set_color(30, 144, 255, rgb_led.bright);
      break;
    case 0xFF58A7:
      Serial.println("Fade");
      break;

    // Remote Row: 6
    case 0xFF38C7:
      Serial.println("Yellow Green");
      rgb_led.set_color(154, 205, 50, rgb_led.bright);
      break;
    case 0xFF28D7:
      Serial.println("Sky Blue");
      rgb_led.set_color(135, 206, 235, rgb_led.bright);
      break;
    case 0xFFF00F:
      Serial.println("Blue Violet");
      rgb_led.set_color(138, 43, 226, rgb_led.bright);
      break;
    case 0xFF30CF:
      Serial.println("Smooth");
      break;
  }

  // Update MQTT state
  //update_state();

}
