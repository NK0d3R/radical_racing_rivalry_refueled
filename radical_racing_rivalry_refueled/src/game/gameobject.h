// Copyright 2018 Catalin G. Manciu

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "../defs.h"

class Level;
class SpriteRenderer;

class GameObject {
 protected:
    // World coordinates
    FP32        xPos;
    FP32        zPos;
    // Screen coordinates
    int32_t     screenX;
    uint8_t     screenY;
    uint8_t     screenW;

    Level*      parent;
 public:
    GameObject(Level* p, uint8_t scrW):
        parent(p), xPos(0), zPos(0), screenW(scrW) {}
    void updateScreenX();
    bool isVisible();
    FP32& getX()            { return xPos; }
    virtual void update(int16_t dt) = 0;
    virtual void draw(SpriteRenderer* renderer) = 0;
    virtual void onRaceStart() {}
    virtual void onRaceEnd() {}
};

#endif  // GAMEOBJECT_H_
