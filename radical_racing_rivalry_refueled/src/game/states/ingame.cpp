// Copyright 2019 Catalin G. Manciu

#include "ingame.h"
#include "../../game.h"

void StateIngame::stateInit() {
}

void StateIngame::stateUpdate() {
    R3R& g = R3R::getInstance();
    Level& l = g.getLevel();
    l.updateControls(g.buttonsState, g.oldButtonsState);
    l.update(33);
}

void StateIngame::stateRender(SpriteRenderer* renderer) {
    Level& l = R3R::getInstance().getLevel();
    l.draw(renderer);
}
