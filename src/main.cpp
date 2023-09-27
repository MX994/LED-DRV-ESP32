#include "main.h"

#include <Arduino.h>
#include <LittleFS.h>

namespace LED_DRV {
    void setup() {
      #ifdef DEBUG
      Serial.begin(9600);
      while (!Serial);
      #endif

      if (!LittleFS.begin(true)) {
          Serial.println("LittleFS failed to initialize.");
          return;
      }

      // Setup AP.
      WiFi.mode(WIFI_STA);
      WiFi.begin(SSID, PASS);

      while (WiFi.status() != WL_CONNECTED) {
          Serial.println("Connecting to WiFi...");
          delay(500);
      }

      Serial.print("Connect to AP, then go to this IP: ");
      Serial.println(WiFi.localIP());

      FastLED.addLeds<WS2812B, DATA_PIN, GRB>(LEDs, NUM_LEDS);
      FastLED.setBrightness(BRIGHTNESS_MAX);
      FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);

      InitializeWork();
      g_AnimSeqHndl = new AnimSeqHndl(LEDs, NUM_LEDS);
    }

    void loop() {
        bool Updated = g_Work->BrightnessUpdate || g_Work->ColorUpdate || g_Work->ModeUpdate;
        
        // Change brightness if requested.
        if (Updated) {
          if (g_Work->BrightnessUpdate) {
            FastLED.setBrightness(g_Work->Brightness);
            g_Work->BrightnessUpdate = false;
          } 

          // Update the mode or color if requested.
          if (g_Work->ModeUpdate || g_Work->ColorUpdate) {
              switch (g_Work->Mode) {
                  case OFF:
                      FastLED.clear();
                      g_Work->ModeUpdate = false;
                      break;
                  case STATIC:
                      fill_solid(LEDs, NUM_LEDS, g_Work->Color);
                      g_Work->ModeUpdate = false;
                      break;
                  case PATTERN:
                      break;
              }
          }
        }

        // If mode is pattern, let the sequence handler control show.
        // Otherwise, do it ourselves.
        if (g_Work->Mode == PATTERN) {
            g_AnimSeqHndl->Step();
        } else {
            FastLED.show();
        }
    }

    char *ReadFile(fs::FS &fs, const char *path) {
        Serial.printf("Reading file: %s\r\n", path);

        File file = fs.open(path);
        if (!file || file.isDirectory()) {
            Serial.println("- failed to open file for reading");
            return NULL;
        }

        char *file_buffer = (char *)malloc(sizeof(char) * file.size());
        file.readBytes(file_buffer, file.size());
        file.close();
        return file_buffer;
    }

    void GetMainPage(AsyncWebServerRequest *request) {
        FetchAndReturnPage(request, "/index.html");
    }

    void SetModeStatic(AsyncWebServerRequest *request) {
        g_Work->Mode = STATIC;
        g_Work->ModeUpdate = true;
        FetchAndReturnPage(request, "/static.html");
    }

    void SetModePattern(AsyncWebServerRequest *request) {
        g_Work->Mode = PATTERN;
        g_Work->ModeUpdate = true;
        FetchAndReturnPage(request, "/pattern.html");
    }

    void FetchAndReturnPage(AsyncWebServerRequest *request, const char *path) {
        char *data = ReadFile(LittleFS, path);
        if (data) {
            request->send(200, "text/html", data);
        } else {
            request->send(400, "text/html", "Error!");
        }
        free(data);
    }

    void HandleCommitRequest(AsyncWebServerRequest *request) {
        if (g_Work->Mode == STATIC) {
            if (request->hasParam("color")) {
                uint32_t RequestedColor = strtoul(request->getParam("color")->value().substring(1).c_str(), NULL, 16);
                if (RequestedColor >= 0x000000 && RequestedColor <= 0xFFFFFF) {
                    g_Work->Color = RequestedColor;
                    g_Work->ColorUpdate = true;
                }
            }
            if (request->hasParam("brightness")) {
                uint8_t RequestedBrightness = strtoul(request->getParam("brightness")->value().c_str(), NULL, 10);
                if (RequestedBrightness >= 0x0 && RequestedBrightness <= BRIGHTNESS_MAX) {
                    g_Work->Brightness = RequestedBrightness;
                    g_Work->BrightnessUpdate = true;
                }
            }
        } else if (g_Work->Mode == PATTERN) {
            if (request->hasParam("pattern")) {
                // TODO: Load pattern from file.
                ANIM_SEQ *Sequence = (ANIM_SEQ *)ReadFile(LittleFS, "test.anim");
                g_AnimSeqHndl->Load(...);
            }
        }

        request->send(200);
    }

    void InitializeWork() {
        g_Work = (LEDWork *)malloc(sizeof(LEDWork));
        Server.on("/", HTTP_GET, GetMainPage);
        Server.on("/static", HTTP_GET, SetModeStatic);
        Server.on("/pattern", HTTP_GET, SetModePattern);
        Server.on("/commit", HTTP_GET, HandleCommitRequest);
        Server.begin();
    }

    void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue) {
        uint8_t thisHue = beatsin8(thisSpeed, 0, 0xFF);
        fill_rainbow(LEDs, NUM_LEDS, thisHue, 0);
    }
}  // namespace LED_DRV