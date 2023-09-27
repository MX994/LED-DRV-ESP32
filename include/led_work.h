#ifndef LED_WORK_H
#define LED_WORK_H

#include <Arduino.h>

namespace LED_DRV {
    typedef enum {
        OFF,
        STATIC,
        PATTERN
    } ILSMode;

    typedef struct {
        uint32_t Color;
        uint8_t Brightness;
        ILSMode Mode;
        bool ColorUpdate;
        bool BrightnessUpdate;
        bool ModeUpdate;
    } LEDWork;
};  // namespace LED_DRV

#endif