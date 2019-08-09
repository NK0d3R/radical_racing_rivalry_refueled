// Copyright 2018 Catalin G. Manciu

#ifndef CAR_H_
#define CAR_H_

#include "../stdinc.h"
#include "../defs.h"
#include "gameobject.h"
#include "../res/sprites.h"
#include "../engine/ardusprite.h"

class SpriteRenderer;
class Level;
class Car : public GameObject {
 public:
    Car(Level* p, uint8_t scrW) :
        GameObject(p, scrW) {
    }
    virtual void reset(const FP32& z);
    virtual void draw(SpriteRenderer* renderer);
    virtual void update(int16_t dt);
    FP32& getRPM()                      { return engineRPM; }
    FP32& getSpeed()                    { return speed; }
    int8_t getGear()                    { return gear; }
    void shiftGear(bool up = true);
    void shiftGear(int8_t newGear)      { gear = newGear; }
    void pedalToTheMetal(bool on);
    void setClutch(bool on)             { clutch = on; }
    bool isClutched()                   { return clutch; }
    bool isAlive()                      { return alive; }
    int8_t getMaxGear()                 { return Defs::MaxGear; }
    void destroy();
    int8_t getOverheat()               { return overheatCounter; }

 protected:
    int8_t gear;
    int8_t lastReflectionPos;
    int8_t overheatCounter;
    FP32 engineRPM;
    FP32 wheelsRPM;
    FP32 speed;
    FP32 throttle;
    bool clutch;
    bool alive;
    SpriteAnimator reflection;
    SpriteAnimator wheels;
    SpriteAnimator explosion;
    inline void updateEngine(int16_t dt);
    inline void updateWheelsAnim(int16_t dt);
    inline void updateReflectionAnim(int16_t dt);
    bool engineIsConnected() { return gear > 0 && clutch == false && alive; }
};

#endif  // CAR_H_

