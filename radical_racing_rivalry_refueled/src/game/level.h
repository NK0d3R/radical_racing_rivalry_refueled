// Copyright 2018 Catalin G. Manciu

#ifndef LEVEL_H_
#define LEVEL_H_

#include "../stdinc.h"
#include "../defs.h"
#include "../controls.h"
#include "../engine/ardusprite.h"
#include "gearshift.h"
#include "gameobject.h"

class SpriteRenderer;
struct Sprite;
class Car;

class Level {
 public:
    Level();
    void restart();
    void draw(SpriteRenderer* renderer);
    void update(int16_t dt);
    int32_t worldToScreenX(const FP32& x, const FP32& y);
    uint8_t worldToScreenY(const FP32& x, const FP32& y);
    void updateControls(uint8_t buttonsState, uint8_t oldButtonsState);
    void raceStart();
    void raceEnd();
    void setGearMode(uint8_t mode) {
        currentGearShift = mode == 0 ?
            static_cast<GearShift*>(&autoGearShift) :
            static_cast<GearShift*>(&manualGearShift);
    }
    uint8_t getGearMode() {
        return currentGearShift == static_cast<GearShift*>(&autoGearShift) ?
            0 : 1;
    }
    void setGameMode(uint8_t gamemode) {
        mode = static_cast<GameMode>(gamemode);
    }
    void setMainChassis(uint8_t chassis) { mainChassis = chassis; }
    void setRivalChassis(uint8_t chassis) { rivalChassis = chassis; }
    uint8_t getMainChassis() { return mainChassis; }

    uint8_t getGameMode() {
        return static_cast<uint8_t>(mode);
    }
    Car* getOtherCar(Car* current) { return current == enemyCar ?
                                            playerCar : enemyCar; }

    enum EndResultType : int8_t {
        NoResult = -1,
        PlayerDeadGearbox = 0,
        PlayerDeadEngine,
        RaceEndLose,
        RaceEndTimeAttack,
        RaceEndWin
    };

    enum GameMode : uint8_t {
        TimeAttack = 0,
        Duel,
        DuelDemo
    };

 private:
    class BackgroundLayer {
     public:
        explicit BackgroundLayer(int16_t factor) : offsetFactor(factor) {
        }
        virtual void draw(SpriteRenderer* renderer,
                            const FP32& cameraPosition) = 0;
        virtual void update(int16_t dt) {}
     protected:
        int16_t offsetFactor;
        int16_t camPosToOffset(const FP32& cameraPosition);
    };

    class BackgroundGrid : public BackgroundLayer {
        int8_t yTop;
        int8_t yBot;
        int8_t density;
        const uint16_t color = Utils::make16BitColor(0xDF, 0x00, 0xFF);
        void drawSingleLine(SpriteRenderer* renderer, int16_t x,
                            int16_t yTop, int16_t yBot);
     public:
        BackgroundGrid(int8_t _yTop, int8_t _yBot,
                       int8_t _density, int16_t factor) :
                                BackgroundLayer(factor), yTop(_yTop),
                                yBot(_yBot), density(_density) {}
        virtual void draw(SpriteRenderer* renderer,
                          const FP32& cameraPosition);
    };

    class BackgroundSprite : public BackgroundLayer {
        int8_t  yPos;
        uint8_t width;
        uint8_t frame;
     public:
        BackgroundSprite(int8_t y, uint8_t width,
                         uint8_t frame, int16_t factor) :
                                BackgroundLayer(factor), yPos(y),
                                width(width), frame(frame) {
            }
        virtual void draw(SpriteRenderer* renderer,
                          const FP32& cameraPosition);
    };

    class BackgroundLineScroll : public BackgroundLayer {
        int8_t  yPos;
        uint8_t element;
     public:
         BackgroundLineScroll(int8_t y, uint8_t element, int16_t factor) :
                                BackgroundLayer(factor), yPos(y),
                                element(element) {
        }
        virtual void draw(SpriteRenderer* renderer,
                          const FP32& cameraPosition);
    };

