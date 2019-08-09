// Copyright 2018 Catalin G. Manciu

#include "renderer.h"

uint8_t SpriteRenderer::lineBuffer[LINE_BUFFER_CAPACITY] = {0};

void SpriteRenderer::initialize(uint16_t* fb, uint8_t fs) {
    frameBuffer = fb;
    frameStride = fs;
}

void SpriteRenderer::setClip(int16_t x, int16_t y, int16_t w, int16_t h) {
    clip.set(x, y, w, h);
}

void SpriteRenderer::putPixel(uint8_t x, uint8_t y) {
    uint16_t destRowByte = x + (y >> 3) * frameStride;
    uint8_t destBit = (y & 7);
    frameBuffer[destRowByte] |= (1 << destBit);
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

    static uint8_t modes[] = {1, 2, 4, 8};

    uint8_t bpp = modes[bppMode];
    int16_t currentOffset = dataBounds.x + (dataBounds.y * frameStride);
    int16_t lineOffsetIncr = frameStride;
    uint8_t currentLine = 0;

    if (yFlipped) {
        currentOffset += (dataBounds.h - 1) * frameStride;
        lineOffsetIncr = -frameStride;
    }

    if (xFlipped) {
        currentOffset += (dataBounds.w - 1);
    }

    for (uint8_t currentLine = 0; currentLine < dataBounds.h; ++currentLine) {
        transferLineAlpha(bpp, spriteData, paletteData,
                          static_cast<uint8_t>(srcX),
                          static_cast<uint8_t>(srcY + currentLine),
                          static_cast<uint8_t>(dataBounds.w),
                          width,
                          currentOffset, (xFlipped ? -1 : 1));
        currentOffset += lineOffsetIncr;
    }
}

void SpriteRenderer::transferLineAlpha(
                            uint8_t bpp,
                            const uint8_t* spriteData,
                            const uint16_t* paletteData,
                            uint8_t srcX, uint8_t srcY, uint8_t width,
                            uint8_t baseWidth, int16_t offset,
                            int16_t offsetIncr) {
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

void SpriteRenderer::drawLine(int16_t xStart, int16_t yStart,
                              int16_t xEnd, int16_t yEnd) {
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
            putPixel(x, y);
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
            putPixel(x, y);
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
