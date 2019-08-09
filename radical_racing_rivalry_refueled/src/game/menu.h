// Copyright 2018 Catalin G. Manciu

#ifndef MENU_H_
#define MENU_H_

#include "../stdinc.h"
#include "../defs.h"

class SpriteRenderer;

class Menu {
 public:
    void set(uint16_t* menu, uint8_t nbi, uint8_t w, uint8_t anim) {
        menuData = menu;
        nbItems = nbi;
        width = w;
        animation = anim;
        crtSelection = 0;
        optionStatus = 0;
        action = 0;
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
    static constexpr uint16_t itemDataCreate(uint16_t actionID,
                                             uint16_t optionCount,
                                             uint16_t spriteFlag,
                                             uint16_t visualID,
                                             uint16_t height) {
        return (actionID & 7) | ((optionCount & 3) << 3) |
               (spriteFlag & 1) << 5 | (visualID & 15) << 6 |
               ((height & 31) << 10);
    }
    void updateControls(uint8_t buttonsState, uint8_t oldButtonsState);

 private:
    constexpr static uint8_t optionBits = 2;
    constexpr static uint8_t optionMask = (1 << optionBits) - 1;
    void drawSpriteElementBackground(SpriteRenderer* renderer,
                                     int16_t x, int16_t y, int8_t height,
                                     bool hasArrows);
    uint16_t*   menuData;
    uint16_t    optionStatus;
    uint8_t     nbItems;
    uint8_t     crtSelection;
    uint8_t     action;
    uint8_t     animation;
    int8_t      width;

    uint8_t itemHeight(const int16_t& itemData) {
        return ((itemData >> 10) & 31);
    }
    uint8_t itemVisualID(const int16_t& itemData) {
        return ((itemData >> 6) & 15);
    }
    uint8_t itemSpriteFlag(const int16_t& itemData) {
        return ((itemData >> 5) & 1);
    }
    uint8_t itemOptionCount(const int16_t& itemData) {
        return ((itemData >> 3) & 3);
    }
    uint8_t itemActionID(const int16_t& itemData) {
        return (itemData & 7);
    }
    void onUp();
    void onDown();
    void onLeft();
    void onRight();
    void onConfirm();
    void changeCrtItemOption(bool left);
};

int16_t* getMenuData(uint8_t menu);

#endif  // MENU_H_
