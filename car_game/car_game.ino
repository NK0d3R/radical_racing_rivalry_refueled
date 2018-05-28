  // Copyright 2018 Catalin G. Manciu

#include "src/stdinc.h"
#include "src/defs.h"
#include "src/engine/ardusprite.h"
#include "src/engine/renderer.h"
#include "src/game/level.h"
#include "src/game/menu.h"
#include "src/res/stringmgr.h"
#include "src/res/sprites.h"
#include "src/res/env_sprite.h"
#include "src/res/car_sprite.h"
#include "src/res/menu_sprite.h"
#include "src/res/font.h"
#include "src/res/fontmap.h"

Arduboy app;
uint8_t         buttonsState;
uint8_t         oldButtonsState;
SpriteRenderer  renderer;
Menu            menu;
Level           level;
uint32_t        frameCounter;

AppState appState = Invalid;

int32_t bestTimes[4] = {
    180000,
    220000,
     -1000,
      -800
};


int32_t getTimeRecord(uint8_t gameMode, uint8_t gearMode) {
    return bestTimes[(gameMode << 1) + gearMode];
}

void updateTimeRecord(uint8_t gameMode, uint8_t gearMode, int32_t newValue) {
    bestTimes[(gameMode << 1) + gearMode] = newValue;
    saveSave();
}

static constexpr uint16_t signature = (static_cast<uint16_t>('N') << 8) |
                                      (static_cast<uint16_t>('R'));

void saveSave() {
    EEPROM.put(0, signature);
    EEPROM.put(sizeof(signature), bestTimes);
}

void saveLoad() {
    uint16_t sign;
    EEPROM.get(0, sign);
    if(sign != signature) {
        saveSave();
    } else {
        EEPROM.get(sizeof(signature), bestTimes);
    }
}

void setAppState(AppState newState) {
    if (appState != newState) {
        switch (newState) {
            case MainMenu:
                menu.set(getMenuData(0), 3, 100, Defs::AnimMenuMain);
                menu.setItemOption(0, level.getGameMode());
                menu.setItemOption(1, level.getGearMode());
                menu.restart();
            break;
            case AfterGameMenu:
                menu.set(getMenuData(1), 2, 80, 0);
                menu.restart();
            break;
        }
        appState = newState;
    }
}

uint32_t getFrameCounter() {
    return frameCounter;
}

void setup() {
    saveLoad();
    app.beginNoLogo();
    app.setFrameRate(30);
    app.clear();

    buttonsState = 0;
    oldButtonsState = 0;
    frameCounter = 0;
    GetSprite(Defs::SpriteEnv)->create(ENV_SPRITE_DATA);
    GetSprite(Defs::SpriteCar)->create(CAR_SPRITE_DATA);
    GetSprite(Defs::SpriteMenu)->create(MENU_SPRITE_DATA);
    GetFont(Defs::FontMain)->create(FONT_DATA, mapping, nb_map_elems,
                                    Defs::MainFontSpaceW, Defs::MainFontHeight,
                                    default_frame);
    renderer.initialize(app.getBuffer(), 128);
    renderer.setClip(0, 0, 128, 64);
    setAppState(MainMenu);
    //Serial.begin(9600);
}

void loop() {
    if (!app.nextFrame()) {
        return;
    }

    app.clear();

    oldButtonsState = buttonsState;
    buttonsState = app.buttonsState();

    switch(appState) {
        case MainMenu: {
            menu.updateControls(buttonsState, oldButtonsState);
            menu.draw(&renderer, (Defs::ScreenW >> 1), 0);
            if (menu.getAction() == Defs::MenuActionStart) {
                level.setGameMode(menu.getItemOption(0));
                level.setGearMode(menu.getItemOption(1));
                level.restart();
                setAppState(Ingame);
            }
            uint8_t selectedGameMode = menu.getItemOption(0);
            getString(Strings::BestTime);
            Utils::formatTime(getTimeRecord(selectedGameMode,
                                            menu.getItemOption(1)),
                              getStringBuffer() +
                              getStringLen(Strings::BestTime),
                              selectedGameMode == 1);
            GetFont(Defs::FontMain)->drawString(
                    &renderer, getStringBuffer(),
                    Defs::ScreenW / 2, 64, ANCHOR_BOTTOM | ANCHOR_HCENTER, -1);
        } break;
        case Ingame: {
            level.updateControls(buttonsState, oldButtonsState);
            level.update(33);
            level.draw(&renderer);
        } break;
        case AfterGameMenu: {
            menu.updateControls(buttonsState, oldButtonsState);
            switch (menu.getAction()) {
                case Defs::MenuActionRestart:
                    level.restart();
                    setAppState(Ingame);
                break;
                case Defs::MenuActionBackToMM:
                    setAppState(MainMenu);
                break;
            }
            menu.draw(&renderer, (Defs::ScreenW >> 1), 16);
        } break;
    }
    app.display();
    ++frameCounter;
}

