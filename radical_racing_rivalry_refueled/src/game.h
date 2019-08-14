// Copyright 2019 Catalin G. Manciu

#ifndef GAME_H_
#define GAME_H_

#include "stdinc.h"
#include "controls.h"
#include "engine/ardusprite.h"
#include "engine/renderer.h"
#include "engine/fireeffect.h"
#include "game/level.h"
#include "game/menu.h"
#include "game/states/basegamestate.h"

enum GameState : uint8_t {
    Invalid,
    Splash,
    MainMenu,
    Ingame,
    AfterGameMenu
};

class RRRR {
 private:
    SpriteRenderer      renderer;
    TinyScreenCompact   tinyScreen;
    Menu                menu;
    Level               level;
    BaseGameState*      currentState;
    BaseGameState*      pendingState;
    uint8_t             waitTimeMillis;
    bool                nextFrame();
    static int32_t bestTimes[4];
    static constexpr uint32_t saveVersion = 1;
    static constexpr uint32_t signature = (static_cast<uint32_t>('N') << 24) |
                                          (static_cast<uint32_t>('K') << 16) |
                                          (static_cast<uint32_t>('D') << 8)  |
                                          saveVersion;

 public:
    uint8_t             buttonsState;
    uint8_t             oldButtonsState;
    uint32_t            frameCounter;

    RRRR() : currentState(nullptr), pendingState(nullptr) {}

    Menu& getMenu()     { return menu; }
    Level& getLevel()   { return level; }

    void initialize(uint8_t fps);
    void update();
    uint32_t getFrameCounter() { return frameCounter; }
    void setFramerate(uint8_t framerate) { waitTimeMillis = 1000 / framerate; }
    void saveLoad();
    void saveSave();
    int32_t getTimeRecord(uint8_t gameMode, uint8_t gearMode);
    void updateTimeRecord(uint8_t gameMode, uint8_t gearMode, int32_t newValue);
    void setState(GameState newState);
    static RRRR& getInstance();
};

#endif  // GAME_H_
