// Copyright 2019 Catalin G. Manciu

#include "aftergamemenu.h"
#include "../../game.h"

void StateAfterGameMenu::stateInit() {
    Menu& m = R3R::getInstance().getMenu();
    m.set(getMenuData(1), 2, 96, 0);
    m.restart();
}

void StateAfterGameMenu::stateUpdate(int16_t dt) {
    R3R& g = R3R::getInstance();
    Menu& m = g.getMenu();
    Level& l = g.getLevel();
    m.updateControls(g.buttonsState, g.oldButtonsState);
    switch (m.getAction()) {
        case Defs::MenuActionRestart:
            l.restart();
            g.setState(Ingame);
        break;
        case Defs::MenuActionBackToMM:
            g.setState(MainMenu);
        break;
    }
}

void StateAfterGameMenu::stateRender(SpriteRenderer* renderer) {
    renderer->drawVerticalGradient(0, 0, 0,
                                   10, 8, 10);
    R3R::getInstance().getMenu().draw(renderer, (Defs::ScreenW >> 1), 14);
}
