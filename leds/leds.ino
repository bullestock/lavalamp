#include <DigiUSB.h>
#include <WS2811.h>

#define LED_PIN     1
#define RGB_PIN     2
#define NUMPIXELS   3

const int HEATER_PIN = LED_PIN;

DEFINE_WS2811_FN(updatePixels, PORTB, RGB_PIN)
RGB_t rgb[NUMPIXELS];

int delayval = 20;

void setup()
{
#if 1
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(RGB_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
#endif
    DigiUSB.begin();
    DigiUSB.println("LavaLamp 0.1");
}

int r = 128;
int g = 128;
int b = 128;
int r2, g2, b2;
bool first_colour = true;
int iterations = 0;
int blink_speed = 100;
enum {
    STATE_STEADY,
    STATE_BLINK
} state = STATE_STEADY;

const int MAX_CMD_LEN = 20;
char buf[MAX_CMD_LEN+1];
int buf_index = 0;

int get_color(const char* buf, int offset)
{
    return (buf[offset] - '0')*100 + (buf[offset+1] - '0')*10 + (buf[offset+2] - '0');
}

void setPixel(int i, int r, int g, int b)
{
	rgb[i].r = r;
    rgb[i].g = g;
    rgb[i].b = b;
}

void loop()
{
    switch (state)
    {
    case STATE_STEADY:
        for (int i = 0; i < NUMPIXELS; i++)
            setPixel(i, r, g, b);

        break;

    case STATE_BLINK:
        for (int i = 0; i < NUMPIXELS; i++)
            if (first_colour)
                setPixel(i, r, g, b);
            else
                setPixel(i, r2, g2, b2);
        if (++iterations > blink_speed)
        {
            iterations = 0;
            first_colour = !first_colour;
        }
        break;
    }
    updatePixels(rgb, NUMPIXELS);
    DigiUSB.delay(10);
    if (DigiUSB.available() > 0)
    {
        buf[buf_index] = DigiUSB.read();
        if (int(buf[buf_index]) == 13 || int(buf[buf_index]) == 10)
        {
            buf[buf_index+1] = 0;
            if (buf[0] == 'C')
            {
                r = get_color(buf, 1);
                g = get_color(buf, 4);
                b = get_color(buf, 7);
                state = STATE_STEADY;
            }
            else if (buf[0] == 'B')
            {
                r = get_color(buf, 1);
                g = get_color(buf, 4);
                b = get_color(buf, 7);
                r2 = get_color(buf, 10);
                g2 = get_color(buf, 13);
                b2 = get_color(buf, 16);
                state = STATE_BLINK;
            }
            else if (buf[0] == 'H')
            {
                const int heater_on = buf[1] - '0';
                DigiUSB.print("Heater ");
                DigiUSB.println(heater_on ? "on" : "off");
                digitalWrite(HEATER_PIN, heater_on);
            }
            else if (buf[0] == 'D')
            {
                blink_speed = get_color(buf, 1);
                DigiUSB.print("Blink delay ");
                DigiUSB.println(blink_speed);
            }
            else
            {
                DigiUSB.println("ERR");
            }
            buf_index = 0;
        }
        else
        {
            ++buf_index;
            if (buf_index >= MAX_CMD_LEN)
                buf_index = 0;
        }
    }
}
