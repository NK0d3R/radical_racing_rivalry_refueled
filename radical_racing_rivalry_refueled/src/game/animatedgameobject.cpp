// Copyright 2019 Catalin G. Manciu

#include "animatedgameobject.h"
#include "level.h"
#include "../res/sprites.h"

AnimatedGameObject::AnimatedGameObject(Level* p,
                                       int8_t scrMinX, int8_t scrMaxX,
                                       uint8_t spr, uint8_t anim):
                                       GameObject(p, scrMinX, scrMaxX) {
    animator.init(GetSprite(spr));
    animator.setAnimation(anim, 0, true);
}

void AnimatedGameObject::update(int16_t dt) {
    animator.update(dt);
}

void AnimatedGameObject::draw(SpriteRenderer* renderer) {
    animator.draw(renderer, screenX, screenY);
}

void AnimatedGameObject::reset(const FP32& x, const FP32& z) {
    GameObject::reset(x, z);
    animator.setRandomFrame();
}
