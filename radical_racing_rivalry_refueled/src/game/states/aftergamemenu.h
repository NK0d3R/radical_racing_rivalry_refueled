// Copyright 2019 Catalin G. Manciu

#ifndef AFTERGAMEMENU_H_
#define AFTERGAMEMENU_H_

#include "basegamestate.h"

class StateAfterGameMenu : public BaseGameState {
 private:
    virtual void stateInit();
    virtual void stateUpdate(int16_t dt);
    virtual void stateRender(SpriteRenderer* renderer);
};

#endif  // AFTERGAMEMENU_H_
