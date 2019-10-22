// Copyright 2019 Catalin G. Manciu

#include "mainmenu.h"
#include "../../game.h"
#include "../../res/stringmgr.h"
#include "../../res/sprites.h"

void StateMainMenu::stateInit() {
    R3R& g = R3R::getInstance();
    Menu& m = R3R::getInstance().getMenu();
    Level& l = R3R::getInstance().getLevel();
    m.set(getMenuData(0), 3, 96, Defs::AnimMenuMain, Defs::SpriteCar,
          Defs::AnimCarMenu);
    m.setItemOption(0, g.getSavedGameMode());
    m.setItemOption(1, g.getSavedGearMode());
    m.setItemOption(2, g.getSavedChassis());
    m.restart();
}

void StateMainMenu::stateUpdate(int16_t dt) {
    R3R& g = R3R::getInstance();
    Menu& m = g.getMenu();
    Level& l = g.getLevel();
    uint8_t changedButtons = (g.buttonsState ^ g.oldButtonsState);
    m.updateControls(g.buttonsState, g.oldButtonsState);
    fireEffect.update();
    if (changedButtons & g.buttonsState & BTN_A) {
        uint8_t gameMode = m.getItemOption(0);
        uint8_t gearMode = m.getItemOption(1);
        uint8_t playerChassis = m.getItemOption(2);
        l.setGameMode(m.getItemOption(0));
        l.setGearMode(m.getItemOption(1));
        l.setMainChassis(playerChassis);
        l.setRivalChassis(Utils::random8Except(
                          0, Defs::CarNbChassis, playerChassis));
        l.restart();
        g.saveGameConfig(gameMode, gearMode, playerChassis);
        g.saveSave();
        g.setState(Ingame);
    }
}

void StateMainMenu::stateRender(SpriteRenderer* renderer) {
    R3R& g = R3R::getInstance();
    Menu& m = g.getMenu();
    fireEffect.render(renderer);
    m.draw(renderer, (Defs::ScreenW >> 1), 0);
    uint8_t selectedGameMode = m.getItemOption(0);
    uint8_t selectedGearMode = m.getItemOption(1);
    bool showWins = (selectedGameMode == 1) &&
                    (stateFrameCounter & 255) < 128;
    if (showWins) {
        getString(Strings::Wins);
        Utils::getDigits(g.getNbDuelWins(selectedGearMode),
                         getStringBuffer() + getStringLen(Strings::Wins));
    } else {
        getString(Strings::BestTime);
        Utils::formatTime(g.getTimeRecord(selectedGameMode,
                                          selectedGearMode),
                          getStringBuffer() +
                          getStringLen(Strings::BestTime),
                          selectedGameMode == 1);
    }
    GetFont(Defs::FontMain)->drawString(
        renderer, getStringBuffer(),
        Defs::ScreenW / 2, 64, ANCHOR_BOTTOM | ANCHOR_HCENTER, -1);
    drawButtonHint(95, m.getCurrentSelection() != 2 ? 56 : 36, renderer);
}

void StateMainMenu::drawButtonHint(uint8_t x, uint8_t y,
                                   SpriteRenderer* renderer) {
    GetSprite(Defs::SpriteMenu)->drawAnimationFrame(
                renderer, Defs::AnimMenuElements,
                Defs::ButtonHint +
                ((stateFrameCounter & 31) < 15 ? 0 : 1), x, y, 0);
}
