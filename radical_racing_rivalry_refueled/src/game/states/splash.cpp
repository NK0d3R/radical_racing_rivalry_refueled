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
    uint8_t frame = stateFrameCounter < 90 ? (stateFrameCounter - 10) / 10 : 8;
    if (stateFrameCounter > 10) {
        GetSprite(Defs::SpriteMenu)->drawAnimationFrame(
                                        renderer, Defs::AnimLogo,
                                        frame, 10, 45, 0);
    }
}
