// Copyright 2018 Catalin G. Manciu

#ifndef RENDERER_H_
#define RENDERER_H_

#include "../stdinc.h"
#include "../defs.h"
#include "ardusprite.h"

class SpriteRenderer {
 private:
#if USE_RENDERER_LINE_BUFFER
    static uint8_t lineBuffer[BUFFER_CAPACITY];
#endif
    Rect clip;
    uint8_t* frameBuffer;
    uint8_t frameStride;

 public:
    void initialize(uint8_t* fb, uint8_t fs);
    void setClip(int16_t x, int16_t y, int16_t w, int16_t h);
    inline Rect& getClip() { return clip; }
    void drawLine(int16_t xStart, int16_t yStart,
                  int16_t xEnd, int16_t yEnd);
    void putPixel(uint8_t x, uint8_t y);
    void drawSpriteData(uint8_t* spriteData, int16_t targetX,
                        int16_t targetY, uint8_t width,
                        uint8_t height, uint8_t flags);
    void drawSpriteData1Bit(uint8_t* spriteData, uint8_t srcX,
                            uint8_t srcY, uint8_t targetX,
                            uint8_t targetY, int8_t width,
                            int8_t height, uint8_t initialWidth,
                            uint8_t initialHeight, uint8_t flags);
    void fastDrawVerticalPattern(uint8_t pattern, uint8_t x, uint8_t y);
    void updatePixelBatch(uint8_t* pix, uint8_t data, uint8_t mask) {
        *pix = ((data & mask) | (*pix & ~mask));
    }
    inline uint8_t bitReverse(uint8_t byte);
    inline uint8_t leftMask(uint8_t nbBits);
    inline uint8_t rightMask(uint8_t nbBits);
};

#endif  // RENDERER_H_
