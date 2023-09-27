#ifndef ANIM_SEQ_H
#define ANIM_SEQ_H

#include <Arduino.h>
#include <FastLED.h>

#include "led_work.h"

namespace LED_DRV {
    struct ANIM_SEQ_FRAME {
        ANIM_SEQ_CMD Command;
        uint16_t StartIdx;
        uint16_t FillCount;
        uint32_t Color;
        uint8_t Brightness;
        uint16_t Duration;
    };

    enum ANIM_SEQ_CMD {
        CMD_SHOW,
        CMD_FILL,
        CMD_CLEAR
    };

    struct ANIM_SEQ {
        uint16_t FrameCnt;
        ANIM_SEQ_FRAME **Frames;
    };

    class AnimSeqHndl {
       public:
        AnimSeqHndl(CRGB *LEDs, uint16_t Count);
        bool Load(ANIM_SEQ *Sequence);
        bool Step();
        bool Unload();

       private:
        void StepFrameCounter();
        ANIM_SEQ *CurrentSeq;
        uint16_t CurrentFrameIdx;
        uint16_t LED_Count;
        CRGB *LEDs;
    };
};  // namespace LED_DRV

#endif