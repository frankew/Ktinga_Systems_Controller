#pragma once
#include "Adafruit_NeoPixel.h"

// Based on Adafruit's NeoPatterns
// https://learn.adafruit.com/multi-tasking-the-arduino-part-3/using-neopatterns
//
// Pattern types supported:
enum pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, FADEFLICKER, FLICKER, SHUTTLE};
// Patern directions supported:
enum direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class FewPatterns : public Adafruit_NeoPixel
{
  public:
    FewPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)());

    // Member Variables:
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern

    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position

    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern

    void (*OnComplete)();  // Callback on completion of pattern

    // Added Functions
    void ShuttleApproach(uint8_t interval);
    void ShuttleApproachUpdate();
    void FadeFlicker(uint32_t color, uint16_t steps, uint8_t interval);
    void FadeFlickerUpdate();
    void FlickerUpdate();

    // Original NeoPatterns Functinos
    void Update();
    void Increment();
    void Decrement();
    void Reverse();
    void RainbowCycle(uint8_t interval, direction dir);
    void RainbowCycleUpdate();
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir);
    void TheaterChaseUpdate();
    void ColorWipe(uint32_t color, uint8_t interval, direction dir);
    void ColorWipeUpdate();
    void Scanner(uint32_t color1, uint8_t interval);
    void ScannerUpdate();
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir);
    void FadeUpdate();
    void ColorSet(uint32_t color);
    uint32_t DimColor(uint32_t color);
    uint32_t Wheel(byte WheelPos);
    uint8_t Red(uint32_t color);
    uint8_t Blue(uint32_t color);
    uint8_t Green(uint32_t color);
};
