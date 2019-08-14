// Copyright 2018 Catalin G. Manciu

#include "src/game.h"

void setup() {
    RRRR::getInstance().initialize(30);
}

void loop() {
    RRRR::getInstance().update();
}

