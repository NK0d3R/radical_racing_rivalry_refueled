// Copyright 2018 Catalin G. Manciu

#include "../defs.h"
#include "car.h"
#include "level.h"
#include "../res/sprites.h"
#include "../engine/renderer.h"

// Torque (in Nm)
constexpr int16_t kTorques[] PROGMEM = {
    226,                // 1000 RPM
    395,                // 2000 RPM
    452,                // 3000 RPM
    480,                // 4000 RPM
    500,                // 5000 RPM
    520,                // 6000 RPM
    550,                // 7000 RPM
    580                 // 8000 RPM
};

constexpr uint8_t kNbTorques = (sizeof(kTorques)/sizeof(FP32));

constexpr FP32 kGearRatios[] PROGMEM = {
    FP32(0.0f),
    FP32(9.13f),
    FP32(6.65f),
    FP32(4.43f),
    FP32(3.51f),
    FP32(2.89f)
};

constexpr FP32 kWheelCircumference(2.0f * 3.141539f * 0.303f);
constexpr FP32 kWheelRadius = FP32(0.303f);
constexpr FP32 kVehicleMass = FP32(1490);

constexpr int16_t kWindResistanceMult = -26;
constexpr int16_t kWindResistanceDiv =  100;
constexpr FP32 kWindResistanceConst(-600);

FP32 RPM2Torque(FP32 rpm) {
    if (rpm > Defs::MaxRPM) {
        return FP32(0);
    }
    rpm.clampLower(Defs::MinRPM);
    int16_t rpmI = rpm.getInt();
    int16_t idx = (rpmI / 1000);
    FP32 crtTorque((int16_t)pgm_read_word(&kTorques[idx]));
    if (idx == (kNbTorques - 1)) {
        return FP32(crtTorque);
    }
    int16_t rpmOver = rpmI - (idx * 1000);
    if (rpmOver == 0) {
        return FP32(crtTorque);
    }
    FP32 endTorque((int16_t)pgm_read_word(&kTorques[idx + 1]));
    return crtTorque + (((endTorque - crtTorque) * rpmOver) / 1000);
}

FP32 getGearRatio(int16_t idx) {
    return FP32(&kGearRatios[idx]);
}

void Car::reset(const FP32& z) {
    xPos = 0;
    zPos = z;
    screenY = parent->worldToScreenY(xPos, z);
    engineRPM = 0;
    wheelsRPM = 0;
    speed = 0;
    gear = 0;
    throttle = 0;
    overheatCounter = 0;
    clutch = false;
    alive = true;
    lastReflectionPos = 0;

    wheels.init(GetSprite(Defs::SpriteCar));
    reflection.init(GetSprite(Defs::SpriteCar));
    explosion.init(GetSprite(Defs::SpriteCar));
    wheels.setAnimation(Defs::AnimCarWheels, 0, true);
}

void Car::shiftGear(bool up = true) {
    if (up == true) {
        if (gear < Defs::MaxGear) {
            gear++;
        }
    } else {
        if (gear > 0) {
            gear--;
        }
    }
}

void Car::destroy() {
     alive = false;
     explosion.setAnimation(Defs::AnimCarExplosion, 0, false);
}

void Car::draw(SpriteRenderer* renderer) {
    GetSprite(Defs::SpriteCar)->drawAnimationFrame(renderer, Defs::AnimCarBody,
                                              0, screenX, screenY, 0);
    wheels.draw(renderer, screenX - 34, screenY);
    wheels.draw(renderer, screenX - 10, screenY);
    if (reflection.animPlaying()) {
        reflection.draw(renderer, screenX, screenY);
    }
    if (explosion.animPlaying()) {
        explosion.draw(renderer, screenX, screenY);
    }
}

void Car::pedalToTheMetal(bool on) {
    if (on) {
        FP32 maxThrottle(1);
        if (throttle < maxThrottle) {
            throttle += FP32(0.1f);
            throttle.clampUpper(maxThrottle);
        }
    } else {
        throttle = 0;
    }
}

void Car::updateEngine(int16_t dt) {
    bool engineConnected = engineIsConnected();

    FP32 forwardForce = 0;
    FP32 engineTorque = 0;
    FP32 fpDT = FP32(dt);

    wheelsRPM = (speed * 60) / kWheelCircumference;

    if (engineConnected) {
        engineRPM = getGearRatio(gear) * wheelsRPM;
        engineTorque = RPM2Torque(engineRPM.getInt());
        forwardForce = (throttle * engineTorque * getGearRatio(gear)) /
                        kWheelRadius;
    } else {
        if (throttle > FP32(0)) {
            engineRPM += (throttle * Defs::IdleRPMModif) * dt;
        } else {
            engineRPM -= Defs::IdleRPMModif * dt;
        }
        engineRPM.clamp(Defs::MinRPM, Defs::MaxRPM);
    }

    forwardForce += kWindResistanceConst +
                     (speed * speed * kWindResistanceMult) /
                     kWindResistanceDiv;
    FP32 accel = forwardForce / kVehicleMass;
    speed += (accel * fpDT) / 1000;
    speed.clampLower(FP32(0));

    if (alive) {
        if (engineRPM > Defs::MaxRPM) {
            destroy();
        } else {
            int16_t engineRPMi = engineRPM.getInt();
            uint8_t baseOverheat = 1 + (gear >> 1);
            if (throttle > 0 && engineRPMi >= Defs::OverheatRPM) {
                uint8_t overheatIncrease = baseOverheat +
                                        (engineRPMi - Defs::OverheatRPM) /
                                        Defs::OverheatDiv;
                overheatCounter += overheatIncrease;
                if (overheatCounter > Defs::MaxOverheat) {
                    destroy();
                }
            } else {
                if (engineRPMi < Defs::OverheatRPM) {
                    overheatCounter = 0;
                } else {
                    uint8_t coolingSpeed = 4 - baseOverheat;
                    if (overheatCounter >= (coolingSpeed + 1)) {
                        overheatCounter -= coolingSpeed;
                    } else {
                        overheatCounter = 1;  // don't go to 0 while RPM is high
                    }
                }
            }
        }
    }

    xPos += (speed * fpDT) / 1000;
}

void Car::updateWheelsAnim(int16_t dt) {
    int16_t newDT = ((wheelsRPM * (int32_t)dt) / 125).getInt();
    newDT = Utils::upperClamp(newDT, 200);
    wheels.update(newDT);
}

void Car::updateReflectionAnim(int16_t dt) {
    int8_t crtReflectionPos = xPos.getInt() / 250;
    if (lastReflectionPos != crtReflectionPos) {
        reflection.setAnimation(Defs::AnimCarReflection, 0, false);
        lastReflectionPos = crtReflectionPos;
    }
    reflection.update(((Utils::mpsToKph(speed) * (int32_t)dt) / 120).getInt());
}

void Car::update(int16_t dt) {
    updateEngine(dt);
    updateWheelsAnim(dt);
    updateReflectionAnim(dt);
    explosion.update(dt);
}
