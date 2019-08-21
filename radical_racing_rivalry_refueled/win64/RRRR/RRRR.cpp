// Copyright 2019 Catalin G. Manciu

#include <iostream>
#include "../../radical_racing_rivalry_refueled.ino"

int main(int argc, char* argv[]) {
    setup();
    while (isRunning()) {
        processEvents();
        if (isRunning()) {
            loop();
        }
    }
    return 0;
}
