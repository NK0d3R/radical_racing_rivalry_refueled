// Copyright 2018 Catalin G. Manciu

#ifndef SPRITES_H_
#define SPRITES_H_

#include "../stdinc.h"
#include "../engine/ardusprite.h"
#include "../defs.h"

class Sprite;
class Font;

inline Sprite* GetSprite(int8_t idx) {
    static Sprite sprites[Defs::SpritesNb];
    return &sprites[idx];
}

inline Font* GetFont(int8_t idx) {
    static Font fonts[Defs::FontsNb];
    return &fonts[idx];
}

#endif  // SPRITES_H_

