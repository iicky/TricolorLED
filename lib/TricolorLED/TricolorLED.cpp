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

  TricolorLED::effect = "solid";

};

int TricolorLED::_rgb_scale(int value) {
  /*
    Scales RGB values from 255 to 1023 for ESP8266
  */
  return (
    (value / 255.0) * 1023.0
  );
};

void TricolorLED::set_color(int red, int green, int blue, float bright) {
  /*
    Sets RGB LED colors using 0 - 255 scale.
  */

  // Change RGB colors using rgb scale
  analogWrite(_red_pin, TricolorLED::_rgb_scale(red) * bright);
  analogWrite(_green_pin, TricolorLED::_rgb_scale(green) * bright);
  analogWrite(_blue_pin, TricolorLED::_rgb_scale(blue) * bright);

  // Set LED attributes
  TricolorLED::red = red;
  TricolorLED::green = green;
  TricolorLED::blue = blue;
  TricolorLED::bright = bright;

}

void TricolorLED::power_off() {
  /*
    Powers off LEDs without overwriting color attributes.
  */

  analogWrite(_red_pin, TricolorLED::_rgb_scale(0));
  analogWrite(_green_pin, TricolorLED::_rgb_scale(0));
  analogWrite(_blue_pin, TricolorLED::_rgb_scale(0));

}

void TricolorLED::power_on() {
  /*
    Powers on LEDs using previous color attributes.
  */

  analogWrite(_red_pin, TricolorLED::_rgb_scale(TricolorLED::red));
  analogWrite(_green_pin, TricolorLED::_rgb_scale(TricolorLED::green));
  analogWrite(_blue_pin, TricolorLED::_rgb_scale(TricolorLED::blue));

}

void TricolorLED::refresh() {
  /*
    Refreshes LED status
  */


}
