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
    R3R& g = R3R::getInstance();
    fireEffect.update();
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
        fireEffect.addTempOverlay(overlayX, 54, overlayW, 8);
    }
    if ((g.oldButtonsState ^ g.buttonsState) & g.buttonsState) {
        g.setState(MainMenu);
    }
}

void StateSplash::stateRender(SpriteRenderer* renderer) {
    R3R& g = R3R::getInstance();
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
