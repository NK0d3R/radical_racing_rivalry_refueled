// Copyright 2018 Catalin G. Manciu

#include "ardusprite.h"
#include "renderer.h"

void Sprite::create(const uint8_t* data) {
    int totalAnimFrames = 0;
    int totalFrameElems = 0;

    elemsNb = pgm_read_byte(data++);
    animsNb = pgm_read_byte(data++);

    elements = reinterpret_cast<const SpriteElement*>(data);
    data += (elemsNb) * sizeof(SpriteElement);
    anims = reinterpret_cast<const SpriteAnim*>(data);

    data += (animsNb) * sizeof(SpriteAnim);
    animFrames = reinterpret_cast<const SpriteAnimFrame*>(data);
    for (int anim = 0; anim < animsNb; ++anim) {
        SpriteAnim current;
        memcpy_P(&current, &anims[anim], sizeof(SpriteAnim));
        totalAnimFrames += current.framesNb;
    }

    data += totalAnimFrames * sizeof(SpriteAnimFrame);
    frameElems = reinterpret_cast<const SpriteFrameElement*>(data);
    for (int anim = 0; anim < animsNb; ++anim) {
        SpriteAnim currentAnim;
        memcpy_P(&currentAnim, &anims[anim], sizeof(SpriteAnim));
        for (uint8_t frame = 0; frame < currentAnim.framesNb; ++frame) {
            SpriteAnimFrame currentFrame;
            memcpy_P(&currentFrame,
                     &animFrames[currentAnim.framesStart + frame],
                     sizeof(SpriteAnimFrame));
            totalFrameElems += currentFrame.elemsNb;
        }
    }

    data += totalFrameElems * sizeof(SpriteFrameElement);
    imageData = data;
}

void Sprite::drawAnimationFrame(SpriteRenderer* renderer,
                                uint8_t animation, uint8_t frame,
                                int16_t posX, int16_t posY,
                                uint8_t flags) {
    SpriteAnim currentAnim;
    memcpy_P(&currentAnim, &anims[animation], sizeof(SpriteAnim));

    SpriteAnimFrame currentFrame;
    memcpy_P(&currentFrame,
             &animFrames[currentAnim.framesStart + frame],
             sizeof(SpriteAnimFrame));

    int16_t elemPosX;
    int16_t elemPosY;
    uint8_t elemFlags;

    for (int idx = 0; idx < currentFrame.elemsNb; ++idx) {
        SpriteFrameElement currentFrameElem;
        SpriteElement currentElem;

        memcpy_P(&currentFrameElem,
                 &frameElems[currentFrame.frameElemsStart + idx],
                 sizeof(SpriteFrameElement));

        elemPosX = (int16_t)currentFrameElem.posX;
        elemPosY = (int16_t)currentFrameElem.posY;

        elemFlags = currentFrameElem.flags;
        memcpy_P(&currentElem, &elements[currentFrameElem.elementIdx],
                 sizeof(SpriteElement));

        if (flags & ARD_FLAGS_FLIP_X) {
          uint8_t elem_width = currentElem.width;
          elemPosX = -elemPosX - elem_width + 1;
          elemFlags = invertBits(elemFlags, ARD_FLAGS_FLIP_X);
        }

        uint8_t elem_height = getHeight(currentElem.height);

        if (flags & ARD_FLAGS_FLIP_Y) {
          elemPosY = -elemPosY - elem_height + 1;
          elemFlags = invertBits(elemFlags, ARD_FLAGS_FLIP_Y);
        }

        renderer->drawSpriteData(imageData + currentElem.imageOffset,
                                 posX + elemPosX, posY + elemPosY,
                                 currentElem.width, elem_height,
                                 elemFlags | getOpacityBit(currentElem.height));
    }
}

int32_t Sprite::measureAnimationFrame(uint8_t animation, uint8_t frame) {
    SpriteAnim currentAnim;
    memcpy_P(&currentAnim, &anims[animation], sizeof(SpriteAnim));

    SpriteAnimFrame currentFrame;
    memcpy_P(&currentFrame,
             &animFrames[currentAnim.framesStart + frame],
             sizeof(SpriteAnimFrame));

    int16_t width = 0;
    int16_t height = 0;

    int16_t crtWidth = 0;
    int16_t crtHeight = 0;

    for (int idx = 0; idx < currentFrame.elemsNb; ++idx) {
        SpriteFrameElement currentFrameElem;
        SpriteElement currentElem;

        memcpy_P(&currentFrameElem,
                 &frameElems[currentFrame.frameElemsStart + idx],
                 sizeof(SpriteFrameElement));
        memcpy_P(&currentElem, &elements[currentFrameElem.elementIdx],
                 sizeof(SpriteElement));

        crtWidth = (int16_t)currentFrameElem.posX +
                                (int16_t)currentElem.width;
        crtHeight = (int16_t)currentFrameElem.posY +
                                (int16_t)currentElem.height;
        if (crtWidth > width) {
            width = crtWidth;
        }
        if (crtHeight > height) {
            height = crtHeight;
        }
    }
    return (width | (height << 16));
}

