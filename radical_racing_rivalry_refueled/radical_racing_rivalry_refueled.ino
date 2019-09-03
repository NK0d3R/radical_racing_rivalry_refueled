// Copyright 2018 Catalin G. Manciu

#include "src/game.h"

void setup() {
    R3R::getInstance().initialize(30);
}

void loop() {
    R3R::getInstance().update();
}

