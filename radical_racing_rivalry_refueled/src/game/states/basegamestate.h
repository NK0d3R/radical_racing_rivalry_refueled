// Copyright 2019 Catalin G. Manciu

#ifndef BASEGAMESTATE_H_
#define BASEGAMESTATE_H_

#include "../../stdinc.h"

class SpriteRenderer;

class BaseGameState {
 protected:
    static uint32_t stateFrameCounter;
    virtual void stateInit() {}
    virtual void stateUpdate(int16_t dt) {}
    virtual void stateRender(SpriteRenderer* renderer) {}
 public:
    void initialize() {
        stateFrameCounter = 0;
        stateInit();
    }

    void update(int16_t dt) {
        stateUpdate(dt);
        stateFrameCounter++;
    }

    void render(SpriteRenderer* renderer) {
        stateRender(renderer);
    }
};

#endif  // BASEGAMESTATE_H_
