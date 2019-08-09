// Copyright 2018 Catalin G. Manciu

#ifndef ENEMYCAR_H_
#define ENEMYCAR_H_

#include "car.h"
#include "level.h"

class EnemyCar final : public Car {
 public:
    EnemyCar(Level* p, uint8_t scrW) :
        Car(p, scrW) {}
    virtual void reset(const FP32& z);
    virtual void update(int16_t dt);
    virtual void onRaceStart();
    virtual void onRaceEnd();
 private:
    static constexpr uint16_t GearChangeRPMStart = Defs::OverheatRPM - 200;
    int16_t stateTimer;

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

