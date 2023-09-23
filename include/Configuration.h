#ifndef ILS_CONFIG
#define ILS_CONFIG

#include <FastLED.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

// This enables serial output.
#define DEBUG

// Listen to me very carefully.
// Do not change the limits down here unless you know what you are doing.
// This can have serious implications for the wattage, or stability of the circuit.
// Again, do not change these unless you know what you are doing.
// You have been warned.
#define NUM_LEDS 861
#define DATA_PIN 25
#define BRIGHTNESS_MAX 64

// Change these as necessary.
// Note that we only support 2.4Ghz.
const char *SSID = "CAN-LS";
const char *PASS = "John0343";

AsyncWebServer Server(80);
CRGB LEDs[NUM_LEDS];

// Mode Enum
typedef enum {
    OFF,
    STATIC,
    PATTERN
} ILSMode;

// Work structure.
typedef struct {
    uint32_t Color;
    uint8_t Brightness;
    ILSMode Mode;
    bool ColorUpdate;
    bool BrightnessUpdate;
    bool ModeUpdate;
} LEDWork;

LEDWork *Work;

void InitializeWork();
void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue);
void SetModeStatic(AsyncWebServerRequest* request);
void SetModePattern(AsyncWebServerRequest* request);
void FetchAndReturnPage(AsyncWebServerRequest* request, const char *);

#endif 