// Copyright 2018 Catalin G. Manciu

#ifndef ARDUSPRITE_H_
#define ARDUSPRITE_H_

#include "../stdinc.h"

#define INVALID_ANIM  (0xFF)
#define INVALID_FRAME (0xFF)

#define ANCHOR_LEFT     (1)
#define ANCHOR_HCENTER  (1<<1)
#define ANCHOR_RIGHT    (1<<2)
#define ANCHOR_TOP      (1<<3)
#define ANCHOR_VCENTER  (1<<4)
#define ANCHOR_BOTTOM   (1<<5)

#define ARD_FLAGS_FLIP_X    (1)
#define ARD_FLAGS_FLIP_Y    (2)

#define GET_W_FROM_SIZE(sz) (uint16_t)(sz & 0xFFFF)
#define GET_H_FROM_SIZE(sz) (uint16_t)((sz & 0xFFFF0000) >> 16)

class SpriteRenderer;

inline constexpr uint8_t invertBits(uint8_t value, uint8_t mask) {
    return ((~value) & mask) | (value & (~mask));
}

inline constexpr uint8_t getHeight(uint8_t value) {
    return (value & 0x7F);
}

inline constexpr uint8_t getOpacityBit(uint8_t value) {
    return (value & 0x80);
}

struct SpriteElement {
    uint8_t     width;
    uint8_t     height;
    uint16_t    imageOffset;
} __attribute__((__packed__));

struct SpriteFrameElement {
    uint8_t     elementIdx;
    uint8_t     flags;
    int8_t      posX;
    int8_t      posY;
} __attribute__((__packed__));

struct SpriteAnimFrame {
    uint8_t   duration;
    uint8_t   elemsNb;
    uint8_t   frameElemsStart;
}__attribute__((__packed__));

struct SpriteAnim {
    uint8_t   framesNb;
    uint8_t   framesStart;
}__attribute__((__packed__));

struct Sprite {
    uint8_t   elemsNb;
    uint8_t   animsNb;
    const uint8_t*  imageData;

    const SpriteElement* elements;
    const SpriteAnim* anims;
    const SpriteAnimFrame* animFrames;
    const SpriteFrameElement* frameElems;

    void drawAnimationFrame(SpriteRenderer* renderer, uint8_t animation,
                            uint8_t frame, int16_t posX,
                            int16_t posY, uint8_t flags);
    int32_t measureAnimationFrame(uint8_t animation, uint8_t frame);
    void create(const uint8_t* data);
} __attribute__((__packed__));

struct Font : public Sprite {
    const uint8_t*    mapping;
    uint8_t     mappingLen;
    uint8_t     defaultFrame;
    uint8_t     spaceWidth;
    uint8_t     fontHeight;
    uint8_t     firstAvailableChar;

    void create(const uint8_t* data, const uint8_t* mapData, uint8_t mapLen,
                uint8_t spaceW, uint8_t height, uint8_t defFrame,
                uint8_t minChar);
    void drawString(SpriteRenderer* renderer, const char* string,
                    int16_t posX, int16_t posY,
                    uint8_t anchor, int8_t charSpacing = 0);
    uint16_t getStringWidth(const char* string, int8_t charSpacing);
}__attribute__((__packed__));

class SpriteAnimator {
    Sprite*      sprite;
    uint16_t     currentFrameTime;

    bool         loopAnim;
    bool         isPlaying;

    uint8_t      currentAnim;
    uint8_t      currentAnimFrame;
    uint8_t      currentAnimFlags;

 public:
    void init(Sprite* animSprite);
    void setAnimation(uint8_t animation, uint8_t flags, bool loop);
    bool update(uint16_t dt);
    void draw(SpriteRenderer* renderer, int16_t posX, int16_t posY);
    bool animPlaying() { return isPlaying; }
}__attribute__((__packed__));

#endif  // ARDUSPRITE_H_

