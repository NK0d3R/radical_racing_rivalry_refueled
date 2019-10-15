// Copyright 2019 Catalin G. Manciu

#include "animatedgameobject.h"
#include "level.h"
#include "../res/sprites.h"

AnimatedGameObject::AnimatedGameObject(Level* p, uint8_t scrW,
                                       uint8_t spr, uint8_t anim):
                                       GameObject(p, scrW) {
    animator.init(GetSprite(spr));
    animator.setAnimation(anim, 0, true);
}

void AnimatedGameObject::update(int16_t dt) {
    animator.update(dt);
}

void AnimatedGameObject::draw(SpriteRenderer* renderer) {
    animator.draw(renderer, screenX, screenY);
}

void AnimatedGameObject::onRaceStart() {
    animator.setRandomFrame();
}
