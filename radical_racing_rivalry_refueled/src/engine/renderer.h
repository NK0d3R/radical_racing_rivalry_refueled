// Copyright 2018 Catalin G. Manciu

#ifndef RENDERER_H_
#define RENDERER_H_

#include "../stdinc.h"
#include "../defs.h"
#include "ardusprite.h"

#define ENABLE_NOALPHA_SUPPORT  (0)

class SpriteRenderer {
 private:
    static uint8_t lineBuffer[LINE_BUFFER_CAPACITY];
    Rect clip;
    uint16_t* frameBuffer;
    uint8_t frameWidth;
    uint8_t frameHeight;

 public:
    void initialize(uint16_t* fb, uint8_t fw, uint8_t fh);
    void setClip(int16_t x, int16_t y, int16_t w, int16_t h);
    inline Rect& getClip() { return clip; }
    void drawLine(int16_t xStart, int16_t yStart,
                  int16_t xEnd, int16_t yEnd, uint16_t color);
    void putPixel(uint8_t x, uint8_t y, uint16_t color);
    void drawSpriteData(const uint8_t* spriteData, const uint16_t* paletteData,
                        int16_t targetX, int16_t targetY, uint8_t width,
                        uint8_t height, uint8_t flags);
    void fillSingleLine(const uint8_t* spriteData,
                        const uint16_t* paletteData,
                        uint8_t targetX, uint8_t targetY,
                        uint8_t srcY,
                        uint8_t width, uint8_t flags);
    template<bool alpha, bool add>
    void transferLine(uint8_t bpp,
                      const uint8_t* spriteData,
                      const uint16_t* paletteData,
                      uint8_t srcX, uint8_t srcY, uint8_t width,
                      uint8_t baseWidth, int16_t offset,
                      int16_t offsetIncr, bool refreshLineBuff);
    template<bool alpha, bool add>
    void transferLine8(uint8_t bpp,
                       const uint8_t* spriteData,
                       const uint16_t* paletteData,
                       uint8_t srcX, uint8_t srcY, uint8_t width,
                       uint8_t baseWidth, int16_t offset,
                       int16_t offsetIncr, bool refreshLineBuff);
    void renderFireEffect(const uint8_t* data, const uint16_t* palette,
                          uint8_t width, uint8_t height, uint8_t scale,
                          uint8_t y);
    void fastDrawVerticalPattern(uint8_t pattern, uint8_t x, uint8_t y,
                                 uint16_t colorZero, uint16_t colorOne,
                                 uint8_t repeat = 1);
    void reasonablyFastBlur();
    uint16_t addBlendAdd(uint16_t one, uint16_t two) {
        uint8_t totalR = Utils::upperClamp(
                                Utils::getRLow(one) + Utils::getRLow(two), 31);
        uint8_t totalG = Utils::upperClamp(
                                Utils::getGLow(one) + Utils::getGLow(two), 63);
        uint8_t totalB = Utils::upperClamp(
                                Utils::getBLow(one) + Utils::getBLow(two), 31);
        return Utils::make16BitColorLow(totalR, totalG, totalB);
    }
    void drawVerticalGradient(int8_t topR, int8_t topG, int8_t topB,
                              int8_t botR, int8_t botG, int8_t botB);
};
#endif  // RENDERER_H_
