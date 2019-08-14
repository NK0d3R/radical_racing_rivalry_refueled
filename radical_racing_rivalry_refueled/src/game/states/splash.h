// Copyright 2019 Catalin G. Manciu

#ifndef SPLASH_H_
#define SPLASH_H_

#include "basegamestate.h"
#include "../../engine/fireeffect.h"

class StateSplash : public BaseGameState {
 protected:
    static FireEffect fireEffect;
 private:
    virtual void stateInit();
    virtual void stateUpdate();
    virtual void stateRender(SpriteRenderer* renderer);
};

#endif  // SPLASH_H_
