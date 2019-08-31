// Copyright 2019 Catalin G. Manciu

#include "game.h"
#include "defs.h"
#include "res/env_sprite.h"
#include "res/car_sprite.h"
#include "res/hud_sprite.h"
#include "res/menu_sprite.h"
#include "res/font.h"
#include "res/fontmap.h"
#include "res/sprites.h"
#include "res/stringmgr.h"
#include "game/states/splash.h"
#include "game/states/mainmenu.h"
#include "game/states/ingame.h"
#include "game/states/aftergamemenu.h"

void RRRR::initialize(uint8_t fps) {
    tinyScreen.start();
    arcadeInit();

    setFramerate(fps);
    saveLoad();

    buttonsState = 0;
    oldButtonsState = 0;
    frameCounter = 0;

    renderer.initialize(reinterpret_cast<uint16_t*>(tinyScreen.getSurface()),
                        Defs::ScreenW, Defs::ScreenH);
    renderer.setClip(0, 0, Defs::ScreenW, Defs::ScreenH);

    GetSprite(Defs::SpriteEnv)->create(ENV_SPRITE_DATA);
    GetSprite(Defs::SpriteCar)->create(CAR_SPRITE_DATA);
    GetSprite(Defs::SpriteHud)->create(HUD_SPRITE_DATA);
    GetSprite(Defs::SpriteMenu)->create(MENU_SPRITE_DATA);
    GetFont(Defs::FontMain)->create(FONT_DATA, mapping, nb_map_elems,
                                    Defs::MainFontSpaceW, Defs::MainFontHeight,
                                    default_frame, map_start_char);
    setState(Splash);
}

void RRRR::update() {
    if (!nextFrame()) {
        return;
    }

    if (pendingState != nullptr) {
        currentState = pendingState;
        currentState->initialize();
        pendingState = nullptr;
    }

    oldButtonsState = buttonsState;
    buttonsState = (getButtonsState() | (getJoystickState() << 2));

    tinyScreen.beginFrame();
    currentState->update();
    currentState->render(&renderer);
#if SHOW_FPS
    static int32_t lastFrameTime = millis();
    static int16_t minFPS = 60;
    int32_t currentFrameTime = millis();
    int32_t frameDuration = currentFrameTime - lastFrameTime;
    char* buff = getStringBuffer();
    buff[3] = 0;
    if (frameDuration > 0) {
        int32_t fps = 1000 / frameDuration;
        if (fps < minFPS) {
            minFPS = fps;
        }
        Utils::fastGetDigits(uint16_t(fps), buff + 2, 3);
        GetFont(Defs::FontMain)->drawString(&renderer, buff,
                                            Defs::ScreenW - 1, 1,
                                            ANCHOR_RIGHT | ANCHOR_TOP);
        Utils::fastGetDigits(minFPS, buff + 2, 3);
        GetFont(Defs::FontMain)->drawString(&renderer, buff,
                                            Defs::ScreenW - 1, 8,
                                            ANCHOR_RIGHT | ANCHOR_TOP);
    }
    lastFrameTime = currentFrameTime;
#endif
    tinyScreen.endFrame();

    ++frameCounter;
}

bool RRRR::nextFrame() {
  static int32_t lastFrameTime = 0;
  int32_t currentTime = millis();
  if (currentTime - lastFrameTime < waitTimeMillis) {
    return false;
  }
  lastFrameTime = currentTime;
  return true;
}

int32_t RRRR::bestTimes[4] = {
    60000,
    70000,
      -20,
      -10
};

int32_t RRRR::getTimeRecord(uint8_t gameMode, uint8_t gearMode) {
    return bestTimes[(gameMode << 1) + gearMode];
}

void RRRR::updateTimeRecord(uint8_t gameMode,
                            uint8_t gearMode,
                            int32_t newValue) {
    bestTimes[(gameMode << 1) + gearMode] = newValue;
    saveSave();
}

void RRRR::saveSave() {
}

void RRRR::saveLoad() {
}

void RRRR::setState(GameState newState) {
    static BaseGameState invalid;
    static StateSplash splash;
    static StateMainMenu mainmenu;
    static StateIngame ingame;
    static StateAfterGameMenu aftergamemenu;
    static BaseGameState* states[] = {
        &invalid,
        &splash,
        &mainmenu,
        &ingame,
        &aftergamemenu
    };
    if (currentState != states[newState]) {
        pendingState = states[newState];
    }
}

static RRRR instance;

RRRR& RRRR::getInstance() {
    return instance;
}
