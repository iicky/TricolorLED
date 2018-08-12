/*
  ESP controller script for RGB LEDs.
*/

// Arduino libraries
// --------------------------------------------------
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <PubSubClient.h>
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

// JSON
// --------------------------------------------------
const int BUFFER_SIZE = JSON_OBJECT_SIZE(10);

// Prototypes
// --------------------------------------------------
void wifi_setup();
void ota_setup();
void remote_set();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
bool set_state(char *message);
void update_state();

WiFiClient espClient;
PubSubClient client(espClient);
IRrecv irrecv(IR_PIN);
decode_results results;

TricolorLED rgb_led(RED_PIN, GREEN_PIN, BLUE_PIN);

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

  // Setup MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  // Set initial color and state
  rgb_led.state = "ON";
  rgb_led.set_color(255, 255, 255, 255);

}

void loop() {

  // Reconnect to MQTT if disconnected
  if (!client.connected()) {
    reconnect();
  }

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

  // Check MQTT
  client.loop();

  // Refresh LED
  rgb_led.refresh();

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
      update_state();
      break;
    case 0xFFB847:
      Serial.println("Brightness Down");
      rgb_led.change_brightness(-5);
      rgb_led.print_state();
      update_state();
      break;
    case 0xFFF807:
      Serial.println("Time");
      rgb_led.effect = "solid";
      update_state();
      break;
    case 0xFFB04F:
      Serial.println("Power");
      if (rgb_led.state == "ON") {
        rgb_led.power_off();
      } else {
        rgb_led.power_on();
      }
      rgb_led.print_state();
      update_state();
      break;

    // Remote Row: 2
    case 0xFF9867:
      Serial.println("Red");
      rgb_led.set_color(255, 0, 0, rgb_led.bright);
      update_state();
      break;
    case 0xFFD827:
      Serial.println("Green");
      rgb_led.set_color(0, 128, 0, rgb_led.bright);
      update_state();
      break;
    case 0xFF8877:
      Serial.println("Navy");
      rgb_led.set_color(0, 0, 128, rgb_led.bright);
      update_state();
      break;
    case 0xFFA857:
      Serial.println("White");
      rgb_led.set_color(255, 255, 255, rgb_led.bright);
      update_state();
      break;

    // Remote Row: 3
    case 0xFFE817:
      Serial.println("Orange");
      rgb_led.set_color(255, 69, 0, rgb_led.bright);
      update_state();
      break;
    case 0xFF48B7:
      Serial.println("Lime Green");
      rgb_led.set_color(50, 205, 50, rgb_led.bright);
      update_state();
      break;
    case 0xFF6897:
      Serial.println("Blue");
      rgb_led.set_color(0, 0, 255, rgb_led.bright);
      update_state();
      break;
    case 0xFFB24D:
      Serial.println("Flash");
      update_state();
      break;

    // Remote Row: 4
    case 0xFF02FD:
      Serial.println("Golden Rod");
      rgb_led.set_color(218, 165, 32, rgb_led.bright);
      update_state();
      break;
    case 0xFF32CD:
      Serial.println("Lime");
      rgb_led.set_color(0, 255, 0, rgb_led.bright);
      update_state();
      break;
    case 0xFF20DF:
      Serial.println("Purple");
      rgb_led.set_color(128, 0, 128, rgb_led.bright);
      update_state();
      break;
    case 0xFF00FF:
      Serial.println("Strobe");
      update_state();
      break;

    // Remote Row: 5
    case 0xFF50AF:
      Serial.println("Yellow");
      rgb_led.set_color(255, 160, 0, rgb_led.bright);
      update_state();
      break;
    case 0xFF7887:
      Serial.println("Teal");
      rgb_led.set_color(0, 128, 128, rgb_led.bright);
      update_state();
      break;
    case 0xFF708F:
      Serial.println("Dodger Blue");
      rgb_led.set_color(30, 144, 255, rgb_led.bright);
      update_state();
      break;
    case 0xFF58A7:
      Serial.println("Fade");
      rgb_led.effect = "fade";
      update_state();
      break;

    // Remote Row: 6
    case 0xFF38C7:
      Serial.println("Yellow Green");
      rgb_led.set_color(154, 205, 50, rgb_led.bright);
      update_state();
      break;
    case 0xFF28D7:
      Serial.println("Sky Blue");
      rgb_led.set_color(135, 206, 235, rgb_led.bright);
      update_state();
      break;
    case 0xFFF00F:
      Serial.println("Blue Violet");
      rgb_led.set_color(138, 43, 226, rgb_led.bright);
      update_state();
      break;
    case 0xFF30CF:
      Serial.println("Smooth");
      update_state();
      break;
  }

}

void reconnect() {

  // Loop until we're reconnected
  while (!client.connected()) {

    // Attempt to connect
    if (client.connect(DEVICE, MQTT_USER, MQTT_PASS)) {

      // Subscribe to topic
      client.subscribe(MQTT_TOPIC "/set");

      // Update state
      update_state();

    } else {

      // Retry in 5 seconds
      delay(5000);

    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  // Process message
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  // Parse JSON
  if(!set_state(message))
    return;

  // Update LED state
  update_state();

}

bool set_state(char *message) {

  // Parse JSON from MQTT message
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(message);

  // Parse JSON failed
  if(!root.success()) {
    Serial.println("Could not parse JSON.");
    return false;
  }

  // Parse on/off state
  if (root.containsKey("state")) {
    if (root["state"] == "ON") {
      if (rgb_led.state != "ON")
        rgb_led.power_on();
    }
    else if (root["state"] == "OFF") {
      rgb_led.power_off();
    }
    else {
      update_state();
      return false;
    }
  }

  // Parse color state
  if (root.containsKey("color")) {
    int r = root["color"]["r"];
    int g = root["color"]["g"];
    int b = root["color"]["b"];
    rgb_led.set_color(r, g, b, rgb_led.bright);
  }

  // Parse brightness state
  if (root.containsKey("brightness")) {
    int br = root["brightness"];
    rgb_led.set_color(rgb_led.red, rgb_led.green, rgb_led.blue, br);
  }

  // Set effect state
  if (root.containsKey("effect")) {
    const char* effect = root["effect"];
    rgb_led.effect = effect;
  }

  return true;

}

void update_state() {

  // Create JSON object
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  // Set LED state
  root["state"] = rgb_led.state;

  // Set color states
  JsonObject &color = root.createNestedObject("color");
  color["r"] = rgb_led.red;
  color["g"] = rgb_led.green;
  color["b"] = rgb_led.blue;

  // Set brightness and effect states
  root["brightness"] = rgb_led.bright;
  root["effect"] = rgb_led.effect.c_str();

  // Publish JSON
  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));
  client.publish(MQTT_TOPIC, buffer, true);

}
