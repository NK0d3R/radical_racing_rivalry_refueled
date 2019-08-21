// Copyright 2019 Catalin G. Manciu

#include "tinycompatibility.h"
#include <Windows.h>
#include <unordered_map>

TinyScreenCompact* TinyScreenCompact::instance = nullptr;

bool appRunning = true;

uint8_t buttons = 0;
uint8_t joystick = 0;

void arcadeInit() {
}

uint32_t millis() {
    return timeGetTime();
}

int32_t random(int32_t lower, int32_t upper) {
    return lower + rand() % upper;
}

int32_t random(int32_t upper) {
    return random(0, upper);
}

uint8_t getButtonsState() {
    return buttons;
}

uint8_t getJoystickState() {
    return joystick;
}

typedef std::unordered_map<SDL_Scancode, uint8_t> ButtonMap;

bool isRunning() {
    return appRunning;
}

void processEvents() {
    static ButtonMap btnEvents(
        { {SDL_SCANCODE_Z, TAButton1}, {SDL_SCANCODE_X, TAButton2} });
    static ButtonMap joyEvents(
        { {SDL_SCANCODE_UP, TAJoystickUp},
          {SDL_SCANCODE_DOWN, TAJoystickDown},
          {SDL_SCANCODE_LEFT, TAJoystickLeft},
          {SDL_SCANCODE_RIGHT, TAJoystickRight} });
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        switch (evt.type) {
        case SDL_QUIT: appRunning = false; break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            auto code = evt.key.keysym.scancode;
            auto res = btnEvents.find(code);
            if (res != btnEvents.end()) {
                if (evt.type == SDL_KEYDOWN) {
                    buttons |= (res->second);
                } else {
                    buttons &= ~(res->second);
                }
                break;
            }
            res = joyEvents.find(code);
            if (res != joyEvents.end()) {
                if (evt.type == SDL_KEYDOWN) {
                    joystick |= (res->second);
                } else {
                    joystick &= ~(res->second);
                }
                break;
            }
            if (evt.type == SDL_KEYUP) {
                if (code == SDL_SCANCODE_KP_PLUS) {
                    TinyScreenCompact::instance->increaseZoom();
                }
                if (code == SDL_SCANCODE_KP_MINUS) {
                    TinyScreenCompact::instance->decreaseZoom();
                }
            }
            break;
        }
    }
}


TinyScreenCompact::~TinyScreenCompact() {
    if (backBuffer) {
        delete[] backBuffer;
    }
}

void TinyScreenCompact::start() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Radical Racing:Rivalry Refueled",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              xRes * zoomFactor,
                              yRes * zoomFactor,
                              0);
    drawingSurface = SDL_CreateRGBSurface(0, xRes, yRes, 16,
                                          (0x1f),
                                          (0x3f << 5),
                                          (0x1f << 11),
                                          0);
    backBuffer = new uint16_t[xRes * yRes];
}

void TinyScreenCompact::increaseZoom() {
    if (zoomFactor < MAX_ZOOM_FACTOR) {
        zoomFactor++;
        updateWindowSize();
    }
}

void TinyScreenCompact::decreaseZoom() {
    if (zoomFactor > 1) {
        zoomFactor--;
        updateWindowSize();
    }
}

void TinyScreenCompact::updateWindowSize() {
    SDL_SetWindowSize(window, xRes * zoomFactor, yRes * zoomFactor);
}

void TinyScreenCompact::beginFrame() {
    memset(backBuffer, 0, xRes * yRes * 2);
}

void TinyScreenCompact::endFrame() {
    SDL_LockSurface(drawingSurface);
    uint16_t* surface = reinterpret_cast<uint16_t*>(drawingSurface->pixels);
    for (int idx = 0; idx < xRes * yRes; ++idx) {
        surface[idx] = (backBuffer[idx] >> 8) | (backBuffer[idx] << 8);
    }
    SDL_UnlockSurface(drawingSurface);
    SDL_BlitScaled(drawingSurface, NULL, SDL_GetWindowSurface(window), NULL);
    SDL_UpdateWindowSurface(window);
}
