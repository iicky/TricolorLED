/*
  TricolorLED.cpp - Library for controlling tricolor RGB LEDs.
*/

#include <Arduino.h>
#include <TricolorLED.h>

TricolorLED::TricolorLED(int red_pin, int green_pin, int blue_pin) {

  _red_pin = red_pin;
  _green_pin = green_pin;
  _blue_pin = blue_pin;

  pinMode(_red_pin, OUTPUT);
  pinMode(_green_pin, OUTPUT);
  pinMode(_blue_pin, OUTPUT);

  state = "ON";
  effect = "solid";
  red = 255;
  green = 255;
  blue = 255;
  bright = 255;

};

float TricolorLED::_bright_scale(int value) {
  /*
    Scales brightness values from integer to percentage
  */
  return(value / 255.0);
}

void TricolorLED::change_brightness(int percent) {
  /*
    Changes brightness by a specified integer percentage.
  */

  // Scale percent change to 255
  float change = (percent / 100.0) * 255;

  // Change brightness
  if (bright + change > 255) {
    bright = 255;
  } else if (bright + change < 0){
    bright = 0;
  } else {
    bright = (int)(bright + change);
  }

  // Update color and brightness
  set_color(red, green, blue, bright);

};

void TricolorLED::power_off() {
  /*
    Powers off LEDs without overwriting color attributes.
  */

  analogWrite(_red_pin, _rgb_scale(0));
  analogWrite(_green_pin, _rgb_scale(0));
  analogWrite(_blue_pin, _rgb_scale(0));

  state = "OFF";
};

void TricolorLED::power_on() {
  /*
    Powers on LEDs using previous color attributes.
  */

  analogWrite(_red_pin, _rgb_scale(red));
  analogWrite(_green_pin, _rgb_scale(green));
  analogWrite(_blue_pin, _rgb_scale(blue));

  state = "ON";
};

void TricolorLED::print_state() {
  /*
    Prints LED attributes to Serial monitor.
  */
  Serial.print("State:\t");
  Serial.println(state);
  Serial.print("R:\t");
  Serial.println(red);
  Serial.print("G:\t");
  Serial.println(green);
  Serial.print("B:\t");
  Serial.println(blue);
  Serial.print("Bright:\t");
  Serial.println(bright);
  Serial.println("---------------------------\n");

};

int TricolorLED::_rgb_scale(int value) {
  /*
    Scales RGB values from 255 to 1023 for ESP8266 and
    apply gamma correction.
  */
  return (
    (pgm_read_byte(&_gammaTable[value]) / 255.0) * 1023.0
  );
};

void TricolorLED::set_color(int r, int g, int b, int br) {
  /*
    Sets RGB LED colors using 0 - 255 integer scale for each color channel
    and brightness.
  */

  // Set LED attributes
  red = r;
  green = g;
  blue = b;
  bright = br;

  // Change RGB colors using rgb scale
  analogWrite(_red_pin, _rgb_scale(red) * _bright_scale(bright));
  analogWrite(_green_pin, _rgb_scale(green) * _bright_scale(bright));
  analogWrite(_blue_pin, _rgb_scale(blue) * _bright_scale(bright));

  print_state();

};

const uint8_t PROGMEM TricolorLED::_gammaTable[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
    3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
    7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
   13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
   20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
   30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
   42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
   58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
   76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
   97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
  122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
  150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
  182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
  218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255};
