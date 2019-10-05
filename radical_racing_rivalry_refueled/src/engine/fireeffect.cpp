// Copyright 2019 Catalin G. Manciu

#include "fireeffect.h"
#include "renderer.h"

void FireEffect::initialize(uint8_t scale, const uint16_t* pal,
                            uint8_t palSize) {
    deleteBuffers();
    width = Defs::ScreenW / scale;
    height = Defs::ScreenH / scale;

    uint16_t bufferSize = static_cast<uint16_t>(width) *
                          static_cast<uint16_t>(height);
    buffer = new uint8_t[bufferSize];
    memset(buffer, 0, bufferSize);

    paletteSize = palSize;
    palette = new uint16_t[paletteSize];
    memcpy_P(palette, pal, paletteSize * sizeof(uint16_t));

    buffScale = scale;
    generateBaseline(0, height - 1, width, baselineH);
}

void FireEffect::deleteBuffers() {
    if (buffer) {
        delete[] buffer;
    }
    if (palette) {
        delete[] palette;
    }
}

FireEffect::~FireEffect() {
    deleteBuffers();
}

// Starts from y, goes upward
void FireEffect::generateBaseline(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    uint16_t offset = width * y + x;
    for (uint8_t line = 0; line < h; ++line) {
        memset(buffer + offset, (paletteSize - 1), w);
        offset -= width;
    }
}

void FireEffect::process(uint8_t x, uint8_t y, uint8_t w) {
    int16_t offset = width * y + x;
    uint8_t totalH = y - 1;
    uint32_t randomOffset = Utils::random32();
    uint8_t offsetBit = 0;
    int8_t increment = 1;
    for (uint8_t line = 0; line < totalH; ++line) {
        for (uint8_t col = 0; col < w; ++col) {
            int8_t value = ((randomOffset >> offsetBit) & 0x3);
            int8_t newPix = buffer[offset] - value;
            if (newPix < 0) {
                newPix = 0;
            }
            if (value == 3) {
                value = rand() % 3;
            }
            value -= 1;
            buffer[offset - width + value] = newPix;
            offsetBit += 2;
            if (offsetBit > 31) {
                randomOffset = Utils::random32();
                offsetBit = 0;
            }
            offset += increment;
        }
        increment *= -1;
        offset -= (width + increment);
    }
}

void FireEffect::update() {
    process(0, height - 1, width);
}

void FireEffect::render(SpriteRenderer* renderer) {
    renderer->renderFireEffect(buffer, palette, width,
                               height, buffScale,
                               0);
}

void FireEffect::addTempOverlay(uint8_t x, uint8_t y,
                                uint8_t w, uint8_t h) {
    uint8_t tempX = x / buffScale;
    uint8_t tempY = y / buffScale;
    uint8_t tempW = w / buffScale;
    uint8_t tempH = h / buffScale;
    generateBaseline(tempX, tempY, tempW, tempH);
    process(tempX, tempY, tempW);
}
