// Copyright 2019 Catalin G. Manciu

#include "splash.h"
#include "../../game.h"
#include "../../res/firepal.h"
#include "../../res/sprites.h"

FireEffect StateSplash::fireEffect;

void StateSplash::stateInit() {
    static auto init = [&]() -> bool {
        fireEffect.initialize(2, FIREPAL, FIREPAL_SIZE);
        return true;
    } ();
}

void StateSplash::stateUpdate() {
    RRRR& g = RRRR::getInstance();

    if (stateFrameCounter >= LogoLetterWaitFrames + LogoLetterFrames) {
        uint8_t overlayX;
        uint8_t overlayW;
        if (stateFrameCounter < LogoLetterTotalFrames) {
            overlayX = 10 + 10 * ((stateFrameCounter -
                 (LogoLetterWaitFrames + LogoLetterFrames)) / LogoLetterFrames);
            overlayW = 10;
        } else {
            overlayX = 10;
            overlayW = 80;
        }
        fireEffect.addTempOverlay(overlayX, 52, overlayW, 4, 1);
    }
    fireEffect.update();
    if ((g.oldButtonsState ^ g.buttonsState) & g.buttonsState) {
        g.setState(MainMenu);
    }
}

void StateSplash::stateRender(SpriteRenderer* renderer) {
    RRRR& g = RRRR::getInstance();
    fireEffect.render(renderer);
    GetSprite(Defs::SpriteMenu)->drawAnimationFrame(
                                    renderer, Defs::AnimMenuElements,
                                    Defs::MenuSplash, 0, 0, 0);
    if (stateFrameCounter > LogoLetterWaitFrames) {
        uint8_t frame = (stateFrameCounter < LogoLetterTotalFrames) ?
                        (stateFrameCounter - LogoLetterWaitFrames) /
                        LogoLetterFrames : Defs::MenuLogoLastFrame;
        GetSprite(Defs::SpriteMenu)->drawAnimationFrame(
                                        renderer, Defs::AnimLogo,
                                        frame, 4, 50, 0);
    }
}
