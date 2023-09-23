#include <Arduino.h>
#include "Configuration.h"
#include <LittleFS.h>

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
    delay(1000);
  }

  Serial.print("Connect to AP, then go to this IP: ");
  Serial.println(WiFi.localIP());
  
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(LEDs, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS_MAX);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);

  InitializeWork();
}

void loop() {
  bool Updated = Work->BrightnessUpdate || Work->ColorUpdate || Work->ModeUpdate;
  
  if (Work->BrightnessUpdate) {
    FastLED.setBrightness(Work->Brightness);
    Work->BrightnessUpdate = false;
  }

  if (Work->ModeUpdate || Work->ColorUpdate) {
    switch (Work->Mode) {
      case OFF:
        fill_solid(LEDs, NUM_LEDS, 0x0);
        Work->ModeUpdate = false;
        break;
      case STATIC:
        fill_solid(LEDs, NUM_LEDS, Work->Color);
        Work->ModeUpdate = false;
        break;
      case PATTERN:
        rainbow_wave(10, 10);
        break;
    }
  }

  if (Updated) {
     FastLED.show();
  }
}

char *ReadFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return NULL;
    }

    char *file_buffer = (char *)malloc(sizeof(char) * file.size());
    file.readBytes(file_buffer, file.size());
    file.close();
    return file_buffer;
}

void GetMainPage(AsyncWebServerRequest* request) {
  FetchAndReturnPage(request, "/index.html");
}

void SetModeStatic(AsyncWebServerRequest* request) {
  Work->Mode = STATIC;
  Work->ModeUpdate = true;
  FetchAndReturnPage(request, "/static.html");
}

void SetModePattern(AsyncWebServerRequest* request) {
  Work->Mode = PATTERN;
  Work->ModeUpdate = true;
  FetchAndReturnPage(request, "/pattern.html");
}

void FetchAndReturnPage(AsyncWebServerRequest* request, const char *path) {
  char *data = ReadFile(LittleFS, path);
  if (data) {
    request->send(200, "text/html", data);
  } else {
    request->send(400, "text/html", "Error!");
  }
  free(data);
}

void HandleCommitRequest(AsyncWebServerRequest* request) {
  Serial.println("CAUGHT!");
  if (Work->Mode == STATIC) {
    if (request->hasParam("color")) {
      uint32_t RequestedColor = strtoul(request->getParam("color")->value().substring(1).c_str(), NULL, 16);
      if (RequestedColor >= 0x000000 && RequestedColor <= 0xFFFFFF) {
        Serial.println(RequestedColor);
        Work->Color = RequestedColor;
        Work->ColorUpdate = true;
      }
    }
  } else if (Work->Mode == PATTERN) {
    if (request->hasParam("pattern")) {
      //
    }
  }
  if (request->hasParam("brightness")) {
    uint8_t RequestedBrightness = strtoul(request->getParam("brightness")->value().c_str(), NULL, 10); 
    if (RequestedBrightness >= 0x0 && RequestedBrightness <= BRIGHTNESS_MAX) {
      Work->Brightness = RequestedBrightness;
      Work->BrightnessUpdate = true;
    }
  }
  request->send(200);
}

void InitializeWork() {
  Work = (LEDWork *)malloc(sizeof(LEDWork));
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