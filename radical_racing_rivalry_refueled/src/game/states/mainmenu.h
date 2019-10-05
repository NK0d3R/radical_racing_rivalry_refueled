// Copyright 2019 Catalin G. Manciu

#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "splash.h"

class StateMainMenu : public StateSplash {
 private:
    virtual void stateInit();
    virtual void stateUpdate();
    virtual void stateRender(SpriteRenderer* renderer);
    void drawButtonHint(uint8_t x, uint8_t y, SpriteRenderer* renderer);
};

#endif  // MAINMENU_H_
