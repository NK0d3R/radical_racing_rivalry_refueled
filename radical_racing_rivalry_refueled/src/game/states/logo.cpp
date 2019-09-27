// Copyright 2019 Catalin G. Manciu

#include "logo.h"
#include "../../game.h"
#include "../../res/stringmgr.h"
#include "../../res/strings.h"
#include "../../res/sprites.h"

void StateLogo::stateInit() {
}

void StateLogo::stateUpdate() {
    R3R& g = R3R::getInstance();
    if (((g.oldButtonsState ^ g.buttonsState) & g.buttonsState &&
        stateFrameCounter > 75) || stateFrameCounter > 120) {
        g.setState(Splash);
    }
}

void StateLogo::stateRender(SpriteRenderer* renderer) {
    renderer->drawVerticalGradient(24, 0, 24,
                                   0, 16, 0);
    GetSprite(Defs::SpriteLogo)->drawAnimationFrame(renderer, 0, 0, 0, 0, 0);
    if (stateFrameCounter > 75) {
        renderer->reasonablyFastBlur();
        GetFont(Defs::FontMain)->drawString(renderer,
                                            getString(Strings::Logo_1),
                                            Defs::ScreenW / 2,
                                            Defs::ScreenH / 2,
                                            ANCHOR_HCENTER | ANCHOR_VCENTER);
    }
}
