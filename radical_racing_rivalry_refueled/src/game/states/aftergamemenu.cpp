// Copyright 2019 Catalin G. Manciu

#include "aftergamemenu.h"
#include "../../game.h"

void StateAfterGameMenu::stateInit() {
    Menu& m = R3R::getInstance().getMenu();
    m.set(getMenuData(1), 2, 80, 0);
    m.restart();
}

void StateAfterGameMenu::stateUpdate() {
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
    R3R::getInstance().getMenu().draw(renderer, (Defs::ScreenW >> 1), 16);
}
