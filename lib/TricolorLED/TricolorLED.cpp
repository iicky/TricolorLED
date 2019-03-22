/*
  TricolorLED.cpp - Library for controlling tricolor RGB LEDs.
*/

#include <Arduino.h>
#include <TricolorLED.h>

TricolorLED::TricolorLED(int red_pin, int green_pin, int blue_pin, int pwm_range, int common_anode) {

  _red_pin = red_pin;
  _green_pin = green_pin;
  _blue_pin = blue_pin;

  pinMode(_red_pin, OUTPUT);
  pinMode(_green_pin, OUTPUT);
  pinMode(_blue_pin, OUTPUT);

  _pwm_range = pwm_range;
  _pwm_bright = pwm_range;

  _ac_mod = (int)common_anode * _pwm_range;

  state = "OFF";
  effect = "solid";
  red = 0;
  green = 0;
  blue = 0;

  // Sync brightness with _pwm_bright
  bright = (int)((float)_pwm_bright / _pwm_range * 255.0);

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

  // Scale percent change to PWM range
  float change = (percent / 100.0) * _pwm_range;

  // Change brightness
  if (_pwm_bright + change > _pwm_range) {
    _pwm_bright = _pwm_range;
  } else if (_pwm_bright + change < 0){
    _pwm_bright = 0;
  } else {
    _pwm_bright = (int)(_pwm_bright + change);
  }

  // Keep brightness in sync
  bright = (int)((float)_pwm_bright / _pwm_range * 255.0);

};

void TricolorLED::hsv(int h, float s, float v) {
  /*
    Sets HSV values, converts and mirrors corresponding RGB values.
  */

  // Set HSV values
  hue = h;
  saturation = s;
  value = v;

  // Calculate intermediate values
  float hprime = h / 60.0;
  float c = v * s;
  float x = c * (1 - fabs(fmod(hprime, 2) - 1));
  float m = v - c;

  // Calculate rgb intermediate values
  float r; float g; float b;

  if (!hprime) {
    r = 0.0; g = 0.0; b = 0.0;
  } else if (hprime >= 0 && hprime < 1) {
    r = c; g = x; b = 0.0;
  } else if (hprime >= 1 && hprime < 2) {
    r = x; g = c; b = 0.0;
  } else if (hprime >= 2 && hprime < 3) {
    r = 0.0; g = c; b = x;
  } else if (hprime >= 3 && hprime < 4) {
    r = 0.0; g = x; b = c;
  } else if (hprime >= 4 && hprime < 5) {
    r = x; g = 0.0; b = c;
  } else if (hprime >= 5 && hprime < 6) {
    r = c; g = 0.0; b = x;
  }

  // Calculate RGB and scale
  red = (int)((r + m) * 255.0);
  green = (int)((g + m) * 255.0);
  blue = (int)((b + m) * 255.0);

}

void TricolorLED::off() {
  /*
    Powers off LEDs without overwriting color attributes.
  */

  state = "OFF";
  analogWrite(_red_pin, _ac_mod * _pwm_range);
  analogWrite(_green_pin, _ac_mod * _pwm_range);
  analogWrite(_blue_pin, _ac_mod * _pwm_range);

};

void TricolorLED::on() {
  /*
    Powers on LEDs using previous color attributes.
  */
  state = "ON";
};

void TricolorLED::rgb(int r, int g, int b) {
  /*
    Sets RGB values, converts and mirrors corresponding HSV values.
  */

  // Set RGB values
  red = r;
  green = g;
  blue = b;

}

int TricolorLED::_rgb_scale(int value) {
  /*
    Scales RGB values from 255 to PWM_RANGE and
    apply gamma correction.
  */
  return (
    (pgm_read_byte(&_gammaTable[value]) / 255.0) * _pwm_range
  );
};

void TricolorLED::refresh() {
  /*
    Refreshes light effect based on time to avoid holding up loop cycle.
  */
  if( ((int)(millis() - _time) > delay) && (state == "ON") ) {

    // Reset timer
    _time = millis();

    // Flash effect
    // Light flashes on and off
    if(effect == "flash") {

      // Turn light on or off based on direction
      if(_direction > 0) {
        bright = (int)((float)_pwm_bright / _pwm_range * 255.0);
      } else if (_direction < 0) {
        bright = 0;
      }

      // Reverse direction
      _direction = _direction * -1;

    }

    // Fade effect
    // Brightness sweeps from full to almost off
    if(effect == "fade" || effect == "fade_rainbow") {

      // Determine new fade brightness
      bright = bright + 1 * _direction;
      _fade_cap = (int)((float)_pwm_bright / _pwm_range * 255.0);

      if(bright > _fade_cap) {

        // Reached peak, reverse direction
        bright = _fade_cap;
        _direction = _direction * -1;

      } else if((bright + 10 * _direction) < 10) {

        // Reached bottom, reverse direction
        bright = 10;
        _direction = _direction * -1;

      }
    }

    // Rainbow effect
    // Lights cycle through hue, maintaining brightness
    if(effect == "rainbow" || effect == "fade_rainbow") {

      // Iterate over hue cycle
      _hue_cycle += 1;
      if(_hue_cycle >= 360) {
        _hue_cycle = 1;
      }

      // Set hue
      hsv(_hue_cycle, 1, 1);

    }

    // Set light pins each cycle
    _set();

  }
}

void TricolorLED::_set() {
  /*
    Writes pin values based on current values of red, green, blue, and bright.
  */

  // Scale brightness
  float _scaled_bright = _bright_scale(bright);

  // Scale RGB and dim by scaled brightness
  int _scaled_red = _rgb_scale(red) * _scaled_bright;
  int _scaled_green = _rgb_scale(green) * _scaled_bright;
  int _scaled_blue = _rgb_scale(blue) * _scaled_bright;

  // Change RGB colors using rgb scale
  analogWrite(_red_pin, abs(_ac_mod - _scaled_red));
  analogWrite(_green_pin, abs(_ac_mod - _scaled_green));
  analogWrite( _blue_pin, abs(_ac_mod - _scaled_blue));

};

void TricolorLED::set_brightness(int value) {
  /*
    Changes brightness to a specified integer of 0 - 255.
  */

  // Scale value change to PWM range
  float change = (value / 255.0) * _pwm_range;

  // Change brightness
  if (change > _pwm_range) {
    _pwm_bright = _pwm_range;
  } else if (change < 0){
    _pwm_bright = 0;
  } else {
    _pwm_bright = (int)(change);
  }

  // Keep brightness in sync
  bright = (int)((float)_pwm_bright / _pwm_range * 255.0);

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
