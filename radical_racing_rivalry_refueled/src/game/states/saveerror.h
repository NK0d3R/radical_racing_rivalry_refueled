// Copyright 2019 Catalin G. Manciu

#ifndef SAVEERROR_H_
#define SAVEERROR_H_

#include "basegamestate.h"

class StateSaveError : public BaseGameState {
 private:
    virtual void stateInit();
    virtual void stateUpdate(int16_t dt);
    virtual void stateRender(SpriteRenderer* renderer);
};

#endif  // SAVEERROR_H_
