// Copyright 2019 Catalin G. Manciu

#ifndef CONTROLS_H_
#define CONTROLS_H_

#if _WIN64
    #include "tinycompatibility.h"
#else
    #include "lib/TinyArcade.h"
#endif

enum : uint8_t {
    BTN_A = TAButton1,
    BTN_B = TAButton2,
    DPAD_UP = (TAJoystickUp << 2),
    DPAD_DOWN = (TAJoystickDown << 2),
    DPAD_LEFT = (TAJoystickLeft << 2),
    DPAD_RIGHT = (TAJoystickRight << 2)
};

#endif  // CONTROLS_H_
