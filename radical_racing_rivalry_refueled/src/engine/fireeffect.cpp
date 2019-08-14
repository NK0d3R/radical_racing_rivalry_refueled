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
    generateBaseline();
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

void FireEffect::generateBaseline() {
    uint16_t offset = width * (height - 1);
    for (uint16_t line = 0; line < baselineH; ++line) {
        memset(buffer + offset, (paletteSize - 1), width);
        offset -= width;
    }
}

void FireEffect::process() {
    uint16_t offset = height * width - 1;
    uint32_t randomOffset = Utils::random32();
    uint8_t offsetBit = 0;
    for (uint8_t line = 0; line < height - 2; ++line) {
        for (uint8_t col = 0; col < width; ++col) {
            int8_t value = ((randomOffset >> offsetBit) & 0x3);
            if (value == 3) {
                value = random(0, 3);
            }
            int8_t newPix = buffer[offset] - value;
            if (newPix < 0) {
                newPix = 0;
            }
            buffer[offset - width + (value - 1)] = newPix;
            offsetBit += 2;
            if (offsetBit > 31) {
                randomOffset = Utils::random32();
                offsetBit = 0;
            }
            offset--;
        }
    }
}

void FireEffect::update() {
    process();
}

void FireEffect::render(SpriteRenderer* renderer) {
    renderer->renderFireEffect(buffer, palette, width,
                               height - baselineH, buffScale,
                               buffScale * baselineH - 1);
}
