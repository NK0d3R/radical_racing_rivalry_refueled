// Copyright 2019 Catalin G. Manciu

#include "ingame.h"
#include "../../game.h"

void StateIngame::stateInit() {
}

void StateIngame::stateUpdate(int16_t dt) {
    R3R& g = R3R::getInstance();
    Level& l = g.getLevel();
    l.updateControls(g.buttonsState, g.oldButtonsState);
    l.update(dt);
}

void StateIngame::stateRender(SpriteRenderer* renderer) {
    Level& l = R3R::getInstance().getLevel();
    l.draw(renderer);
}
