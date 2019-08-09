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
    60000,
    70000,
      -20,
      -10
};


int32_t getTimeRecord(uint8_t gameMode, uint8_t gearMode) {
    return bestTimes[(gameMode << 1) + gearMode];
}

void updateTimeRecord(uint8_t gameMode, uint8_t gearMode, int32_t newValue) {
    bestTimes[(gameMode << 1) + gearMode] = newValue;
    saveSave();
}

static constexpr uint32_t saveVersion = 1;
static constexpr uint32_t signature = (static_cast<uint32_t>('N') << 24) |
                                      (static_cast<uint32_t>('K') << 16) |
                                      (static_cast<uint32_t>('D') << 8)  |
                                       saveVersion;

void saveSave() {
    EEPROM.put(EEPROM_STORAGE_SPACE_START, signature);
    EEPROM.put(EEPROM_STORAGE_SPACE_START + sizeof(signature), bestTimes);
}

void saveLoad() {
    uint32_t sign;
    EEPROM.get(EEPROM_STORAGE_SPACE_START, sign);
    if(sign != signature) {
        saveSave();
    } else {
        EEPROM.get(EEPROM_STORAGE_SPACE_START + sizeof(signature), bestTimes);
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
                                    default_frame, map_start_char);
    renderer.initialize(app.getBuffer(), 128);
    renderer.setClip(0, 0, Defs::ScreenW, Defs::ScreenH);
    setAppState(Splash);
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
        case Splash: {
            GetSprite(Defs::SpriteMenu)->drawAnimationFrame(
                                            &renderer, Defs::AnimMenuElements,
                                            Defs::MenuSplash, 0, 0, 0);
            for (uint8_t idx = Strings::Title_1, x = 55, y = 10;
                 idx <= Strings::Title_3; ++idx, x += 10, y += 10) {
                    GetFont(Defs::FontMain)->drawString(
                            &renderer, getString(idx), x, y,
                            ANCHOR_LEFT | ANCHOR_TOP, 1);
            }
            GetFont(Defs::FontMain)->drawString(
                            &renderer, getString(Strings::Copyleft),
                            (Defs::ScreenW >> 1), Defs::ScreenH,
                            ANCHOR_HCENTER | ANCHOR_BOTTOM, -1);
            Utils::drawBlinkingText(&renderer, Strings::PressToCont,
                                    (Defs::ScreenW >> 1), 48);
            if ((oldButtonsState ^ buttonsState) & buttonsState) {
                setAppState(MainMenu);
            }
        } break;
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

