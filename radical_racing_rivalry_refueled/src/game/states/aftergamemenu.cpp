// Copyright 2019 Catalin G. Manciu

#include "aftergamemenu.h"
#include "../../game.h"

void StateAfterGameMenu::stateInit() {
    Menu& m = RRRR::getInstance().getMenu();
    m.set(getMenuData(1), 2, 80, 0);
    m.restart();
}

void StateAfterGameMenu::stateUpdate() {
    RRRR& g = RRRR::getInstance();
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
    RRRR::getInstance().getMenu().draw(renderer, (Defs::ScreenW >> 1), 16);
}
