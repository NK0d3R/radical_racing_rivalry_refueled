// Copyright 2018 Catalin G. Manciu

#include "renderer.h"

uint8_t SpriteRenderer::lineBuffer[LINE_BUFFER_CAPACITY] = {0};

void SpriteRenderer::initialize(uint16_t* fb, uint8_t fw, uint8_t fh) {
    frameBuffer = fb;
    frameWidth = fw;
    frameHeight = fh;
}

void SpriteRenderer::setClip(int16_t x, int16_t y, int16_t w, int16_t h) {
    clip.set(x, y, w, h);
}

void SpriteRenderer::putPixel(uint8_t x, uint8_t y, uint16_t color) {
    frameBuffer[y * frameWidth + x] = color;
}

void SpriteRenderer::fastDrawVerticalPattern(uint8_t pattern,
                                             uint8_t x, uint8_t y) {
}

uint8_t bppModes[] = { 1, 2, 4, 8 };

#if ENABLE_NOALPHA_SUPPORT
#define GET_TRANSF_FUNC(bpp8, alpha, add)                                  \
                            ((bpp8) ? ((alpha) ?                           \
                             &SpriteRenderer::transferLine8<true, add> :   \
                             &SpriteRenderer::transferLine8<false, add>) : \
                            ((alpha) ?                                     \
                             &SpriteRenderer::transferLine<true, add> :    \
                             &SpriteRenderer::transferLine<false, add>))
#else
#define GET_TRANSF_FUNC(bpp8, alpha, add)                                  \
                            ((bpp8) ?                                      \
                             &SpriteRenderer::transferLine8<true, add> :   \
                             &SpriteRenderer::transferLine<true, add>)
#endif

void SpriteRenderer::drawSpriteData(const uint8_t* spriteData,
                                    const uint16_t* paletteData,
                                    int16_t targetX, int16_t targetY,
                                    uint8_t width, uint8_t height,
                                    uint8_t flags) {
    Rect dataBounds(targetX, targetY, width, height);

    dataBounds.clip(clip);

    if (dataBounds.isEmpty()) {
        return;
    }

    bool xFlipped = (flags & ARD_FLAGS_FLIP_X);
    bool yFlipped = (flags & ARD_FLAGS_FLIP_Y);
    bool additiveBlend = (flags & ARD_FLAGS_ADD);

    int16_t srcX = dataBounds.x - targetX;
    int16_t srcY = dataBounds.y - targetY;

    if (srcX < 0 || srcY < 0) {
        return;
    }

    uint8_t bppMode = getBPPFromElementFlags(flags);
    uint8_t hasAlpha = getHasAlphaFromElementFlags(flags);

    int16_t currentOffset = dataBounds.x + (dataBounds.y * frameWidth);
    int16_t lineOffsetIncr = frameWidth;

    if (yFlipped) {
        currentOffset += (dataBounds.h - 1) * frameWidth;
        lineOffsetIncr = -frameWidth;
        srcY = height - (srcY + dataBounds.h);
    }

    if (xFlipped) {
        currentOffset += (dataBounds.w - 1);
        srcX = width - (srcX + dataBounds.w);
    }
    auto func = additiveBlend ?
                GET_TRANSF_FUNC(bppMode == 3, hasAlpha == true, true):
                GET_TRANSF_FUNC(bppMode == 3, hasAlpha == true, false);
    uint8_t bpp = bppModes[bppMode];
    for (uint8_t currentLine = 0; currentLine < dataBounds.h;
            ++currentLine) {
        (this->*func)(bpp, spriteData, paletteData,
                      static_cast<uint8_t>(srcX),
                      static_cast<uint8_t>(srcY + currentLine),
                      static_cast<uint8_t>(dataBounds.w),
                      width,
                      currentOffset, (xFlipped ? -1 : 1), true);
        currentOffset += lineOffsetIncr;
    }
}

