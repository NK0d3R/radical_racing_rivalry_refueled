// Copyright 2019 Catalin G. Manciu

#include "mainmenu.h"
#include "../../game.h"
#include "../../res/stringmgr.h"
#include "../../res/sprites.h"

void StateMainMenu::stateInit() {
    Menu& m = RRRR::getInstance().getMenu();
    Level& l = RRRR::getInstance().getLevel();
    m.set(getMenuData(0), 3, 100, Defs::AnimMenuMain);
    m.setItemOption(0, l.getGameMode());
    m.setItemOption(1, l.getGearMode());
    m.restart();
}

void StateMainMenu::stateUpdate() {
    RRRR& g = RRRR::getInstance();
    Menu& m = g.getMenu();
    Level& l = g.getLevel();
    m.updateControls(g.buttonsState, g.oldButtonsState);
    fireEffect.update();
    if (m.getAction() == Defs::MenuActionStart) {
        l.setGameMode(m.getItemOption(0));
        l.setGearMode(m.getItemOption(1));
        l.restart();
        g.setState(Ingame);
    }
}

void StateMainMenu::stateRender(SpriteRenderer* renderer) {
    RRRR& g = RRRR::getInstance();
    Menu& m = g.getMenu();
    fireEffect.render(renderer);
    m.draw(renderer, (Defs::ScreenW >> 1), 0);
    uint8_t selectedGameMode = m.getItemOption(0);
    getString(Strings::BestTime);
    Utils::formatTime(g.getTimeRecord(selectedGameMode,
                                      m.getItemOption(1)),
                      getStringBuffer() +
                      getStringLen(Strings::BestTime),
                      selectedGameMode == 1);
    GetFont(Defs::FontMain)->drawString(
                renderer, getStringBuffer(),
                Defs::ScreenW / 2, 64, ANCHOR_BOTTOM | ANCHOR_HCENTER, -1);
}
