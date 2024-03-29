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
    Logo,
    Splash,
    MainMenu,
    Ingame,
    AfterGameMenu,
    SaveError
};

class R3R {
 private:
    SpriteRenderer      renderer;
    TinyScreenCompact   tinyScreen;
    Menu                menu;
    Level               level;
    BaseGameState*      currentState;
    BaseGameState*      pendingState;
    uint8_t             waitTimeMillis;
    int32_t             frameTime;
    bool                nextFrame();
    static int32_t bestTimes[4];
    static constexpr uint32_t saveVersion = 1;
    static constexpr uint32_t signature = (static_cast<uint32_t>('N') << 24) |
                                          (static_cast<uint32_t>('K') << 16) |
                                          (static_cast<uint32_t>('D') << 8)  |
                                          saveVersion;
#if _WIN64
    static constexpr const char* saveFileName = "profile.bin";
#else
    static constexpr const char* saveFileName = "R3R/profile.bin";
#endif
    struct Profile {
        uint32_t signature;
        int32_t bestTimes[4];
        uint32_t nbDuelWins[2];
        uint8_t lastGameConfig;
    };
    bool profileNeedsSave;
    bool savingEnabled;
    Profile p;

 public:
    uint8_t             buttonsState;
    uint8_t             oldButtonsState;
    uint32_t            frameCounter;

    R3R();

    Menu& getMenu()     { return menu; }
    Level& getLevel()   { return level; }

    void initialize(uint8_t fps);
    void update();
    uint32_t getFrameCounter() { return frameCounter; }
    void setFramerate(uint8_t framerate) { waitTimeMillis = 1000 / framerate; }
    void saveLoad();
    void saveSave();
    int32_t getTimeRecord(uint8_t gameMode, uint8_t gearMode);
    void updateTimeRecord(uint8_t gameMode, uint8_t gearMode,
                          int32_t newValue);
    void increaseDuelWins(uint8_t gearMode);
    void saveGameConfig(uint8_t gameMode,
                        uint8_t gearMode,
                        uint8_t chassis);
    uint8_t getSavedGameMode() { return ((p.lastGameConfig >> 4) & 0x1); }
    uint8_t getSavedGearMode() { return ((p.lastGameConfig >> 3) & 0x1); }
    uint8_t getSavedChassis() { return (p.lastGameConfig & 0x7); }
    uint32_t getNbDuelWins(uint8_t gearMode) { return p.nbDuelWins[gearMode]; }
    void setState(GameState newState);
    static R3R& getInstance();
};

#endif  // GAME_H_
