// Copyright 2018 Catalin G. Manciu

#include "menu.h"
#include "../res/stringmgr.h"
#include "../res/sprites.h"
#include "../engine/ardusprite.h"
#include "../engine/renderer.h"
#include "../game.h"

void Menu::drawSpriteElementBackground(SpriteRenderer* renderer,
                                       int16_t x, int16_t y, int8_t height,
                                       bool hasArrows) {
    Sprite* spr = GetSprite(Defs::SpriteMenu);
    static int16_t bgTileW = GET_MAXX_FROM_PACK(
                                spr->measureAnimationFrame<true>(
                                                    Defs::AnimMenuElements,
                                                    Defs::MenuItemBg)) + 1;
    renderer->setClip(x - (width >> 1), y - (height >> 1), width, height);
    int16_t maxX = renderer->getClip().maxX;
    uint32_t frame = R3R::getInstance().getFrameCounter();
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
        uint32_t itemData = pgm_read_dword(&menuData[idx]);
        uint8_t height = itemHeight(itemData);
        uint8_t visualID = itemVisualID(itemData);
        uint8_t itemOptionCnt = itemOptionCount(itemData);
        uint8_t flags = itemSpriteFlag(itemData);
        if (itemOptionCnt) {
            visualID += getItemOption(idx);
        }
        y += (height >> 1);
        if (crtSelection == idx) {
            drawSpriteElementBackground(renderer, x, y, height,
                                        itemOptionCnt > 0);
        }
        if (flags & FlagSprite) {
            uint8_t sprite = Defs::SpriteMenu;
            uint8_t anim = animation;
            if (flags & FlagAltSprite) {
                sprite = alternateSprite;
                anim = alternateAnim;
            }
            Sprite* spr = GetSprite(sprite);
            spr->drawAnimationFrame(renderer, anim, visualID, x,
                                    y, 0);
        } else {
            Font* font = GetFont(Defs::FontMain);
            font->drawString(renderer,
                             getString(static_cast<Strings>(visualID)), x, y,
                             ANCHOR_HCENTER | ANCHOR_VCENTER);
        }
        y += (height >> 1);
    }
}

void Menu::updateControls(uint8_t buttonsState, uint8_t oldButtonsState) {
    uint8_t changedButtons = (buttonsState ^ oldButtonsState);
    if ((changedButtons & buttonsState & DPAD_UP)) {
        onUp();
    }
    if ((changedButtons & buttonsState & DPAD_DOWN)) {
        onDown();
    }
    if ((changedButtons & buttonsState & DPAD_LEFT)) {
        onLeft();
    }
    if ((changedButtons & buttonsState & DPAD_RIGHT)) {
        onRight();
    }
    if ((changedButtons & buttonsState & BTN_A)) {
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

PROGMEM const uint32_t mainmenu[] = {
    Menu::itemDataCreate(0, 1, Menu::FlagSprite, 0, 20),
    Menu::itemDataCreate(0, 1, Menu::FlagSprite, 2, 18),
    Menu::itemDataCreate(0, Defs::CarNbChassis - 1,
                         Menu::FlagSprite | Menu::FlagAltSprite, 0, 18)
};

PROGMEM const uint32_t endracemenu[] = {
    Menu::itemDataCreate(Defs::MenuActionRestart, 0, 0,
                         Strings::Igm_Restart, 16),
    Menu::itemDataCreate(Defs::MenuActionBackToMM, 0, 0,
                         Strings::Igm_BackToMM, 16)
};

const uint32_t* getMenuData(uint8_t menu) {
    static const uint32_t* menus[] = { mainmenu, endracemenu };
    return menus[menu];
}
