// Copyright 2019 Catalin G. Manciu

#include "mainmenu.h"
#include "../../game.h"
#include "../../res/stringmgr.h"
#include "../../res/sprites.h"

void StateMainMenu::stateInit() {
    Menu& m = RRRR::getInstance().getMenu();
    Level& l = RRRR::getInstance().getLevel();
    m.set(getMenuData(0), 3, 96, Defs::AnimMenuMain, Defs::SpriteCar,
          Defs::AnimCarMenu);
    m.setItemOption(0, l.getGameMode());
    m.setItemOption(1, l.getGearMode());
    m.setItemOption(2, l.getMainChassis());
    m.restart();
}

void StateMainMenu::stateUpdate() {
    RRRR& g = RRRR::getInstance();
    Menu& m = g.getMenu();
    Level& l = g.getLevel();
    uint8_t changedButtons = (g.buttonsState ^ g.oldButtonsState);
    m.updateControls(g.buttonsState, g.oldButtonsState);
    fireEffect.update();
    if (changedButtons & g.buttonsState & BTN_B) {
        uint8_t playerChassis = m.getItemOption(2);
        l.setGameMode(m.getItemOption(0));
        l.setGearMode(m.getItemOption(1));
        l.setMainChassis(playerChassis);
        l.setRivalChassis(Utils::random8Except(
                          0, Defs::CarNbChassis, playerChassis));
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
