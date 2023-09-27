#include "anim_seq.h"

namespace LED_DRV {
    AnimSeqHndl::AnimSeqHndl(CRGB *LEDs, uint16_t Count) {
        this->LED_Count = Count;
        this->LEDs = LEDs;
    }

    bool AnimSeqHndl::Load(ANIM_SEQ *Sequence) {
        // Load a sequence.
        this->CurrentSeq = Sequence;
    }

    void AnimSeqHndl::StepFrameCounter() {
        // Increment current frame index, and wrap (for looping).
        CurrentFrameIdx = (CurrentFrameIdx + 1) % this->CurrentSeq->FrameCnt;
    }

    bool AnimSeqHndl::Step() {
        // Handle the step for each frame iteration.
        if (!this->CurrentSeq || !this->LEDs) {
            return false;
        }
        ANIM_SEQ_FRAME *CurrentFrame = this->CurrentSeq->Frames[CurrentFrameIdx];
        switch (CurrentFrame->Command) {
            case CMD_SHOW:
                // Update the frame buffer.
                FastLED.show();
                StepFrameCounter();
                return true;
            case CMD_FILL:
                // Fill a select range of LEDs with a color.
                uint16_t LED_SLOT_MAX = CurrentFrame->StartIdx + CurrentFrame->FillCount;
                if (CurrentFrame->StartIdx >= this->CurrentSeq->FrameCnt || LED_SLOT_MAX >= this->CurrentSeq->FrameCnt) {
                    return false;
                }
                fill_solid(this->LEDs + CurrentFrame->StartIdx, CurrentFrame->FillCount, CurrentFrame->Color);
                StepFrameCounter();
                return true;
            case CMD_CLEAR:
                // Clear the LED buffer.
                FastLED.clear();
                StepFrameCounter();
                return true;
            default:
                // Drop the frame, and hang.
                return false;
        }
    }

    bool AnimSeqHndl::Unload() {
        // Remove dangling pointer.
        this->CurrentSeq = nullptr;
    }
};