#include "NeoPixel_FewPatterns.h"

// Constructor - calls base-class constructor to initialize strip
FewPatterns::FewPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    : Adafruit_NeoPixel(pixels, pin, type)
{
    OnComplete = callback;
}

// Increment the Index and reset at the end
void FewPatterns::Increment()
{
    if (Direction == FORWARD)
    {
        Index++;
        if (Index >= TotalSteps)
        {
            Index = 0;
            if (OnComplete != NULL)
            {
                OnComplete(); // call the completion callback
            }
        }
    }
    else // Direction == REVERSE
    {
        --Index;
        if (Index <= 0)
        {
            Index = TotalSteps - 1;
            if (OnComplete != NULL)
            {
                OnComplete(); // call the completion callback
            }
        }
    }
}

// Reverse pattern direction
void FewPatterns::Reverse()
{
    if (Direction == FORWARD)
    {
        Direction = REVERSE;
        Index = TotalSteps - 1;
    }
    else
    {
        Direction = FORWARD;
        Index = 0;
    }
}

// Initialize for a RainbowCycle
void FewPatterns::RainbowCycle(uint8_t interval, direction dir = FORWARD)
{
    ActivePattern = RAINBOW_CYCLE;
    Interval = interval;
    TotalSteps = 255;
    Index = 0;
    Direction = dir;
}

// Update the Rainbow Cycle Pattern
void FewPatterns::RainbowCycleUpdate()
{
    for (int i = 0; i < numPixels(); i++)
    {
        setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
    }
    show();
    Increment();
}

// Initialize for a Theater Chase
void FewPatterns::TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
{
    ActivePattern = THEATER_CHASE;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
}

// Update the Theater Chase Pattern
void FewPatterns::TheaterChaseUpdate()
{
    for (int i = 0; i < numPixels(); i++)
    {
        if ((i + Index) % 3 == 0)
        {
            setPixelColor(i, Color1);
        }
        else
        {
            setPixelColor(i, Color2);
        }
    }
    show();
    Increment();
}

// Initialize for a ColorWipe
void FewPatterns::ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
{
    ActivePattern = COLOR_WIPE;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color;
    Index = 0;
    Direction = dir;
}

// Update the Color Wipe Pattern
void FewPatterns::ColorWipeUpdate()
{
    setPixelColor(Index, Color1);
    show();
    Increment();
}

// Initialize for a SCANNNER
void FewPatterns::Scanner(uint32_t color1, uint8_t interval)
{
    ActivePattern = SCANNER;
    Interval = interval;
    TotalSteps = (numPixels() - 1) * 2;
    Color1 = color1;
    Index = 0;
}

// Update the Scanner Pattern
void FewPatterns::ScannerUpdate()
{
    for (int i = 0; i < numPixels(); i++)
    {
        if (i == Index) // Scan Pixel to the right
        {
            setPixelColor(i, Color1);
        }
        else if (i == TotalSteps - Index) // Scan Pixel to the left
        {
            setPixelColor(i, Color1);
        }
        else // Fading tail
        {
            setPixelColor(i, DimColor(getPixelColor(i)));
        }
    }
    show();
    Increment();
}

// Initialize for a SHUTTLE
void FewPatterns::ShuttleApproach(uint8_t interval)
{
    ActivePattern = SHUTTLE;
    Interval = interval;
    TotalSteps = (numPixels() / 2);
    Color1 = Color(25, 25, 25);
    Index = numPixels();
}

bool isvalueinarray(int val, int *arr, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (arr[i] == val)
            return true;
    }
    return false;
}
// Update the ShuttleApproach Pattern
void FewPatterns::ShuttleApproachUpdate()
{
    // Front->back chasers
    for (int i = 0; i < numPixels(); i++)
    {
        int j = abs(numPixels() - i) - 1;
        if (i == Index) // Scan Pixel to the right
        {
            setPixelColor(i, Color1);
            setPixelColor(j, Color1);
        }
        else // Fading tail
        {
            setPixelColor(i, DimColor(getPixelColor(i)));
            setPixelColor(j, DimColor(getPixelColor(j)));
        }
    }

    // Corner marker flashers
    int markers[] = {0, 31, 15, 16};
    if (!isvalueinarray(Index, markers, 32))
    {
        if (Index % 5 == 0)
        {
            setPixelColor(markers[0], Wheel(20)); // green
            setPixelColor(markers[1], Wheel(20));
            setPixelColor(markers[2], Wheel(60)); // orange
            setPixelColor(markers[3], Wheel(60));
        }
    }

    show();
    Increment();
}

