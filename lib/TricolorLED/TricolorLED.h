/*
  TricolorLED.h - Library for controlling tricolor RGB LEDs.
*/
#ifndef TricolorLED_h
#define TricolorLED_h

#include "Arduino.h"

class TricolorLED {

  public:

    TricolorLED(int red_pin, int green_pin, int blue_pin);
    void set_color(int red, int green, int blue, float bright);
    void power_off();
    void power_on();

    int red;
    int green;
    int blue;
    int bright;
    String effect;

  private:

    int _rgb_scale(int value);

    // RGB pins
    int _red_pin;
    int _green_pin;
    int _blue_pin;

};

#endif
