/*
  TricolorLED.h - Library for controlling tricolor RGB LEDs.
*/
#ifndef TricolorLED_h
#define TricolorLED_h

#include "Arduino.h"

class TricolorLED {

  public:

    TricolorLED(int red_pin, int green_pin, int blue_pin, int pwm_range, int common_anode);
    void change_brightness(int percent);
    void hsv(int h, int s, int v);
    void off();
    void on();
    void refresh();
    void rgb(int r, int g, int b);
    void set_brightness(int value);

    int red;
    int green;
    int blue;

    int hue;
    int saturation;
    int value;

    int bright;
    String state;
    String effect;

  private:

    void _set();

    // Scaling functions for RGB and brightness
    float _bright_scale(int value);
    int _rgb_scale(int value);

    // Gamma correction table
    static const uint8_t PROGMEM _gammaTable[256];

    // RGB pins
    int _red_pin;
    int _green_pin;
    int _blue_pin;

    // PWM range and brightness
    int _pwm_range;
    int _pwm_bright;

    // Common Anode/Cathode modifier
    int _ac_mod;

    // Time and delay in milliseconds
    int _delay = 1000;
    int _time;

    // Effects
    int _direction = 1;

};

#endif
