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
    Car(Level* p, uint8_t scrW);
    virtual void reset(const FP32& x, const FP32& z);
    virtual void draw(SpriteRenderer* renderer);
    virtual void update(int16_t dt);
    FP32& getRPM()                      { return engineRPM; }
    FP32& getSpeed()                    { return speed; }
    int8_t getGear()                    { return gear; }
    void shiftGear(bool up = true);
    void shiftGear(int8_t newGear);
    void pedalToTheMetal(bool on);
    void setClutch(bool on)             { clutch = on; }
    bool isClutched()                   { return clutch; }
    bool isAlive()                      { return alive; }
    bool isExplosionVisible()           { return explosion.animPlaying(); }
    int8_t getMaxGear()                 { return Defs::MaxGear; }
    void destroy();
    int8_t getOverheat()                { return overheatCounter; }
    void setChassis(uint8_t chassis);

 protected:
    int8_t gear;
    int8_t lastReflectionPos;
    uint8_t overheatCounter;
    uint8_t chassisIdx;
    FP32 engineRPM;
    FP32 wheelsRPM;
    FP32 speed;
    FP32 throttle;
    bool clutch;
    bool alive;
    bool carStarted;
    SpriteAnimator reflection;
    SpriteAnimator wheels;
    SpriteAnimator explosion;
    inline void updateEngine(int16_t dt);
    inline void updateWheelsAnim(int16_t dt);
    inline void updateReflectionAnim(int16_t dt);
    inline void onCarStart();
    inline void onEngineBlown();
    inline void onOverheatChanged(uint8_t oldValue, uint8_t newValue);
    void setOverheatCounter(uint8_t newValue);
    template<typename F>
    void foreachCarLight(F func,
                         uint8_t start = 0, uint8_t end = NbCarLights) {
        for (uint8_t light = start; light < end; ++light) {
            func(carLights[light]);
        }
    }
    bool engineIsConnected() { return gear > 0 && clutch == false && alive; }

    class CarLight {
     public:
         enum State : uint8_t {
             Inactive,
             Off,
             On,
             Blinking,
             Blinking_Out
         };
         explicit CarLight(uint8_t anim, uint8_t frame):
                           anim(anim), frame(frame), state(Off) {}
         virtual void reset() {
             state = Off;
         }
         virtual void onCarStart() {
             state = On;
         }
         virtual void onOverheatChanged(uint8_t oldValue, uint8_t newValue) {
         }
         virtual void onSetChassis(uint8_t chassis) {}
         virtual void onEngineBlown() {
             blinkTimeout = 40 + (rand() & 0xF);
             blinkRate = 5 + (rand() & 0x3);
             frameCounter = 0;
             state = Blinking_Out;
         }
         virtual void update();
         virtual void draw(SpriteRenderer* renderer, int16_t x, int16_t y);

     protected:
         State state;
         uint8_t frame;
         uint8_t anim;
         uint8_t blinkRate;
         uint8_t blinkTimeout;
         uint8_t frameCounter;
    };

    class NeonLight : public CarLight {
     public:
        explicit NeonLight(uint8_t anim, uint8_t frame) :
                    CarLight(anim, frame) {}
        virtual void reset() {
            state = On;
        }
        virtual void onSetChassis(uint8_t chassis) {
            frame = chassis;
        }
    };

    class AlertLight : public CarLight {
     public:
        explicit AlertLight(uint8_t anim, uint8_t frame) :
                    CarLight(anim, frame) {}
        virtual void reset() {
            state = Off;
        }
        virtual void onOverheatChanged(uint8_t oldValue, uint8_t newValue) {
            if (state != Inactive) {
                if (newValue == 0) {
                    state = Off;
                } else {
                    if (state != Blinking) {
                        frameCounter = 0;
                        state = Blinking;
                    }
                    if (newValue < Defs::MaxOverheat / 2) {
                        blinkRate = 8;
                    } else {
                        blinkRate = 4;
                    }
                }
            }
        }
        virtual void onEngineBlown() {
            state = Inactive;
        }
        virtual void onCarStart() {
        }
    };

    static constexpr uint8_t NbCarLights = 3;
    static constexpr uint8_t NbBgCarLights = 1;
    CarLight* carLights[NbCarLights];
};


#endif  // CAR_H_