void SpriteAnimator::init(Sprite* animSprite) {
    sprite = animSprite;
    isPlaying = false;
}

void SpriteAnimator::setAnimation(uint8_t animation, uint8_t flags, bool loop) {
    currentAnim = animation;
    currentAnimFrame = 0;
    currentAnimFlags = flags;
    currentFrameTime = 0;
    loopAnim = loop;
    isPlaying = true;
}

bool SpriteAnimator::update(uint16_t dt) {
    if (isPlaying == false) {
        return false;
    }
    SpriteAnim anim;
    SpriteAnimFrame currentFrame;
    memcpy_P(&anim, &sprite->anims[currentAnim], sizeof(SpriteAnim));
    memcpy_P(&currentFrame,
             &sprite->animFrames[anim.framesStart +
                                  currentAnimFrame],
             sizeof(SpriteAnimFrame));

    uint16_t currentDuration = (uint16_t)currentFrame.duration * 10;
    currentFrameTime += dt;

    while (currentFrameTime > currentDuration) {
        currentFrameTime -= currentDuration;
        if (currentAnimFrame < anim.framesNb - 1) {
            currentAnimFrame++;
        } else if (loopAnim) {
            currentAnimFrame = 0;
        } else {
            isPlaying = false;
            return false;
        }
        memcpy_P(&currentFrame,
                 &sprite->animFrames[anim.framesStart +
                                      currentAnimFrame],
                 sizeof(SpriteAnimFrame));
        currentDuration = (uint16_t)currentFrame.duration * 10;
    }
    return true;
}

void SpriteAnimator::draw(SpriteRenderer* renderer,
                          int16_t posX, int16_t posY) {
    sprite->drawAnimationFrame(renderer, currentAnim,
                               currentAnimFrame, posX, posY,
                               currentAnimFlags);
}

void Font::create(const uint8_t* data, const uint8_t* mapData, uint8_t mapLen,
                  uint8_t spaceW, uint8_t height, uint8_t defFrame,
                  uint8_t minChar) {
    mapping = mapData;
    mappingLen = mapLen;
    spaceWidth = spaceW;
    fontHeight = height;
    defaultFrame = defFrame;
    firstAvailableChar = minChar;
    Sprite::create(data);
}

#define MAX_STR_SIZE_BUFF   (32)
uint8_t s_strSizeBuff[MAX_STR_SIZE_BUFF];

uint16_t Font::getStringWidth(const char* string, int8_t charSpacing) {
    uint16_t width = 0;
    uint8_t  idx = 0;
    uint8_t  crtFrame;
    uint16_t crtFrameW;

    while (string[idx] && idx < MAX_STR_SIZE_BUFF) {
        uint8_t mapIndex = string[idx] - firstAvailableChar;
        if (mapIndex < mappingLen) {
            crtFrame = pgm_read_byte(&mapping[mapIndex]);
            if (crtFrame != defaultFrame) {
                crtFrameW = GET_W_FROM_SIZE(
                                measureAnimationFrame(0, crtFrame)) +
                            charSpacing;
            } else {
                crtFrameW = spaceWidth;
            }
        } else {
            crtFrameW = spaceWidth;
        }
        s_strSizeBuff[idx] = crtFrameW;
        width += crtFrameW;
        ++idx;
    }
    return width;
}

void Font::drawString(SpriteRenderer* renderer, const char* string,
                      int16_t posX, int16_t posY,
                      uint8_t anchor, int8_t charSpacing) {
    uint8_t crtFrame;
    int16_t width = getStringWidth(string, charSpacing);
    uint8_t idx = 0;

    if (anchor & (ANCHOR_HCENTER | ANCHOR_RIGHT)) {
        if (anchor & ANCHOR_HCENTER) {
            posX -= (width >> 1);
        } else {
            posX -= width;
        }
    }
    if (anchor & ANCHOR_VCENTER) {
        posY += (fontHeight >> 1);
    } else if (anchor & ANCHOR_TOP) {
        posY += fontHeight;
    }
    while (string[idx] && idx < MAX_STR_SIZE_BUFF) {
        uint8_t mapIndex = string[idx] - firstAvailableChar;
        if (mapIndex < mappingLen) {
            crtFrame = pgm_read_byte(&mapping[mapIndex]);
            if (crtFrame != defaultFrame) {
                drawAnimationFrame(renderer, 0, crtFrame, posX, posY, 0);
            }
        }
        posX += s_strSizeBuff[idx];
        ++idx;
    }
}
