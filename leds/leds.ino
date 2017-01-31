#include <Adafruit_NeoPixel.h>
#include <SoftSerial.h>

#define RGB_PIN     2
#define NUMPIXELS   3

// Flash LED for debugging
#define FLASH_LED      1

const int TX_PIN = 3;
const int RX_PIN = 4;

const int HEATER_PIN = 5;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, RGB_PIN, NEO_RGB + NEO_KHZ800);

SoftSerial mySerial(RX_PIN, TX_PIN);

int delayval = 20;

void setup()
{
    pinMode(RX_PIN, INPUT);
    pinMode(TX_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);
    mySerial.begin(9600);
    pixels.begin();
    pinMode(1, OUTPUT); // LED
}

int led_state = 0;
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

void loop()
{
#if FLASH_LED
    digitalWrite(1, led_state);
    led_state = !led_state;
    //mySerial.println("HELLO");
#endif
    switch (state)
    {
    case STATE_STEADY:
        for (int i = 0; i < NUMPIXELS; i++)
            pixels.setPixelColor(i, r, g, b);

        break;

    case STATE_BLINK:
        for (int i = 0; i < NUMPIXELS; i++)
            if (first_colour)
                pixels.setPixelColor(i, r, g, b);
            else
                pixels.setPixelColor(i, r2, g2, b2);
        if (++iterations > blink_speed)
        {
            iterations = 0;
            first_colour = !first_colour;
        }
        break;
    }
    pixels.show();
    
    delay(10);
    if (mySerial.available() > 0)
    {
        buf[buf_index] = mySerial.read();
        if (int(buf[buf_index]) == 13 || int(buf[buf_index]) == 10)
        {
            buf[buf_index+1] = 0;
            if (buf[0] == 'C')
            {
                r = get_color(buf, 1);
                g = get_color(buf, 4);
                b = get_color(buf, 7);
                state = STATE_STEADY;
                mySerial.print("Colour ");
                mySerial.print(r);
                mySerial.print("/");
                mySerial.print(g);
                mySerial.print("/");
                mySerial.println(b);
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
                mySerial.print("Colours ");
                mySerial.print(r);
                mySerial.print("/");
                mySerial.print(g);
                mySerial.print("/");
                mySerial.print(b);
                mySerial.print(" ");
                mySerial.print(r2);
                mySerial.print("/");
                mySerial.print(g2);
                mySerial.print("/");
                mySerial.println(b2);
            }
            else if (buf[0] == 'H')
            {
                const int heater_on = buf[1] - '0';
                mySerial.print("Heater ");
                mySerial.println(heater_on ? "on" : "off");
                digitalWrite(HEATER_PIN, heater_on);
            }
            else if (buf[0] == 'D')
            {
                blink_speed = get_color(buf, 1);
                mySerial.print("Blink delay ");
                mySerial.println(blink_speed);
            }
            else
            {
                mySerial.print("Unknown command: ");
                mySerial.println(buf);
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
