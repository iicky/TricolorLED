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

  effect = "solid";

};

void TricolorLED::change_brightness(int percent) {
  /*
    Changes brightness by a specified integer percentage.
  */
  float change = percent / 100.0;

  // Increase brightness
  if (percent > 0) {
    if (bright + change > 1) {
      bright = 1;
    } else {
      bright += change;
    }
  }

  // Decrease brightness
  else if(percent < 0) {
    if (bright - change < 0) {
      bright = 0;
    } else {
      bright -= change;
    }
  }

  set_color(
    red,
    green,
    blue,
    bright
  );

}

void TricolorLED::power_off() {
  /*
    Powers off LEDs without overwriting color attributes.
  */

  analogWrite(_red_pin, _rgb_scale(0));
  analogWrite(_green_pin, _rgb_scale(0));
  analogWrite(_blue_pin, _rgb_scale(0));

  state = "OFF";
}

void TricolorLED::power_on() {
  /*
    Powers on LEDs using previous color attributes.
  */

  analogWrite(_red_pin, _rgb_scale(red));
  analogWrite(_green_pin, _rgb_scale(green));
  analogWrite(_blue_pin, _rgb_scale(blue));

  state = "ON";
}

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

}

int TricolorLED::_rgb_scale(int value) {
  /*
    Scales RGB values from 255 to 1023 for ESP8266
  */
  return (
    (value / 255.0) * 1023.0
  );
};

void TricolorLED::set_color(int r, int g, int b, int br) {
  /*
    Sets RGB LED colors using 0 - 255 integer scale for each color channel
    and a 0 - 1 float scale for brightness.
  */

  // Change RGB colors using rgb scale
  analogWrite(_red_pin, _rgb_scale(red));
  analogWrite(_green_pin, _rgb_scale(green));
  analogWrite(_blue_pin, _rgb_scale(blue));

  // Set LED attributes
  red = r;
  green = g;
  blue = b;
  bright = br;

  print_state();

}
