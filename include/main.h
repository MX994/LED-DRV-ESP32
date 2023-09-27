#ifndef MAIN_H
#define MAIN_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#include <WiFi.h>

#include "anim_seq.h"
#include "led_work.h"

namespace LED_DRV {
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
    #define VOLTAGE_MAX 5
    #define CURRENT_MAX_MILLIAMPERES 2000

    // Change these as necessary.
    // Note that we only support 2.4Ghz.
    const char *SSID = "CAN-LS";
    const char *PASS = "John0343";

    AnimSeqHndl *g_AnimSeqHndl;
    AsyncWebServer Server(80);
    CRGB LEDs[NUM_LEDS];

    LEDWork *g_Work;

    void InitializeWork();
    void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue);
    void SetModeStatic(AsyncWebServerRequest *request);
    void SetModePattern(AsyncWebServerRequest *request);
    void FetchAndReturnPage(AsyncWebServerRequest *request, const char *);
}  // namespace LED_DRV

#endif