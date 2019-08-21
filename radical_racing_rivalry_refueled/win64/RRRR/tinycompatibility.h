// Copyright 2018 Catalin G. Manciu

#ifndef TINYCOMPATIBILITY_H_
#define TINYCOMPATIBILITY_H_

#define TAButton1       (1<<0)
#define TAButton2       (1<<1)

#define TAJoystickUp    (1<<0)
#define TAJoystickDown  (1<<1)
#define TAJoystickLeft  (1<<2)
#define TAJoystickRight (1<<3)

#include <stdint.h>
#include <cstring>
#include <SDL.h>

#define INIT_ZOOM_FACTOR    (8)
#define MAX_ZOOM_FACTOR     (12)

#define pgm_read_byte(x) *(x)
#define pgm_read_word(x) *(x)
#define PROGMEM
#define memcpy_P memcpy

void arcadeInit();
void processEvents();
bool isRunning();

uint8_t getButtonsState();
uint8_t getJoystickState();
uint32_t millis();
int32_t random(int32_t lower, int32_t upper);
int32_t random(int32_t upper);

class TinyScreenCompact {
 private:
    uint16_t* backBuffer;
    SDL_Surface* drawingSurface;
    SDL_Window*  window;
    uint8_t zoomFactor;
    void updateWindowSize();

 public:
    static const uint8_t xMax = 95;
    static const uint8_t yMax = 63;
    static const uint8_t xRes = xMax + 1;
    static const uint8_t yRes = yMax + 1;
    static TinyScreenCompact* instance;

    TinyScreenCompact() :
            backBuffer(nullptr),
            window(nullptr),
            zoomFactor(INIT_ZOOM_FACTOR) {
                TinyScreenCompact::instance = this; }
    ~TinyScreenCompact();
    void start();
    void beginFrame();
    void endFrame();
    void increaseZoom();
    void decreaseZoom();
    uint16_t* getSurface() { return backBuffer; }
};
#endif  // TINYCOMPATIBILITY_H_
