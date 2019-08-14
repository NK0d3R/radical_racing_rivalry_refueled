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

    int16_t srcX = dataBounds.x - targetX;
    int16_t srcY = dataBounds.y - targetY;

    if (srcX < 0 || srcY < 0) {
        return;
    }

    bool xFlipped = (flags & ARD_FLAGS_FLIP_X);
    bool yFlipped = (flags & ARD_FLAGS_FLIP_Y);
    bool isOpaque = getNoAlphaFromElementFlags(flags);
    uint8_t bppMode = getBPPFromElementFlags(flags);

    int16_t currentOffset = dataBounds.x + (dataBounds.y * frameWidth);
    int16_t lineOffsetIncr = frameWidth;
    uint8_t currentLine = 0;

    if (yFlipped) {
        currentOffset += (dataBounds.h - 1) * frameWidth;
        lineOffsetIncr = -frameWidth;
    }

    if (xFlipped) {
        currentOffset += (dataBounds.w - 1);
    }

    if (bppMode == 3) {
        for (uint8_t currentLine = 0; currentLine < dataBounds.h;
             ++currentLine) {
            transferLine8Alpha(spriteData, paletteData,
                               static_cast<uint8_t>(srcX),
                               static_cast<uint8_t>(srcY + currentLine),
                               static_cast<uint8_t>(dataBounds.w),
                               width,
                               currentOffset, (xFlipped ? -1 : 1));
            currentOffset += lineOffsetIncr;
        }
    } else {
        static uint8_t modes[] = {1, 2, 4};
        uint8_t bpp = modes[bppMode];
        for (uint8_t currentLine = 0; currentLine < dataBounds.h;
             ++currentLine) {
            transferLineAlpha(bpp, spriteData, paletteData,
                              static_cast<uint8_t>(srcX),
                              static_cast<uint8_t>(srcY + currentLine),
                              static_cast<uint8_t>(dataBounds.w),
                              width,
                              currentOffset, (xFlipped ? -1 : 1));
            currentOffset += lineOffsetIncr;
        }
    }
}

void SpriteRenderer::transferLineAlpha(uint8_t bpp,
                                       const uint8_t* spriteData,
                                       const uint16_t* paletteData,
                                       uint8_t srcX, uint8_t srcY,
                                       uint8_t width, uint8_t baseWidth,
                                       int16_t offset, int16_t offsetIncr) {
    uint16_t mask = (1 << static_cast<uint16_t>(bpp)) - 1;
    uint8_t m = mask;
    int16_t moduleStride = (baseWidth * bpp + 7) / 8;
    int16_t crtOffset = (srcY * moduleStride);
    uint8_t crtBit = (srcX * bpp) % 8;
    uint8_t* buffer = lineBuffer;
    memcpy_P(buffer, spriteData + crtOffset, moduleStride);
    buffer += (srcX * bpp) / 8;

    for (uint8_t currentPix = 0; currentPix < width; ++currentPix) {
        uint8_t pix = ((buffer[0] >> crtBit) & m);
        if (pix != 0) {
            frameBuffer[offset] = paletteData[pix];
        }
        offset += offsetIncr;
        crtBit += bpp;
        if (crtBit > 7) {
            buffer++;
            crtBit = 0;
        }
    }
}

void SpriteRenderer::transferLine8Alpha(const uint8_t* spriteData,
                                        const uint16_t* paletteData,
                                        uint8_t srcX, uint8_t srcY,
                                        uint8_t width, uint8_t baseWidth,
                                        int16_t offset, int16_t offsetIncr) {
    int16_t crtOffset = (srcY * baseWidth) + srcX;
    uint8_t* buffer = lineBuffer;
    memcpy_P(buffer, spriteData + crtOffset, baseWidth - srcX);
    for (uint8_t currentPix = 0; currentPix < width; ++currentPix) {
        uint8_t pix = buffer[0];
        if (pix != 0) {
            frameBuffer[offset] = paletteData[pix];
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
