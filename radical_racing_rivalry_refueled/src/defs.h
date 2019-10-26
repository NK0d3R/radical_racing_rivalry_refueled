// Copyright 2018 Catalin G. Manciu

#ifndef DEFS_H_
#define DEFS_H_

#include "engine/fixedpoint.h"
#include "engine/primitives.h"
#include "res/strings.h"

#define LINE_BUFFER_CAPACITY    (64)
#define SHOW_FPS                (0)

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
        SpriteHud,
        SpriteMenu,
        SpriteLogo,
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
        BackgroundLayer3,
        BackgroundLayer4
    };

    // SpriteCar anims
    enum : uint8_t {
        AnimCarChassis = 0,
        AnimCarWheels,
        AnimCarNeon,
        AnimCarReflection,
        AnimCarLights,
        AnimCarExplosion,
        AnimCarExplosion_Sm,
        AnimCarMenu,
    };

    // AnimCarLights frames
    enum : uint8_t {
        LightFrameHeadlights,
        LightFrameAlert
    };

    // SpriteHud anims
    enum : uint8_t {
        AnimHUDRPM,
        AnimHUDSpeedFont,
        AnimHUDGearsAuto,
        AnimHUDGearsManual,
        AnimHUDCountdown,
    };

    // AnimCarRPMHud frames
    enum : uint8_t {
        HUDFrameRPM = 0,
        HUDFrameRPMBar,
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
        MenuSplash,
        ButtonHint
    };

    // AnimLogo frames
    enum : uint8_t {
        MenuLogoLastFrame = 8,
    };

    // Menu actions
    enum : uint8_t {
        MenuActionStart = 1,
        MenuActionRestart,
        MenuActionBackToMM,
    };

    static constexpr uint8_t CarNbChassis = 4;

    static constexpr int8_t BgChopperMarginOffset = 15;
    static constexpr int16_t BgChopperMinWaitTime = 3000;
    static constexpr int16_t BgChopperMaxWaitTime = 5000;
    static constexpr int16_t BgChopperDecisionTime = 1500;

    static constexpr uint8_t RPMBarLength = 30;
    static constexpr uint8_t CarSpeedFontW = 4;
    static constexpr uint8_t WarningSignH = 7;

    static constexpr uint8_t MainFontHeight = 7;
    static constexpr uint8_t MainFontSpaceW = 4;

    static constexpr uint8_t LevelActionAreaTop = 45;
    static constexpr uint8_t LevelActionAreaBottom = 63;

    static constexpr uint8_t MaxGear = 5;

    static constexpr uint8_t EndFlagW = 80;
    static constexpr uint8_t EndFlagH = 16;

    static constexpr uint8_t ResultTextY = 18;

    static constexpr uint8_t MaxOverheat = 36;
    static constexpr uint16_t OverheatRPM = 7400;
    static constexpr uint16_t OverheatDiv = 100;
    static constexpr uint16_t DemoLength = 18000;
    static constexpr uint16_t DemoStartTime = 330;
    static constexpr int16_t CameraSpeedMultiplier = 16;
    static const FP32 FPHalfScrW;
    static const FP32 MinRPM;
    static const FP32 MaxRPM;
    static const FP32 RaceLength;
    static const FP32 MaxCarSpeed;
    static const FP32 IdleRPMModif;
    static const FP32 CamMaxDistance;
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
    static void getDigits(uint16_t value, char* dest);
    static void formatTime(int32_t time, char* dest, bool addSign = false);
    static void formatDistance(int16_t distance, char* dest);
    static void drawBlinkingText(SpriteRenderer* renderer, Strings stringID,
                                 uint8_t x, uint8_t y);
    static uint32_t random32() {
        return ((static_cast<uint32_t>(rand() & 0xFFFF) << 16) |
                 static_cast<uint32_t>(rand() & 0xFFFF));
    }
    static uint8_t random8Except(uint8_t min, uint8_t max,
                                 uint8_t except = 0xFF) {
        uint8_t range = max - min;
        uint8_t value = ((rand() & 0xFF) % range);
        if (except != 0xFF && value == except) {
            value = (value + 1) % range;
        }
        return min + value;
    }

    static FP32 approachWithSpeed(const FP32& current, const FP32& target,
                                  const FP32& speed, int16_t dt) {
        FP32 diff = target - current;
        FP32 absdiff = diff.fpAbs();
        FP32 incr = (speed * dt) / 1000;
        FP32 result = current;
        if (incr >= absdiff) {
            result = target;
        } else {
            result += Utils::sgn(diff.getInt()) * incr;
        }
        return result;
    }
};

#endif  // DEFS_H__