void SpriteRenderer::fillSingleLine(const uint8_t* spriteData,
                                    const uint16_t* paletteData,
                                    uint8_t targetX, uint8_t targetY,
                                    uint8_t srcY,
                                    uint8_t width, uint8_t flags) {
    uint8_t bppMode = getBPPFromElementFlags(flags);
    uint8_t bpp = bppModes[bppMode];

    auto func = GET_TRANSF_FUNC(bppMode == 3, false, false);

    int16_t currentOffset = clip.x + (targetY * frameWidth);
    uint8_t leftWidth = targetX - clip.x + 1;
    uint8_t rightWidth = clip.w - leftWidth;
    uint8_t leftPartial = leftWidth % width;
    uint8_t rightPartial = rightWidth % width;
    uint8_t totalIters = (leftWidth / width) + (rightWidth / width);

    if (leftPartial != 0) {
        (this->*func)(bpp, spriteData, paletteData,
                      width - leftPartial, srcY, leftPartial, width,
                      currentOffset, 1, true);
        currentOffset += leftPartial;
    }

    int16_t moduleStride = (width * bpp + 7) / 8;
    int16_t crtOffset = (srcY * moduleStride);
    memcpy_P(lineBuffer, spriteData + crtOffset, moduleStride);

    for (uint8_t iter = 0; iter < totalIters; ++iter) {
        (this->*func)(bpp, spriteData, paletteData,
                      0, srcY, width, width,
                      currentOffset, 1, false);
        currentOffset += width;
    }

    if (rightPartial != 0) {
        (this->*func)(bpp, spriteData, paletteData,
                      0, srcY, rightPartial, width,
                      currentOffset, 1, true);
    }
}

template<bool alpha, bool add>
void SpriteRenderer::transferLine(uint8_t bpp,
                                  const uint8_t* spriteData,
                                  const uint16_t* paletteData,
                                  uint8_t srcX, uint8_t srcY,
                                  uint8_t width, uint8_t baseWidth,
                                  int16_t offset, int16_t offsetIncr,
                                  bool refreshLineBuff) {
    uint16_t mask = (1 << static_cast<uint16_t>(bpp)) - 1;
    uint8_t m = mask;
    int16_t moduleStride = (baseWidth * bpp + 7) / 8;
    int16_t crtOffset = (srcY * moduleStride);
    uint8_t crtBit = (srcX * bpp) % 8;
    uint8_t* buffer = lineBuffer;
    if (refreshLineBuff) {
        memcpy_P(buffer, spriteData + crtOffset, moduleStride);
    }
    buffer += (srcX * bpp) / 8;

    for (uint8_t currentPix = 0; currentPix < width; ++currentPix) {
        uint8_t pix = ((buffer[0] >> crtBit) & m);
        if (!alpha || pix != 0) {
            if (!add) {
                frameBuffer[offset] = paletteData[pix];
            } else {
                frameBuffer[offset] = addBlendAdd(frameBuffer[offset],
                                                  paletteData[pix]);
            }
        }
        offset += offsetIncr;
        crtBit += bpp;
        if (crtBit > 7) {
            buffer++;
            crtBit = 0;
        }
    }
}

template<bool alpha, bool add>
void SpriteRenderer::transferLine8(uint8_t bpp,
                                   const uint8_t* spriteData,
                                   const uint16_t* paletteData,
                                   uint8_t srcX, uint8_t srcY,
                                   uint8_t width, uint8_t baseWidth,
                                   int16_t offset, int16_t offsetIncr,
                                   bool refreshLineBuff) {
    int16_t crtOffset = (srcY * baseWidth) + srcX;
    uint8_t* buffer = lineBuffer;
    if (refreshLineBuff) {
        memcpy_P(buffer, spriteData + crtOffset, baseWidth - srcX);
    }
    for (uint8_t currentPix = 0; currentPix < width; ++currentPix) {
        uint8_t pix = buffer[0];
        if (!alpha || pix != 0) {
            if (!add) {
                frameBuffer[offset] = paletteData[pix];
            } else {
                frameBuffer[offset] = addBlendAdd(frameBuffer[offset],
                                                  paletteData[pix]);
            }
        }
        offset += offsetIncr;
        buffer++;
    }
}

