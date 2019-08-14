// Copyright 2018 Catalin G. Manciu

#ifndef DEFS_H_
#define DEFS_H_

#include "engine/primitives.h"
#include "engine/fixedpoint.h"
#include "res/strings.h"

#define LINE_BUFFER_CAPACITY    (64)
#define SHOW_FPS                (1)
#define ENABLE_BLUR             (1)

class SpriteRenderer;

typedef FPValue<int32_t, int64_t, 8> FP32;
typedef VectorT<FP32> Vector;
typedef LineT<FP32> Line;

struct Defs {
    static constexpr uint8_t ScreenW = 96;
    static constexpr uint8_t ScreenH = 64;

    // Sprites
    enum : uint8_t {
        SpriteEnv = 0,
        SpriteCar,
        SpriteMenu,
        SpritesNb
    };

    // Font
    enum : uint8_t {
        FontMain = 0,
        FontsNb
    };

    // SpriteEnv anims
    enum : uint8_t {
        AnimBackgrounds = 0,
        AnimChopper  // Currently removed from sprite
    };

    // AnimBackgrounds frames
    enum : uint8_t {
        BackgroundSun = 0,
        BackgroundLayer1,
        BackgroundLayer2,
        BackgroundLayer3
    };

    // SpriteCar anims
    enum : uint8_t {
        AnimCarBody = 0,
        AnimCarWheels,
        AnimCarReflection,
        AnimCarExplosion,
        AnimCarRPMHud,
        AnimCarSpeedFont,
        AnimCarGearsAuto,
        AnimCarGearsManual,
        AnimCarCountdown,
        AnimCarFlag
    };

    // AnimCarRPMHud frames
    enum : uint8_t {
        HUDFrameRPM = 0,
        HUDFrameRPMBar,
        HUDFrameKPH,
        HUDFrameWarning,
        HUDFrameWarningOver
    };

    // AnimCarGearsManual frames
    enum : uint8_t {
        HUDGearsManualUp = 6,
        HUDGearsManualDown,
        HUDGearsManualBg
    };

    // AnimCarGearsAuto frames
    enum : uint8_t {
        HUDGearsAutoBg = 0,
        HUDGearsAutoStick
    };

    // SpriteMenu anims
    enum : uint8_t {
        AnimMenuElements = 0,
        AnimMenuMain,
        AnimLogo
    };

    // AnimMenuElements frames
    enum : uint8_t {
        MenuItemBg = 0,
        MenuArrowR = 2,
        MenuArrowL,
        MenuSplash
    };

    // Menu actions
    enum : uint8_t {
        MenuActionStart = 1,
        MenuActionRestart,
        MenuActionBackToMM,
    };

    static constexpr int8_t BgChopperMarginOffset = 15;
    static constexpr int16_t BgChopperMinWaitTime = 3000;
    static constexpr int16_t BgChopperMaxWaitTime = 5000;
    static constexpr int16_t BgChopperDecisionTime = 1500;

    static constexpr uint8_t RPMBarLength = 32;
    static constexpr uint8_t CarSpeedFontW = 5;
    static constexpr uint8_t WarningSignH = 9;

    static constexpr uint8_t MainFontHeight = 7;
    static constexpr uint8_t MainFontSpaceW = 4;

    static constexpr uint8_t LevelActionAreaTop = 45;
    static constexpr uint8_t LevelActionAreaBottom = 63;

    static constexpr uint8_t MaxGear = 5;

    static constexpr uint8_t EndFlagW = 96;
    static constexpr uint8_t EndFlagH = 16;

    static constexpr uint8_t ResultTextY = 18;

    static constexpr uint8_t MaxOverheat = 36;
    static constexpr uint16_t OverheatRPM = 7400;
    static constexpr uint16_t OverheatDiv = 100;

    static const FP32 FPHalfScrW;
    static const FP32 MinRPM;
    static const FP32 MaxRPM;
    static const FP32 RaceLength;
    static const FP32 MaxCarSpeed;
    static const FP32 IdleRPMModif;
};

struct Utils {
    template<typename T>
    static T sgn(T x) { return x < 0 ? -1 : 1; }

    template<typename T>
    static T sgnz(T x) { return x == 0 ? 0 : Utils::sgn(x); }

    template<typename T>
    static T pixelsToMeters(T x) { return (x / 10); }

    template<typename T>
    static T metersToPixels(T x) { return (x * 10); }

    template<typename T>
    static T mpsToKph(T x) { return (x * 36) / 10; }

    template<typename T>
    static T upperClamp(T x, T limit) { return x > limit ? limit : x; }

    template<typename T>
    static void swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }

    // Need to switch endian-ness
    // BBBBB GGG GGG RRRRR -> (lower)GGG RRRRR BBBBB (upper)GGG
    static constexpr uint16_t make16BitColor(uint8_t r, uint8_t g, uint8_t b) {
        return make16BitColorLow((r >> 3), (g >> 2), (b >> 3));
    }

    static constexpr uint16_t make16BitColorLow(uint8_t r,
                                                uint8_t g,
                                                uint8_t b) {
    #define R(r)    (r << 8)
    #define G(g)    (((g & 0x7) << 13) | (g >> 3))
    #define B(b)    (b << 3)
        return (R(r) | G(g) | B(b));
    #undef R
    #undef G
    #undef B
    }

    static constexpr uint8_t getRLow(uint16_t value) {
        return ((value >> 8) & 0x1f);
    }

    static constexpr uint8_t getGLow(uint16_t value) {
        return ((value >> 13) | (value & 0x7) << 3);
    }

    static constexpr uint8_t getBLow(uint16_t value) {
        return ((value >> 3) & 0x1f);
    }

    static void fastGetDigits(uint16_t value, char* dest, uint16_t nbDigits);
    static void formatTime(int32_t time, char* dest, bool addSign = false);
    static void formatDistance(int16_t distance, char* dest);
    static void drawBlinkingText(SpriteRenderer* renderer, Strings stringID,
                                 uint8_t x, uint8_t y);
    static uint32_t random32() {
        return ((static_cast<uint32_t>(random(65535)) << 16) |
                 static_cast<uint32_t>(random(65535)));
    }
};

#endif  // DEFS_H__
