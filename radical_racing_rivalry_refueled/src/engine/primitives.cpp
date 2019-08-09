// Copyright 2018 Catalin G. Manciu

#include "primitives.h"

void Rect::clip(const Rect& other) {
    if (maxX < other.x || x > other.maxX ||
        maxY < other.y || y > other.maxY) {
        w = 0;
        h = 0;
        return;
    }

    if (x < other.x) {
        w -= (other.x - x);
        x = other.x;
        maxX = x + w - 1;
    }
    if (maxX > other.maxX) {
        w -= (maxX - other.maxX);
        maxX = x + w - 1;
    }
    if (y < other.y) {
        h -= (other.y - y);
        y = other.y;
        maxY = y + h - 1;
    }
    if (maxY > other.maxY) {
        h -= (maxY - other.maxY);
        maxY = y + h - 1;
    }
}
