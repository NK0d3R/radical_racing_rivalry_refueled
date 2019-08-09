// Copyright 2018 Catalin G. Manciu

#ifndef DEFS_H_
#define DEFS_H_

#include "engine/primitives.h"
#include "engine/fixedpoint.h"

#define USE_RENDERER_LINE_BUFFER    (0)
#if USE_RENDERER_LINE_BUFFER
    #define BUFFER_CAPACITY         (128)
#endif

class SpriteRenderer;

typedef FPValue<int32_t, int64_t, 8> FP32;
typedef VectorT<FP32> Vector;
typedef LineT<FP32> Line;

uint32_t getFrameCounter();

enum AppState {
    Invalid,
    Splash,
    MainMenu,
    Ingame,
    AfterGameMenu
};

void setAppState(AppState newState);

int32_t getTimeRecord(uint8_t gameMode, uint8_t gearMode);
void updateTimeRecord(uint8_t gameMode, uint8_t gearMode, int32_t newValue);
void saveSave();
void saveLoad();

struct Defs {
    static constexpr uint8_t ScreenW = 128;
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
        AnimMenuMain
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

    static void fastGetDigits(uint16_t value, char* dest, uint16_t nbDigits);
    static void formatTime(int32_t time, char* dest, bool addSign = false);
    static void formatDistance(int16_t distance, char* dest);
    static void drawBlinkingText(SpriteRenderer* renderer, uint8_t stringID,
                                 uint8_t x, uint8_t y);
};

#endif  // DEFS_H__