// Initialize for Fire (Fade with Flicker)
void FewPatterns::FadeFlicker(uint32_t color, uint16_t steps, uint8_t interval)
{
    ActivePattern = FADEFLICKER;
    Interval = interval;
    TotalSteps = steps;
    Color1 = getPixelColor(0);
    Color2 = color;
    Index = 0;
    Direction = FORWARD;
}

void FewPatterns::FadeFlickerUpdate()
{
    // Calculate linear interpolation between Color1 and Color2
    // Optimise order of operations to minimize truncation error
    uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
    uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
    uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
    uint32_t targetColor = Color(red, green, blue);

    for (int i = 0; i < numPixels(); i++)
    {
        float flicker = random(60, 100) * 0.01;
 //       uint8_t r1 = Red(targetColor);
 //       uint8_t g1 = Green(targetColor);
 //       uint8_t b1 = Blue(targetColor);
 //       uint8_t r1 = Red(targetColor);
 //       uint8_t g1 = Green(targetColor);
 //       uint8_t b1 = Blue(targetColor);


        uint32_t finalColor = Color(red * flicker, green * flicker, blue * flicker);
        setPixelColor(i, finalColor);
    }
    show();
    Increment();
}

void FewPatterns::FlickerUpdate()
{
    for (int i = 0; i < numPixels(); i++)
    {
        uint32_t currentColor = Color2;

        uint8_t r1 = Red(currentColor);
        uint8_t g1 = Green(currentColor);
        uint8_t b1 = Blue(currentColor);
        
        float flicker = random(70, 100) * 0.01;

        // Serial.print("flicker:");
        // Serial.println(flicker);

        uint32_t finalColor = Color(r1 * flicker, g1 * flicker, b1 * flicker);
        setPixelColor(i, finalColor);
    }
    show();

}

// Initialize for a Fade
void FewPatterns::Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
{
    ActivePattern = FADE;
    Interval = interval;
    TotalSteps = steps;
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
}

// Update the Fade Pattern
void FewPatterns::FadeUpdate()
{
    // Calculate linear interpolation between Color1 and Color2
    // Optimise order of operations to minimize truncation error
    uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
    uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
    uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

    ColorSet(Color(red, green, blue));
    show();
    Increment();
}

// Calculate 50% dimmed version of a color (used by ScannerUpdate)
uint32_t FewPatterns::DimColor(uint32_t color)
{
    // Shift R, G and B components one bit to the right
    float frankieFactor = 0.95;
    uint32_t dimColor = Color(Red(color) * frankieFactor, Green(color) * frankieFactor, Blue(color) * frankieFactor);
    return dimColor;
}

// Set all pixels to a color (synchronously)
void FewPatterns::ColorSet(uint32_t color)
{
    for (int i = 0; i < numPixels(); i++)
    {
        setPixelColor(i, color);
    }
    show();
}

// Returns the Red component of a 32-bit color
uint8_t FewPatterns::Red(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t FewPatterns::Green(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t FewPatterns::Blue(uint32_t color)
{
    return color & 0xFF;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t FewPatterns::Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}

// Update the pattern
void FewPatterns::Update()
{
    if ((millis() - lastUpdate) > Interval) // time to update
    {
        lastUpdate = millis();
        switch (ActivePattern)
        {
        case RAINBOW_CYCLE:
            RainbowCycleUpdate();
            break;
        case THEATER_CHASE:
            TheaterChaseUpdate();
            break;
        case COLOR_WIPE:
            ColorWipeUpdate();
            break;
        case SCANNER:
            ScannerUpdate();
            break;
        case SHUTTLE:
            ShuttleApproachUpdate();
            break;
        case FADEFLICKER:
            FadeFlickerUpdate();
            break;
        case FLICKER:
            FlickerUpdate();
            break;
        case FADE:
            FadeUpdate();
            break;
        default:
            break;
        }
    }
}
