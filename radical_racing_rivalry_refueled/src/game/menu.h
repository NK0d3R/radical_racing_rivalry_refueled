// Copyright 2018 Catalin G. Manciu

#ifndef MENU_H_
#define MENU_H_

#include "../stdinc.h"
#include "../defs.h"

class SpriteRenderer;

/*
 * Menu Data bits: [high] h h h h h i i i i i f f o o o a a a [low]
 * a - action ID
 * o - number of selectable sub-options
 * f - sprite flag (if 1, item is sprite, if 0, item is text)
 * i - frame idx if sprite flag is 1, string idx if sprite flag is 0
 * h - height
 */

class Menu {
 public:
     enum : uint8_t {
         FlagSprite = 1 << 0,
         FlagAltSprite = 1 << 1
     };
    void set(const uint32_t* menu, uint8_t nbi, uint8_t w, uint8_t anim,
             uint8_t altSprite = 0, uint8_t altAnim = 0) {
        menuData = menu;
        nbItems = nbi;
        width = w;
        animation = anim;
        crtSelection = 0;
        optionStatus = 0;
        action = 0;
        alternateSprite = altSprite;
        alternateAnim = altAnim;
    }
    void draw(SpriteRenderer* renderer, uint8_t x, uint8_t y);
    uint8_t getItemOption(uint8_t item) {
        return ((optionStatus >> (item * optionBits)) & optionMask);
    }
    void setItemOption(uint8_t item, uint8_t option) {
        optionStatus &= ~(optionMask << (item * optionBits));
        optionStatus |=  (option << (item * optionBits));
    }
    void restart();
    uint8_t getAction() { return action; }
    static constexpr uint32_t itemDataCreate(uint32_t actionID,
                                             uint32_t optionCount,
                                             uint32_t spriteFlag,
                                             uint32_t visualID,
                                             uint32_t height) {
        return (actionID & 7) | ((optionCount & 7) << 3) |
               (spriteFlag & 3) << 6 | (visualID & 31) << 8 |
               ((height & 31) << 13);
    }
    void updateControls(uint8_t buttonsState, uint8_t oldButtonsState);

 private:
    constexpr static uint8_t optionBits = 3;
    constexpr static uint8_t optionMask = (1 << optionBits) - 1;
    void drawSpriteElementBackground(SpriteRenderer* renderer,
                                     int16_t x, int16_t y, int8_t height,
                                     bool hasArrows);
    const uint32_t*   menuData;
    uint16_t    optionStatus;
    uint8_t     nbItems;
    uint8_t     crtSelection;
    uint8_t     action;
    uint8_t     animation;
    uint8_t     alternateSprite;
    uint8_t     alternateAnim;
    int8_t      width;

    uint8_t itemHeight(const uint32_t& itemData) {
        return ((itemData >> 13) & 31);
    }
    uint8_t itemVisualID(const uint32_t& itemData) {
        return ((itemData >> 8) & 31);
    }
    uint8_t itemSpriteFlag(const uint32_t& itemData) {
        return ((itemData >> 6) & 3);
    }
    uint8_t itemOptionCount(const uint32_t& itemData) {
        return ((itemData >> 3) & 7);
    }
    uint8_t itemActionID(const uint32_t& itemData) {
        return (itemData & 7);
    }
    void onUp();
    void onDown();
    void onLeft();
    void onRight();
    void onConfirm();
    void changeCrtItemOption(bool left);
};

const uint32_t* getMenuData(uint8_t menu);

#endif  // MENU_H_
