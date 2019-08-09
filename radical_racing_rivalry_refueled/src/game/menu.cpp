// Copyright 2018 Catalin G. Manciu

#include "menu.h"
#include "../res/stringmgr.h"
#include "../res/sprites.h"
#include "../engine/ardusprite.h"
#include "../engine/renderer.h"

/*
 * Menu Data bits: [high] h h h h h i i i i f o o a a a [low]
 * a - action ID
 * o - number of selectable sub-options
 * f - sprite flag (if 1, item is sprite, if 0, item is text)
 * i - frame idx if sprite flag is 1, string idx if sprite flag is 0
 * h - height
 */

void Menu::drawSpriteElementBackground(SpriteRenderer* renderer,
                                       int16_t x, int16_t y, int8_t height,
                                       bool hasArrows) {
    Sprite* spr = GetSprite(Defs::SpriteMenu);
    static int16_t bgTileW = GET_W_FROM_SIZE(spr->measureAnimationFrame(
                                                    Defs::AnimMenuElements,
                                                    Defs::MenuItemBg));
    renderer->setClip(x - (width >> 1), y - (height >> 1), width, height);
    int16_t maxX = renderer->getClip().maxX;
    uint32_t frame = getFrameCounter();
    for (uint8_t scale = 2; scale <= 3; ++scale) {
        int16_t offset = (frame * (scale << 1)) % bgTileW;
        int16_t crtX = renderer->getClip().x - offset;
        while (crtX <= maxX) {
            spr->drawAnimationFrame(renderer,
                                    Defs::AnimMenuElements, (scale - 2),
                                    crtX, y, 0);
            crtX += bgTileW;
        }
    }
    renderer->drawLine(renderer->getClip().x, renderer->getClip().y,
                       maxX, renderer->getClip().y);
    renderer->drawLine(renderer->getClip().x, renderer->getClip().maxY,
                       maxX, renderer->getClip().maxY);
    if (hasArrows) {
        uint8_t arrowOffset = 3 + ((frame & 5) >> 2);
        spr->drawAnimationFrame(renderer,
                                Defs::AnimMenuElements, Defs::MenuArrowL,
                                renderer->getClip().x + arrowOffset, y, 0);
        spr->drawAnimationFrame(renderer,
                                Defs::AnimMenuElements, Defs::MenuArrowR,
                                maxX - arrowOffset, y, 0);
    }
    renderer->setClip(0, 0, Defs::ScreenW, Defs::ScreenH);
}

void Menu::draw(SpriteRenderer* renderer, uint8_t x, uint8_t y) {
    for (uint8_t idx = 0; idx < nbItems; ++idx) {
        uint16_t itemData = pgm_read_word(&menuData[idx]);
        uint8_t height = itemHeight(itemData);
        uint8_t visualID = itemVisualID(itemData);
        uint8_t itemOptionCnt = itemOptionCount(itemData);
        if (itemOptionCnt) {
            visualID += getItemOption(idx);
        }
        y += (height >> 1);
        if (crtSelection == idx) {
            drawSpriteElementBackground(renderer, x, y, height,
                                        itemOptionCnt > 0);
        }
        if (itemSpriteFlag(itemData) == 1) {
            Sprite* spr = GetSprite(Defs::SpriteMenu);
            spr->drawAnimationFrame(renderer, animation, visualID, x,
                                    y, 0);
        } else {
            Font* font = GetFont(Defs::FontMain);
            font->drawString(renderer, getString(visualID), x, y,
                             ANCHOR_HCENTER | ANCHOR_VCENTER);
        }
        y += (height >> 1);
    }
}

void Menu::updateControls(uint8_t buttonsState, uint8_t oldButtonsState) {
    uint8_t changedButtons = (buttonsState ^ oldButtonsState);
    if ((changedButtons & buttonsState & UP_BUTTON)) {
        onUp();
    }
    if ((changedButtons & buttonsState & DOWN_BUTTON)) {
        onDown();
    }
    if ((changedButtons & buttonsState & LEFT_BUTTON)) {
        onLeft();
    }
    if ((changedButtons & buttonsState & RIGHT_BUTTON)) {
        onRight();
    }
    if ((changedButtons & buttonsState & B_BUTTON)) {
        onConfirm();
    }
}

void Menu::onUp() {
    crtSelection = crtSelection == 0 ? nbItems - 1 : crtSelection - 1;
}

void Menu::onDown() {
    crtSelection = crtSelection == nbItems - 1 ? 0 : crtSelection + 1;
}

void Menu::changeCrtItemOption(bool left) {
    uint16_t itemData = pgm_read_word(&menuData[crtSelection]);
    uint8_t options = itemOptionCount(itemData);
    if (options) {
        uint8_t newOption = getItemOption(crtSelection);
        if (left) {
            newOption = newOption == 0 ? options : newOption - 1;
        } else {
            newOption = newOption == options ? 0 : newOption + 1;
        }
        setItemOption(crtSelection, newOption);
    }
}

void Menu::onLeft() {
    changeCrtItemOption(true);
}

void Menu::onRight() {
    changeCrtItemOption(false);
}

void Menu::onConfirm() {
     uint16_t itemData = pgm_read_word(&menuData[crtSelection]);
     action = itemActionID(itemData);
}

void Menu::restart() {
    action = 0;
    crtSelection = 0;
}

PROGMEM const int16_t mainmenu[] = {
    Menu::itemDataCreate(0, 1, 1, 0, 24),
    Menu::itemDataCreate(0, 1, 1, 2, 18),
    Menu::itemDataCreate(Defs::MenuActionStart, 0, 1, 4, 14)
};

PROGMEM const int16_t endracemenu[] = {
    Menu::itemDataCreate(Defs::MenuActionRestart, 0, 0,
                         Strings::Igm_Restart, 16),
    Menu::itemDataCreate(Defs::MenuActionBackToMM, 0, 0,
                         Strings::Igm_BackToMM, 16)
};

int16_t* getMenuData(uint8_t menu) {
    static int16_t* menus[] = { mainmenu, endracemenu };
    return menus[menu];
}
