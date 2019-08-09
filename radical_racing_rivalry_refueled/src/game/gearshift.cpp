// Copyright 2018 Catalin G. Manciu

#include "../defs.h"
#include "gearshift.h"
#include "../res/sprites.h"
#include "../engine/renderer.h"

void GearShiftAuto::reset() {
    scrollAmount = 0;
    currentGear = 0;
    state = Idle;
}

void GearShiftAuto::update() {
    if (state != Idle) {
        scrollAmount += GearScrollPerFrame;
        if (scrollAmount >= GearDisplH) {
            currentGear += (static_cast<int16_t>(state));
            scrollAmount = 0;
            state = Idle;
        }
    }
}

void GearShiftAuto::draw(SpriteRenderer* renderer, int16_t x, int16_t y) {
    uint8_t arrowOffset = (getFrameCounter() >> 1) & 0x3;
    Sprite* car = GetSprite(Defs::SpriteCar);
    car->drawAnimationFrame(renderer, Defs::AnimCarGearsAuto,
                            Defs::HUDGearsManualBg, x, y, 0);
    if (currentGear < Defs::MaxGear) {
        car->drawAnimationFrame(renderer, Defs::AnimCarGearsAuto,
                                Defs::HUDGearsManualUp, x,
                                y - arrowOffset, 0);
    }
    if (currentGear > 0) {
        car->drawAnimationFrame(renderer, Defs::AnimCarGearsAuto,
                                Defs::HUDGearsManualDown, x,
                                y + arrowOffset, 0);
    }
    renderer->setClip(x - (GearDisplW >> 1), y - (GearDisplH >> 1),
                      GearDisplW, GearDisplH);
    if (state != Idle) {
        y = y + (static_cast<int16_t>(state)) * scrollAmount;
    }
    car->drawAnimationFrame(renderer, Defs::AnimCarGearsAuto,
                            currentGear, x, y, 0);
    if (state != Idle) {
        if (currentGear < Defs::MaxGear) {
            car->drawAnimationFrame(renderer, Defs::AnimCarGearsAuto,
                                    currentGear + 1,
                                    x, y - GearDisplH, 0);
        }
        if (currentGear > 0) {
            car->drawAnimationFrame(renderer, Defs::AnimCarGearsAuto,
                                    currentGear - 1,
                                    x, y + GearDisplH, 0);
        }
    }
    renderer->setClip(0, 0, Defs::ScreenW, Defs::ScreenH);
}

void GearShiftAuto::onUp() {
    if (state == Idle && currentGear < Defs::MaxGear) {
        state = ShiftUp;
    }
}

void GearShiftAuto::onDown() {
    if (state == Idle && currentGear > 0) {
        state = ShiftDown;
    }
}

void GearShiftManual::reset() {
    currentGear = 0;
    currentX = 1;
    currentY = 1;
    currentOffset = 0;
    targetOffset = 0;
    state = Idle;
}

void GearShiftManual::getNextPosition(ShiftState direction,
                                      int8_t* x, int8_t* y) {
    switch (direction) {
        case Left:  (*x)--; break;
        case Right: (*x)++; break;
        case Up:    (*y)--; break;
        case Down:  (*y)++; break;
    }
}

int8_t GearShiftManual::getGearAtPosition(int8_t x, int8_t y) {
    static constexpr int32_t speedGrid = ((1l << 0)  | (3l << 3)  | (5l << 6)  |
                                          (0l << 9)  | (0l << 12) | (0l << 15) |
                                          (2l << 18) | (4l << 21) | (7l << 24));
    int8_t bitset = ((y * 3) + x) * 3;
    return ((speedGrid >> bitset) & 7);
}

bool GearShiftManual::canGoDirection(ShiftState direction) {
    int8_t newX = currentX;
    int8_t newY = currentY;
    if (currentY != 1 && (direction == Left || direction == Right)) {
        return false;
    }
    getNextPosition(direction, &newX, &newY);
    if (newX < 0 || newX > 2 || newY < 0 || newY > 2 ||
        getGearAtPosition(newX, newY) == 7) {
            return false;
    }
    return true;
}

void GearShiftManual::tryGoDirection(ShiftState direction) {
    if (state != Idle) return;
    if (canGoDirection(direction)) {
        state = direction;
        targetOffset = (direction == Up || direction == Down) ? StickVStep :
                        StickHStep;
    }
}

void GearShiftManual::update() {
    if (state != Idle) {
        int8_t offIncr = (state == Up || state == Down) ?
                          StickMovePerFrameV : StickMovePerFrameH;
        currentOffset += offIncr;
        if (currentOffset >= targetOffset) {
            currentOffset = 0;
            getNextPosition(state, &currentX, &currentY);
            state = Idle;
        }
    }
}

void GearShiftManual::draw(SpriteRenderer* renderer, int16_t x, int16_t y) {
    Sprite* car = GetSprite(Defs::SpriteCar);
    car->drawAnimationFrame(renderer, Defs::AnimCarGearsManual,
                            Defs::HUDGearsAutoBg, x, y, 0);
    int16_t xPos = x + (currentX - 1) * StickHStep;
    int16_t yPos = y + (currentY - 1) * StickVStep;
    switch (state) {
        case Left:  xPos -= currentOffset; break;
        case Right: xPos += currentOffset; break;
        case Up:    yPos -= currentOffset; break;
        case Down:  yPos += currentOffset; break;
    }
    car->drawAnimationFrame(renderer, Defs::AnimCarGearsManual,
                            Defs::HUDGearsAutoStick, xPos, yPos, 0);
}
