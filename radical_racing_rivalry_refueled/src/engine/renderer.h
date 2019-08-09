// Copyright 2018 Catalin G. Manciu

#ifndef RENDERER_H_
#define RENDERER_H_

#include "../stdinc.h"
#include "../defs.h"
#include "ardusprite.h"

class SpriteRenderer {
 private:
    static uint8_t lineBuffer[LINE_BUFFER_CAPACITY];

    Rect clip;
    uint16_t* frameBuffer;
    uint8_t frameStride;

 public:
    void initialize(uint16_t* fb, uint8_t fs);
    void setClip(int16_t x, int16_t y, int16_t w, int16_t h);
    inline Rect& getClip() { return clip; }
    void drawLine(int16_t xStart, int16_t yStart,
                  int16_t xEnd, int16_t yEnd);
    void putPixel(uint8_t x, uint8_t y);
    void drawSpriteData(const uint8_t* spriteData, const uint16_t* paletteData,
                        int16_t targetX, int16_t targetY, uint8_t width,
                        uint8_t height, uint8_t flags);
    void transferLineAlpha(uint8_t bpp,
                           const uint8_t* spriteData,
                           const uint16_t* paletteData,
                           uint8_t srcX, uint8_t srcY, uint8_t width,
                           uint8_t baseWidth, int16_t offset,
                           int16_t offsetIncr);

    void fastDrawVerticalPattern(uint8_t pattern, uint8_t x, uint8_t y);
    void updatePixelBatch(uint8_t* pix, uint8_t data, uint8_t mask) {
        *pix = ((data & mask) | (*pix & ~mask));
    }
};
#endif  // RENDERER_H_
