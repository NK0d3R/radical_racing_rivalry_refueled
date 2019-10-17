// Copyright 2018 Catalin G. Manciu

#include "enemycar.h"

void EnemyCar::reset(const FP32& x, const FP32& z) {
    Car::reset(x, z);
    state = Invalid;
    setState(Wait);
}

static constexpr uint8_t kGearSwitchingTimes[]  = {
    25, 60,       // Auto gear mode (in centisecs)
    40, 75        // Manual gear mode (in centisecs)
};

void EnemyCar::setState(AIState newState) {
    if (state != newState) {
        switch (newState) {
            case SwitchingGears: {
                uint8_t gearTimeIdx = (parent->getGearMode() << 1);
                stateTimer = random(kGearSwitchingTimes[gearTimeIdx],
                                    kGearSwitchingTimes[gearTimeIdx + 1]);
                stateTimer *= 10;
                setClutch(true);
                shiftGear(true);
            } break;
            case Accelerating: {
                rollCounter = 0;
                setClutch(false);
            } break;
        }
        state = newState;
    }
}

void EnemyCar::update(int16_t dt) {
    bool isAhead = parent->getPlayer()->getX() <= getX();
    bool accelerate = false;
    switch (state) {
        case SwitchingGears: {
            stateTimer -= dt;
            if (!isAhead) {
                stateTimer -= (dt >> 1);  // Faster reaction when behind
            }
            if (stateTimer <= 0) {
                setState(Accelerating);
            }
        } break;
        case Accelerating: {
            if (getGear() == Defs::MaxGear) {
                accelerate = overheatCounter < (Defs::MaxOverheat >> 2);
                break;
            }
            uint16_t switchChance = 0;
            if (isAhead) {
                uint16_t iRPM = getRPM().getInt();
                if (iRPM > GearChangeRPMStart) {
                    if (iRPM < Defs::OverheatRPM) {
                        switchChance = (iRPM - GearChangeRPMStart) / 4;
                    } else {
                        switchChance = 50 + (iRPM - Defs::OverheatRPM) / 2;
                    }
                }
            } else {
                if (overheatCounter > 0) {
                    switchChance = (
                        static_cast<uint16_t>(overheatCounter) * 120 /
                        static_cast<uint16_t>(Defs::MaxOverheat));
                }
            }
            accelerate = true;
            if (switchChance > 0) {
                if (rollCounter == 0) {
                    if (random(0, 100) < switchChance) {
                        accelerate = false;
                        setState(SwitchingGears);
                    } else {
                        rollCounter = ReRollCooldown;
                    }
                } else {
                    rollCounter--;
                }
            }
        } break;
    }
    pedalToTheMetal(accelerate);
    Car::update(dt);
    if (!alive) {
        setState(Dead);
    }
}

void EnemyCar::onRaceStart() {
    setState(SwitchingGears);
}

void EnemyCar::onRaceEnd(uint8_t endStatus) {
    if (endStatus > Level::PlayerDeadEngine && state != Dead) {
        setState(Wait);
    }
}
