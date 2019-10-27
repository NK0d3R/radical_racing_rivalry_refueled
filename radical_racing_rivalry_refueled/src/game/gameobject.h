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
    int8_t      screenMinX;
    int8_t      screenMaxX;
    Level*      parent;
 public:
    GameObject(Level* p, int8_t scrMinX, int8_t scrMaxX):
        parent(p), screenMinX(scrMinX), screenMaxX(scrMaxX) {}
    void updateScreenX();
    bool isVisible();
    FP32& getX()            { return xPos; }
    virtual void reset(const FP32& x, const FP32& z);
    virtual void update(int16_t dt) = 0;
    virtual void draw(SpriteRenderer* renderer) = 0;
    virtual void onRaceStart() {}
    virtual void onRaceEnd(uint8_t endResult) {}
};

#endif  // GAMEOBJECT_H_
