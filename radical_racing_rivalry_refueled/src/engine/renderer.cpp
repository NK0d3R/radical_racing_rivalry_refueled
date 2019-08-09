// Copyright 2018 Catalin G. Manciu

#include "renderer.h"

#if USE_RENDERER_LINE_BUFFER
uint8_t SpriteRenderer::lineBuffer[BUFFER_CAPACITY] = {0};
#endif

uint8_t SpriteRenderer::leftMask(uint8_t nbBits) {
    return (0xFF << nbBits);
}

uint8_t SpriteRenderer::rightMask(uint8_t nbBits) {
    return (0xFF >> nbBits);
}

uint8_t SpriteRenderer::bitReverse(uint8_t byte) {
    static const uint8_t reversedBits[] PROGMEM = {
        0x00, 0x08, 0x04, 0x0C, 0x02, 0x0A, 0x06, 0x0E,
        0x01, 0x09, 0x05, 0x0D, 0x03, 0x0B, 0x07, 0x0F
    };
    return (pgm_read_byte(&reversedBits[byte & 0xF]) << 4) |
            pgm_read_byte(&reversedBits[(byte & 0xF0) >> 4]);
}

void SpriteRenderer::initialize(uint8_t* fb, uint8_t fs) {
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
    uint16_t destRowByte = x + (y >> 3) * frameStride;
    uint8_t destBit = (y & 7);
    updatePixelBatch(&frameBuffer[destRowByte], (pattern << destBit),
                     leftMask(destBit));
    if (destBit != 0) {
        destRowByte += frameStride;
        destBit = 8 - destBit;
        updatePixelBatch(&frameBuffer[destRowByte], (pattern >> destBit),
                         rightMask(destBit));
    }
}

void SpriteRenderer::drawSpriteData(uint8_t* spriteData, int16_t targetX,
                                    int16_t targetY, uint8_t width,
                                    uint8_t height, uint8_t flags) {
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

    drawSpriteData1Bit(spriteData, srcX, srcY, dataBounds.x, dataBounds.y,
                       dataBounds.w, dataBounds.h, width, height, flags);
}

void SpriteRenderer::drawSpriteData1Bit(uint8_t* spriteData, uint8_t srcX,
                                        uint8_t srcY, uint8_t targetX,
                                        uint8_t targetY, int8_t width,
                                        int8_t height, uint8_t initialWidth,
                                        uint8_t initialHeight, uint8_t flags) {
    bool xFlipped = (flags & ARD_FLAGS_FLIP_X);
    bool yFlipped = (flags & ARD_FLAGS_FLIP_Y);
    bool isOpaque = (getOpacityBit(flags) != 0);

    uint16_t spriteDataStride = isOpaque ? initialWidth : (initialWidth << 1);
    uint16_t destRowStartByte = targetX + ((targetY >> 3) * frameStride);

    uint8_t destBit = (targetY & 7);

    uint8_t currentPixData[2];

    uint8_t srcBit;
    int16_t srcRowIncr;
    uint16_t destOffset;
    uint16_t srcRowStartByte;

    int8_t toShift;  // if toShift > 0: shift left, if < 0: shift right
    uint8_t pixWritten;

    uint8_t xOffset = 0;
    uint8_t xOffsetStart = 0;
    int8_t xOffsetInc = isOpaque ? 1 : 2;
    uint8_t xLeftShift = xOffsetInc - 1;
    int8_t colsToWrite;

    if (yFlipped == false) {
        srcRowStartByte = (srcY >> 3) * spriteDataStride;
        srcRowIncr = spriteDataStride;
        srcBit = (srcY & 7);
    } else {
        srcRowStartByte = (((initialHeight - srcY - 1) >> 3) *
                             spriteDataStride);
        srcRowIncr = -spriteDataStride;
        srcY = initialHeight - srcY - 1;
        srcBit = 7 - (srcY & 7);
    }

    if (xFlipped == false) {
        srcRowStartByte += (srcX << xLeftShift);
    } else {
        xOffsetStart = ((width - 1) << xLeftShift);
        xOffsetInc = -xOffsetInc;
        if (srcX == 0) {
            srcRowStartByte += ((initialWidth - width) << xLeftShift);
        }
    }

#if USE_RENDERER_LINE_BUFFER
    memcpy_P(lineBuffer, spriteData + srcRowStartByte, (width << xLeftShift));
#endif

    while (height > 0) {
        toShift = destBit - srcBit;
        if (srcBit < destBit) {
            pixWritten = (8 - destBit);
        } else {
            pixWritten = (8 - srcBit);
        }

        pixWritten = min(pixWritten, height);
        destOffset = destRowStartByte;

        for (xOffset = xOffsetStart, colsToWrite = width;
            colsToWrite > 0; --colsToWrite, xOffset += xOffsetInc) {
        #if USE_RENDERER_LINE_BUFFER
            if (isOpaque) {
                currentPixData[0] = lineBuffer[xOffset];
                currentPixData[1] = 0xFF;
            } else {
                currentPixData[0] = lineBuffer[xOffset];
                currentPixData[1] = lineBuffer[xOffset + 1];
            }
        #else
            if (isOpaque) {
                currentPixData[0] = pgm_read_byte(spriteData +
                                                  srcRowStartByte + xOffset);
                currentPixData[1] = 0xFF;
            } else {
                *reinterpret_cast<int16_t*>(currentPixData) =
                        pgm_read_word(spriteData + srcRowStartByte + xOffset);
            }
        #endif
            if (yFlipped != 0) {
                currentPixData[0] = bitReverse(currentPixData[0]);
                currentPixData[1] = bitReverse(currentPixData[1]);
            }
            currentPixData[1] &= leftMask(srcBit);
            if (srcBit + pixWritten < 8) {
                currentPixData[1] &= rightMask(8 - (srcBit + pixWritten));
            }
            if (toShift > 0) {
                currentPixData[0] <<= toShift;
                currentPixData[1] <<= toShift;
            } else if (toShift < 0) {
                uint8_t shiftAmount = -toShift;
                currentPixData[0] >>= shiftAmount;
                currentPixData[1] >>= shiftAmount;
            }
            updatePixelBatch(&frameBuffer[destOffset], currentPixData[0],
                             currentPixData[1]);
            destOffset++;
        }
        height -= pixWritten;
        srcBit += pixWritten;
        destBit += pixWritten;

        if (srcBit > 7) {
            srcBit = 0;
            srcRowStartByte += srcRowIncr;
        #if USE_RENDERER_LINE_BUFFER
            memcpy_P(lineBuffer, spriteData + srcRowStartByte,
                     (width << xLeftShift));
        #endif
        }
        if (destBit > 7) {
            destBit = 0;
            destRowStartByte += frameStride;
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
