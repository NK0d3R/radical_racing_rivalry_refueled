// Copyright 2019 Catalin G. Manciu

#ifndef SPLASH_H_
#define SPLASH_H_

#include "basegamestate.h"
#include "../../engine/fireeffect.h"
#include "../../defs.h"

class StateSplash : public BaseGameState {
 protected:
    static FireEffect fireEffect;
 private:
    static constexpr uint8_t LogoLetterFrames = 10;
    static constexpr uint8_t LogoLetterWaitFrames = 20;
    static constexpr uint8_t LogoLetterTotalFrames =
                                        LogoLetterWaitFrames +
                                        (LogoLetterFrames *
                                        (Defs::MenuLogoLastFrame + 1));

    virtual void stateInit();
    virtual void stateUpdate();
    virtual void stateRender(SpriteRenderer* renderer);
};

#endif  // SPLASH_H_
