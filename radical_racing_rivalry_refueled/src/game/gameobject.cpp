// Copyright 2019 Catalin G. Manciu

#include "gameobject.h"
#include "level.h"

bool GameObject::isVisible() {
    int32_t minX = screenX + screenMinX;
    int32_t maxX = screenX + screenMaxX;
    return !(maxX < 0 || minX >= Defs::ScreenW);
}

void GameObject::updateScreenX() {
    screenX = parent->worldToScreenX(xPos, zPos);
}

void GameObject::reset(const FP32& x, const FP32& z) {
    xPos = x;
    zPos = z;
    screenY = parent->worldToScreenY(xPos, z);
}
