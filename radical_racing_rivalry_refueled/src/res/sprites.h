// Copyright 2018 Catalin G. Manciu

#ifndef SPRITES_H_
#define SPRITES_H_

#include "../stdinc.h"
#include "../engine/ardusprite.h"
#include "../defs.h"

struct Sprite;
struct Font;

inline Sprite* GetSprite(uint8_t idx) {
    static Sprite sprites[Defs::SpritesNb];
    return &sprites[idx];
}

inline Font* GetFont(uint8_t idx) {
    static Font fonts[Defs::FontsNb];
    return &fonts[idx];
}

#endif  // SPRITES_H_