    class BackgroundBlur : public BackgroundLayer {
        bool enabled;
        int8_t yTop;
        int8_t height;
     public:
        void setEnabled(bool isEnabled) { enabled = isEnabled; }
        BackgroundBlur(int8_t yTop, int8_t yBot) :
                                BackgroundLayer(0),
                                yTop(yTop), height(yBot - yTop + 1),
                                enabled(false) {
        }
        virtual void draw(SpriteRenderer* renderer,
                          const FP32& cameraPosition);
    };

    class BackgroundGradient : public BackgroundLayer {
        int8_t yTop;
        int8_t height;
        int8_t rTop;
        int8_t gTop;
        int8_t bTop;
        int8_t rBot;
        int8_t gBot;
        int8_t bBot;
     public:
        BackgroundGradient(int8_t yTop, int8_t yBot,
                           int8_t rTop, int8_t gTop, int8_t bTop,
                           int8_t rBot, int8_t gBot, int8_t bBot) :
                                BackgroundLayer(0),
                                yTop(yTop), height(yBot - yTop + 1),
                                rTop(rTop), gTop(gTop), bTop(bTop),
                                rBot(rBot), gBot(gBot), bBot(bBot) {
        }
        virtual void draw(SpriteRenderer* renderer,
                          const FP32& cameraPosition);
    };

    FP32 cameraPosition;

    static Level::BackgroundLayer* bgLayers[];
    static BackgroundBlur* backgroundBlur;

    enum LevelState : uint8_t {
        Invalid,
        Countdown,
        Race,
        Result
    };

    enum ScreenAnimType : int8_t {
        None,
        Screen_Sprite,
        Screen_Flag
    };

    enum EntityInstance : int8_t {
        PlayerCar,
        EnemyCar1,
        EnemyCar2,
        Barrel1,
        Barrel2,
        NbInstances
    };

    static constexpr int8_t MaxActiveEntities = 4;
    static constexpr int8_t BlurBackgroundIndex = 8;

    LevelState      state = Countdown;
    GameMode        mode = TimeAttack;
    GearShiftAuto   autoGearShift;
    GearShiftManual manualGearShift;
    GearShift*      currentGearShift = &autoGearShift;
    EndResultType   endResult = NoResult;
    GameObject*     entityRepo[EntityInstance::NbInstances];
    GameObject*     activeEntities[MaxActiveEntities];
    Car*            cameraTarget;
    SpriteAnimator  screenAnim;
    ScreenAnimType  screenAnimType;
    Car*            playerCar;
    Car*            enemyCar;
    uint8_t         scrAnimX;
    uint8_t         scrAnimY;
    int32_t         levelTimer;
    uint8_t         stateCounter;
    uint8_t         maxStateCounter;
    uint8_t         mainChassis;
    uint8_t         rivalChassis;
    uint8_t         nbActiveEntities;
    bool            mustMoveBarrels;
    uint8_t         barrelOne;
    uint8_t         barrelTwo;
    bool            newRecord;

    static constexpr uint8_t kAccelButton = BTN_B;
    static constexpr uint8_t kClutchButton = BTN_A;

    void drawEndFlag(SpriteRenderer* renderer, uint8_t x, uint8_t y, uint8_t w);
    inline void drawHUD(SpriteRenderer* renderer);
    inline void drawCarHUD(SpriteRenderer* renderer, uint8_t x, uint8_t y);
    inline void drawTimer(SpriteRenderer* renderer, uint8_t x, uint8_t y,
                          uint8_t anchor = (ANCHOR_BOTTOM | ANCHOR_LEFT),
                          bool addSign = false);
    inline void drawResult(SpriteRenderer* renderer, uint8_t x, uint8_t y);
    inline void updateState(int16_t dt);
    inline void updateGeneral(int16_t dt);
    inline void updateCamera(int16_t dt);
    inline void drawDistanceToRival(SpriteRenderer* renderer,
                                    uint8_t x, uint8_t y);
    void startScreenAnim(uint8_t x, uint8_t y, ScreenAnimType type,
                         uint8_t sprite = Defs::SpriteHud,
                         uint8_t anim = 0, bool loop = false);
    template<typename F>
    void foreachGameObject(F func);
    void setState(LevelState newState);
    void setEndRace(EndResultType type);
    void checkRecord();
    void exitDemo();
};

#endif  // LEVEL_H_

