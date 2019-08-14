// Copyright 2019 Catalin G. Manciu

#ifndef INGAME_H_
#define INGAME_H_

#include "basegamestate.h"

class StateIngame : public BaseGameState {
 private:
    virtual void stateInit();
    virtual void stateUpdate();
    virtual void stateRender(SpriteRenderer* renderer);
};

#endif  // INGAME_H_
