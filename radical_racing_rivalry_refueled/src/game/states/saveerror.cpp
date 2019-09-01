// Copyright 2019 Catalin G. Manciu

#include "saveerror.h"
#include "../../game.h"
#include "../../res/stringmgr.h"
#include "../../res/strings.h"
#include "../../res/sprites.h"

void StateSaveError::stateInit() {
}

void StateSaveError::stateUpdate() {
    RRRR& g = RRRR::getInstance();
    if ((g.oldButtonsState ^ g.buttonsState) & g.buttonsState) {
        g.setState(Splash);
    }
}

void StateSaveError::stateRender(SpriteRenderer* renderer) {
    uint8_t y = 10;
    for (uint8_t str = No_Save1; str <= No_Save2; ++str) {
        GetFont(Defs::FontMain)->drawString(renderer, getString(
                                            static_cast<Strings>(str)),
                                            Defs::ScreenW / 2, y,
                                            ANCHOR_HCENTER | ANCHOR_TOP);
        y += 40;
    }
}
