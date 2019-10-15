// Copyright 2019 Catalin G. Manciu

#ifndef ANIMATEDGAMEOBJECT_H_
#define ANIMATEDGAMEOBJECT_H_

#include "../defs.h"
#include "gameobject.h"
#include "../engine/ardusprite.h"

class Level;
class SpriteRenderer;

class AnimatedGameObject : public GameObject {
 protected:
    SpriteAnimator animator;
 public:
    AnimatedGameObject(Level* p, uint8_t scrW, uint8_t spr, uint8_t anim);
    virtual void update(int16_t dt);
    virtual void draw(SpriteRenderer* renderer);
    virtual void onRaceStart();
    virtual void onRaceEnd() {}
};

#endif  // ANIMATEDGAMEOBJECT_H_
