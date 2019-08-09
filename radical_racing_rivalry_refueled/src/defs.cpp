// Copyright 2018 Catalin G. Manciu

#include "defs.h"
#include "engine/renderer.h"
#include "res/stringmgr.h"
#include "res/sprites.h"

const FP32 Defs::FPHalfScrW(Defs::ScreenW / 2);
const FP32 Defs::MinRPM(1000);
const FP32 Defs::MaxRPM(8000);
const FP32 Defs::RaceLength(2000);
const FP32 Defs::MaxCarSpeed((293.0f * 1000.0f) / 3600.0f);  // 293 km/h in m/s
const FP32 Defs::IdleRPMModif(16.0f);     // 16000 rot /s = 16 rot / msec

void Utils::fastGetDigits(uint16_t value, char* dest, uint16_t nbDigits) {
    int16_t maxVal = 1;
    for (uint16_t p = 0; p < nbDigits; p ++) {
        maxVal *= 10;
    }
    maxVal--;
    if (value > maxVal) {
        value = maxVal;
    }
    while (nbDigits--) {
        *dest-- = '0' + (value % 10);
        value /= 10;
    }
}

void Utils::formatTime(int32_t time, char* dest, bool addSign) {
    bool negative = (time < 0);
    if (negative) {
        time *= -1;
    }
    uint16_t sec = time / 1000;
    uint16_t msec = time - (sec * 1000);
    uint16_t min = sec / 60;
    sec -= min * 60;
    if (addSign) {
        if (negative) {
            dest[0] = '-';
        } else {
            dest[0] = '+';
        }
        dest++;
    }
    Utils::fastGetDigits(min, &dest[1], 2);
    dest[2] = ':';
    Utils::fastGetDigits(sec, &dest[4], 2);
    dest[5] = '.';
    Utils::fastGetDigits(msec, &dest[8], 3);
    dest[9] = 0;
}

void Utils::formatDistance(int16_t distance, char* dest) {
    dest[0] = '+';
    fastGetDigits(distance, &dest[3], 3);
    dest[4] = 'm';
    dest[5] = '>';
    dest[6] = 0;
}

static void Utils::drawBlinkingText(SpriteRenderer* renderer, uint8_t stringID,
                                    uint8_t x, uint8_t y) {
    if ((getFrameCounter() & 0xF) < 7) {
        GetFont(Defs::FontMain)->drawString(renderer,
                                            getString(stringID),
                                            x, y, ANCHOR_TOP | ANCHOR_HCENTER);
    }
}
