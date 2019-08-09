// Copyright 2018 Catalin G. Manciu

#include "gameobject.h"
#include "level.h"

bool GameObject::isVisible() {
    int32_t minX = screenX - screenW;
    return !(screenX < 0 || minX >= Defs::ScreenW);
}

void GameObject::updateScreenX() {
    screenX = parent->worldToScreenX(xPos, zPos);
}
