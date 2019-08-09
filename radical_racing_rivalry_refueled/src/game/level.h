// Copyright 2018 Catalin G. Manciu

#ifndef LEVEL_H_
#define LEVEL_H_

#include "../stdinc.h"
#include "../defs.h"
#include "../engine/ardusprite.h"
#include "gearshift.h"
#include "gameobject.h"

class SpriteRenderer;
class Sprite;
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
                           static_cast<GearShift*>(&autoGearShift):
                           static_cast<GearShift*>(&manualGearShift);
    }
    uint8_t getGearMode() {
        return currentGearShift == static_cast<GearShift*>(&autoGearShift) ?
               0 : 1;
    }
    void setGameMode(uint8_t gamemode) {
        mode = static_cast<GameMode>(gamemode);
    }
    uint8_t getGameMode() {
        return static_cast<uint8_t>(mode);
    }
    Car* getPlayer() { return playerCar; }

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

    class BackgroundChopper : public BackgroundLayer {
        bool waiting;
        int8_t  yPos;
        int16_t timer;
        FP32 xPos;
        FP32 xSpeed;    // in px/sec
        SpriteAnimator chopperAnim;
     public:
        BackgroundChopper();
        virtual void draw(SpriteRenderer* renderer,
                          const FP32& cameraPosition);
        virtual void update(int16_t dt);
     private:
        void restart();
        void wait(bool isWaiting);
    };

    FP32 cameraPosition;

    static Level::BackgroundLayer* bgLayers[];

    enum LevelState : uint8_t {
        Invalid,
        Countdown,
        Race,
        Result
    };

    enum GameMode : uint8_t {
        TimeAttack = 0,
        Duel,
    };

    enum EndResultType : int8_t {
        NoResult = -1,
        PlayerDeadGearbox = 0,
        PlayerDeadEngine,
        RaceEndLose,
        RaceEndTimeAttack,
        RaceEndWin
    };

    enum ScreenAnimType : int8_t {
        None,
        Sprite,
        Flag
    };

    LevelState      state = Countdown;
    GameMode        mode = TimeAttack;
    GearShiftAuto   autoGearShift;
    GearShiftManual manualGearShift;
    GearShift*      currentGearShift = &autoGearShift;
    EndResultType   endResult = NoResult;

    SpriteAnimator  screenAnim;
    ScreenAnimType  screenAnimType;
    Car*            playerCar;
    Car*            enemyCar;
    uint8_t         scrAnimX;
    uint8_t         scrAnimY;
    int32_t         levelTimer;
    uint8_t         stateCounter;
    uint8_t         maxStateCounter;
    bool            newRecord;

    static constexpr uint8_t kAccelButton = B_BUTTON;
    static constexpr uint8_t kClutchButton = A_BUTTON;

    void drawEndFlag(SpriteRenderer* renderer, uint8_t x, uint8_t y, uint8_t w);
    inline void drawHUD(SpriteRenderer* renderer);
    inline void drawCarHUD(SpriteRenderer* renderer, uint8_t x, uint8_t y);
    inline void drawTimer(SpriteRenderer* renderer, uint8_t x, uint8_t y,
                          uint8_t anchor = (ANCHOR_BOTTOM | ANCHOR_LEFT),
                          bool addSign = false);
    inline void drawResult(SpriteRenderer* renderer, uint8_t x, uint8_t y);
    inline void updateState(int16_t dt);
    inline void updateGeneral(int16_t dt);
    inline void updateCamera();
    inline void drawDistanceToRival(SpriteRenderer* renderer,
                                    uint8_t x, uint8_t y);
    void startScreenAnim(uint8_t x, uint8_t y, ScreenAnimType type,
                         uint8_t anim = 0, bool loop = false);
    void foreachGameObject(auto func);
    void drawMarker(SpriteRenderer* renderer, const FP32& worldPos);
    void drawGameMarkers(SpriteRenderer* renderer);
    void setState(LevelState newState);
    void setEndRace(EndResultType type);
    void checkRecord();
};

#endif  // LEVEL_H_

