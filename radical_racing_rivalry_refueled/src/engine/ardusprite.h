// Copyright 2018 Catalin G. Manciu

#ifndef ARDUSPRITE_H_
#define ARDUSPRITE_H_

#include "../stdinc.h"

#if _WIN64
    #pragma pack(push, 1)
#endif

#define INVALID_ANIM  (0xFF)
#define INVALID_FRAME (0xFF)

#define ANCHOR_LEFT     (1)
#define ANCHOR_HCENTER  (1<<1)
#define ANCHOR_RIGHT    (1<<2)
#define ANCHOR_TOP      (1<<3)
#define ANCHOR_VCENTER  (1<<4)
#define ANCHOR_BOTTOM   (1<<5)

#define ARD_FLAGS_FLIP_X    (1 << 0)
#define ARD_FLAGS_FLIP_Y    (1 << 1)

#define GET_W_FROM_SIZE(sz) (uint16_t)(sz & 0xFFFF)
#define GET_H_FROM_SIZE(sz) (uint16_t)((sz & 0xFFFF0000) >> 16)

class SpriteRenderer;

inline constexpr uint8_t invertBits(uint8_t value, uint8_t mask) {
    return ((~value) & mask) | (value & (~mask));
}

inline constexpr uint8_t getBPPFromElementFlags(uint8_t value) {
    return ((value >> 3) & 0x3);
}

inline constexpr uint8_t getHasAlphaFromElementFlags(uint8_t value) {
    return ((value >> 2) & 0x1);
}

struct SpriteElement {
    uint8_t     width;
    uint8_t     height;
    uint16_t    imageOffset;
} PACKED;

struct SpriteFrameElement {
    uint8_t     elementIdx;
    uint8_t     flags;
    int8_t      posX;
    int8_t      posY;
} PACKED;

struct SpriteAnimFrame {
    uint8_t   duration;
    uint8_t   elemsNb;
    uint8_t   frameElemsStart;
} PACKED;

struct SpriteAnim {
    uint8_t   framesNb;
    uint8_t   framesStart;
} PACKED;

struct Sprite {
    uint8_t   elemsNb;
    uint8_t   animsNb;
    uint8_t   paletteSize;
    uint8_t   sprFlags;
    const uint8_t*  imageData;
    uint16_t* paletteData;

    const SpriteElement* elements;
    const SpriteAnim* anims;
    const SpriteAnimFrame* animFrames;
    const SpriteFrameElement* frameElems;

    void drawAnimationFrame(SpriteRenderer* renderer, uint8_t animation,
                            uint8_t frame, int16_t posX,
                            int16_t posY, uint8_t flags);
    void fillSingleLine(SpriteRenderer* renderer, uint8_t element,
                        uint8_t posX, uint8_t posY, uint8_t srcY);
    uint8_t getElementW(uint8_t element);
    uint8_t getElementH(uint8_t element);
    int32_t measureAnimationFrame(uint8_t animation, uint8_t frame);
    void create(const uint8_t* data);
    virtual ~Sprite() { if (paletteData) delete[] paletteData; }
} PACKED;

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
} PACKED;

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
} PACKED;

#if _WIN64
    #pragma pack(pop)
#endif

#endif  // ARDUSPRITE_H_
