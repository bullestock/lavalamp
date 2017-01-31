#include <Adafruit_NeoPixel.h>

#define RGB_PIN     2
#define NUMPIXELS   3

// Flash LED for debugging
#define FLASH_LED      1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, RGB_PIN, NEO_RGB + NEO_KHZ800);

int delayval = 20;

void setup()
{
    pixels.begin();
    pinMode(1, OUTPUT); // LED
}

uint32_t Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}

int led_state = 0;
int colour = 0;
void loop()
{
#if FLASH_LED
    digitalWrite(1, led_state);
    led_state = !led_state;
#endif
    for (int i = 0; i < NUMPIXELS; i++)
    {
        pixels.setPixelColor(i, Wheel(colour));
        pixels.show();
    }
    ++colour;
    if (colour > 255)
        colour = 0;
    delay(delayval);
}
