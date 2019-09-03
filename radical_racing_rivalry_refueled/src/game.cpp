// Copyright 2019 Catalin G. Manciu

#include "game.h"
#include "defs.h"
#include "engine/filesystem.h"
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
#include "game/states/saveerror.h"

R3R::R3R(): currentState(nullptr), pendingState(nullptr) {
    p = { R3R::signature,
          60000,
          70000,
          -20,
          -10,
          0,
          0
        };
}

void R3R::initialize(uint8_t fps) {
    tinyScreen.start();
    arcadeInit();

    setFramerate(fps);

    profileNeedsSave = false;

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
    savingEnabled = initializeFileSystem();
    saveLoad();
    if (savingEnabled) {
        setState(Splash);
    } else {
        setState(SaveError);
    }
}

void R3R::update() {
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

bool R3R::nextFrame() {
    static int32_t lastFrameTime = 0;
    int32_t currentTime = millis();
    if (currentTime - lastFrameTime < waitTimeMillis) {
        return false;
    }
    lastFrameTime = currentTime;
    return true;
}

int32_t R3R::getTimeRecord(uint8_t gameMode, uint8_t gearMode) {
    return p.bestTimes[(gameMode << 1) + gearMode];
}

void R3R::updateTimeRecord(uint8_t gameMode, uint8_t gearMode,
                            int32_t newValue) {
    p.bestTimes[(gameMode << 1) + gearMode] = newValue;
    profileNeedsSave = true;
}

void R3R::increaseDuelWins() {
    if (p.nbDuelWins < 9999999) {
        p.nbDuelWins++;
        profileNeedsSave = true;
    }
}

void R3R::saveSave() {
    if (savingEnabled && profileNeedsSave) {
        save(saveFileName, &p, sizeof(p));
        profileNeedsSave = false;
    }
}

void R3R::saveLoad() {
    if (savingEnabled) {
        Profile temp;
        if (load(saveFileName, &temp, sizeof(temp)) == false ||
            temp.signature != signature) {
            profileNeedsSave = true;
            saveSave();
        } else {
            memcpy(&p, &temp, sizeof(temp));
        }
    }
}

void R3R::setState(GameState newState) {
    static BaseGameState invalid;
    static StateSplash splash;
    static StateMainMenu mainmenu;
    static StateIngame ingame;
    static StateAfterGameMenu aftergamemenu;
    static StateSaveError saveerror;
    static BaseGameState* states[] = {
        &invalid,
        &splash,
        &mainmenu,
        &ingame,
        &aftergamemenu,
        &saveerror
    };
    if (currentState != states[newState]) {
        pendingState = states[newState];
    }
}

static R3R instance;

R3R& R3R::getInstance() {
    return instance;
}
