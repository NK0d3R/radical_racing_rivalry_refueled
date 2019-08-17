// Copyright 2019 Catalin G. Manciu

#ifndef FIREEFFECT_H_
#define FIREEFFECT_H_

#include "../stdinc.h"

class SpriteRenderer;

class FireEffect {
 private:
    uint8_t* buffer;
    uint16_t* palette;
    uint8_t  paletteSize;
    uint8_t  width;
    uint8_t  height;
    uint8_t  buffScale;
    int8_t   tempOverlayTimer;
    uint8_t  tempX;
    uint8_t  tempY;
    uint8_t  tempW;
    uint8_t  tempH;
    const uint8_t baselineH = 1;
    void generateBaseline(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    void process(uint8_t x, uint8_t y, uint8_t w);
    void deleteBuffers();
 public:
    FireEffect() : buffer(nullptr), palette(nullptr) {}
    ~FireEffect();
    void initialize(uint8_t scale, const uint16_t* pal, uint8_t palSize);
    void update();
    void render(SpriteRenderer* renderer);
    void addTempOverlay(uint8_t x, uint8_t y,
                        uint8_t w, uint8_t h,
                        int8_t timer) {
                             tempX = x / buffScale;
                             tempY = y / buffScale;
                             tempW = w / buffScale;
                             tempH = h / buffScale;
                             tempOverlayTimer = timer;
                         }
};

#endif  // FIREEFFECT_H_
