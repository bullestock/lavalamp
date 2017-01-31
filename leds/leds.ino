#include <Adafruit_NeoPixel.h>
#include <SoftSerial.h>

#define RGB_PIN     2
#define NUMPIXELS   3

// Flash LED for debugging
#define FLASH_LED      1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, RGB_PIN, NEO_RGB + NEO_KHZ800);

const int TX_PIN = 3;
const int RX_PIN = 4;

SoftSerial mySerial(RX_PIN, TX_PIN);

int delayval = 20;

void setup()
{
    pinMode(RX_PIN, INPUT);
    pinMode(TX_PIN, OUTPUT);
    mySerial.begin(9600);
    pixels.begin();
    pinMode(1, OUTPUT); // LED
}

int led_state = 0;
int r = 128;
int g = 128;
int b = 128;
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
    for (int i = 0; i < NUMPIXELS; i++)
    {
        pixels.setPixelColor(i, r, g, b);
        pixels.show();
    }
    delay(delayval);
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
                mySerial.print("Colour ");
                mySerial.print(r);
                mySerial.print("/");
                mySerial.print(g);
                mySerial.print("/");
                mySerial.println(b);
            }
            else if (buf[0] == 'H')
            {
                const int heater_on = buf[1] - '0';
                mySerial.print("Heater ");
                mySerial.println(heater_on ? "on" : "off");
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
