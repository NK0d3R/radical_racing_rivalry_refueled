// Copyright 2018 Catalin G. Manciu

#include "enemycar.h"

void EnemyCar::reset(const FP32& z) {
    Car::reset(z);
    state = Invalid;
    setState(Wait);
}

static constexpr uint8_t kGearSwitchingTimes[] PROGMEM = {
    25, 60,       // Auto gear mode (in centisecs)
    50, 75        // Manual gear mode (in centisecs)
};

void EnemyCar::setState(AIState newState) {
    if (state != newState) {
        switch (newState) {
            case SwitchingGears: {
                uint8_t gearTimeIdx = (parent->getGearMode() << 1);
                stateTimer = random(
                        pgm_read_byte(&kGearSwitchingTimes[gearTimeIdx]),
                        pgm_read_byte(&kGearSwitchingTimes[gearTimeIdx + 1]));
                stateTimer *= 10;
                setClutch(true);
                shiftGear(true);
            } break;
            case Accelerating: {
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
            if (switchChance > 0 && random(0, 100) < switchChance) {
                setState(SwitchingGears);
            } else {
                accelerate = true;
            }
        } break;
    }
    pedalToTheMetal(accelerate);
    if (!alive) {
        setState(Dead);
    }
    Car::update(dt);
}

void EnemyCar::onRaceStart() {
    setState(SwitchingGears);
}

void EnemyCar::onRaceEnd() {
    setState(Wait);
}