void SpriteRenderer::drawLine(int16_t xStart, int16_t yStart,
                              int16_t xEnd, int16_t yEnd, uint16_t color) {
    int16_t xDiff = xEnd - xStart;
    int16_t yDiff = yEnd - yStart;
    int16_t absX = abs(xDiff);
    int16_t absY = abs(yDiff);
    int16_t incrementX = Utils::sgnz(xDiff);
    int16_t incrementY = Utils::sgnz(yDiff);

    if (absX > absY) {
        int16_t d = (absY << 1) - absX;
        int16_t y = yStart;
        for (int16_t x = xStart;; x += incrementX) {
            putPixel(x, y, color);
            if (x == xEnd) {
                return;
            }
            if (d > 0) {
                y += incrementY;
                d -= (absX << 1);
            }
            d += (absY << 1);
        }
    } else {
        int16_t d = (absX << 1) - absY;
        int16_t x = xStart;
        for (int16_t y = yStart;; y += incrementY) {
            putPixel(x, y, color);
            if (y == yEnd) {
                return;
            }
            if (d > 0) {
                x += incrementX;
                d -= (absY << 1);
            }
            d += (absX << 1);
        }
    }
}

void SpriteRenderer::renderFireEffect(const uint8_t* data,
                                      const uint16_t* palette,
                                      uint8_t width, uint8_t height,
                                      uint8_t scale, uint8_t y) {
    uint16_t sourceOffset = 0;
    uint16_t destOffset = y * frameWidth;
    for (uint8_t line = 0; line < height; ++line) {
        for (uint8_t col = 0; col < width; ++col) {
            uint16_t color = palette[data[sourceOffset]];
            uint16_t crtDestOffset = destOffset;
            for (uint8_t destLine = 0; destLine < scale; ++destLine) {
                for (uint8_t destCol = 0; destCol < scale; ++destCol) {
                    frameBuffer[crtDestOffset + destCol] = color;
                }
                crtDestOffset += frameWidth;
            }
            sourceOffset += 1;
            destOffset += scale;
        }
        destOffset += frameWidth * (scale - 1);
    }
}

void SpriteRenderer::reasonablyFastBlur() {
    uint16_t offset = frameWidth + 1;
    uint16_t value;
    for (uint8_t line = 0; line < frameHeight - 2; ++line) {
        for (uint8_t col = 0; col < frameWidth - 2; ++col) {
            uint16_t totalR = 0;
            uint16_t totalG = 0;
            uint16_t totalB = 0;

    // 1 0 1
    // 1 2 1
    // 1 0 1
    #define ADD_FROM_OFFSET(offset, shift)              \
        value = frameBuffer[offset];                    \
        totalR += (Utils::getRLow(value) << shift);     \
        totalG += (Utils::getGLow(value) << shift);     \
        totalB += (Utils::getBLow(value) << shift);

        ADD_FROM_OFFSET(offset - 1, 0);
        ADD_FROM_OFFSET(offset, 1);
        ADD_FROM_OFFSET(offset + 1, 0);
        ADD_FROM_OFFSET(offset - frameWidth - 1, 0);
        ADD_FROM_OFFSET(offset - frameWidth + 1, 0);
        ADD_FROM_OFFSET(offset + frameWidth - 1, 0);
        ADD_FROM_OFFSET(offset + frameWidth + 1, 0);
        frameBuffer[offset++] = Utils::make16BitColorLow(totalR >> 3,
                                                         totalG >> 3,
                                                         totalB >> 3);
    #undef ADD_FROM_OFFSET
        }
        offset += 2;
    }
}

void SpriteRenderer::drawVerticalGradient(int8_t topR, int8_t topG,
                                          int8_t topB, int8_t botR,
                                          int8_t botG, int8_t botB) {
    FP32 crtR(topR);
    FP32 crtG(topG);
    FP32 crtB(topB);
    FP32 incrR(botR - topR);
    FP32 incrG(botG - topG);
    FP32 incrB(botB - topB);
    FP32 totalH(clip.h);
    incrR /= totalH;
    incrG /= totalH;
    incrB /= totalH;
    int16_t currentOffset = clip.x + clip.y * frameWidth;
    for (int16_t line = 0; line < clip.h; ++line) {
        uint16_t color = Utils::make16BitColorLow(crtR.getInt(),
                                                  crtG.getInt(),
                                                  crtB.getInt());
        int16_t currentPix = currentOffset;
        for (int16_t col = 0; col < clip.w; ++col) {
            frameBuffer[currentPix++] = color;
        }
        crtR += incrR;
        crtG += incrG;
        crtB += incrB;
        currentOffset += frameWidth;
    }
}
