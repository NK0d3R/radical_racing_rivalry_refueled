// Copyright 2018 Catalin G. Manciu

#ifndef ENEMYCAR_H_
#define ENEMYCAR_H_

#include "car.h"
#include "level.h"

class EnemyCar final : public Car {
 public:
    using Car::Car;
    virtual void reset(const FP32& x, const FP32& z);
    virtual void update(int16_t dt);
    virtual void onRaceStart();
    virtual void onRaceEnd(uint8_t endResult);
 private:
    static constexpr uint16_t GearChangeRPMStart = Defs::OverheatRPM - 200;
    static constexpr int8_t ReRollCooldown = 2;
    int16_t stateTimer;
    int8_t rollCounter;

    enum AIState : uint8_t {
        Invalid,
        Wait,
        SwitchingGears,
        Accelerating,
        Dead
    };

    AIState state;
    void setState(AIState newState);
};

#endif  // ENEMYCAR_H_

