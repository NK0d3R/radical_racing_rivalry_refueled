// Copyright 2019 Catalin G. Manciu

#ifndef LOGO_H_
#define LOGO_H_

#include "basegamestate.h"

class SpriteRenderer;

class StateLogo : public BaseGameState {
 private:
    virtual void stateInit();
    virtual void stateUpdate();
    virtual void stateRender(SpriteRenderer* renderer);
};

#endif  // LOGO_H_
