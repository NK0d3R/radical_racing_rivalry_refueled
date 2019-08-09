// Copyright 2018 Catalin G. Manciu

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "../stdinc.h"

template <typename T> struct LineT;
template <typename T> struct VectorT;

struct Rect {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    int16_t maxX;
    int16_t maxY;

    Rect() {
        set(0, 0, 0, 0);
    }

    Rect(int16_t _x, int16_t _y, int16_t _w, int16_t _h) {
        set(_x, _y, _w, _h);
    }

    inline void set(int16_t _x, int16_t _y, int16_t _w, int16_t _h) {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
        maxX = x + w - 1;
        maxY = y + h - 1;
    }

    bool isEmpty() const { return (w == 0 || h == 0); }
    void clip(const Rect& other);
    bool coordsInside(int16_t px, int16_t py) {
        return (px >= x && px <= maxX &&
                py >= y && py <= maxY);
    }
    template <typename T>
    void clipLineX(LineT<T>* line) {
#define OUTSIDE_LEFT        (1)
#define OUTSIDE_RIGHT       (1<<1)
#define INSIDE              (1<<2)
#define OUTSIDE_MASK        (OUTSIDE_LEFT | OUTSIDE_RIGHT)
#define GET_STATUS_X(xpos)  (xpos < fpX ? OUTSIDE_LEFT :                \
                            (xpos > fpXMax ? OUTSIDE_RIGHT : INSIDE));
        T fpX = T(x);
        T fpXMax = T(maxX);
        uint8_t startXStatus = GET_STATUS_X(line->start.x);
        uint8_t endXStatus = GET_STATUS_X(line->end.x);
        if (((startXStatus & endXStatus) & OUTSIDE_MASK) != 0) {
            line->start = {0, 0};
            line->end = {0, 0};
            return;
        }
        VectorT<T> diff = line->end - line->start;
        if (diff.x == 0) {
             if (((startXStatus | endXStatus) & OUTSIDE_MASK) != 0) {
                line->start = {0, 0};
                line->end = {0, 0};
             }
            return;
        }
        T pitch = diff.y / diff.x;
        if (startXStatus != INSIDE) {
            if (line->start.x < fpX) {
                line->start.x = fpX;
            } else {
                line->start.x = fpXMax;
            }
            line->start.y = line->end.y -
                            (pitch * (line->end.x - line->start.x));
        }
        if (endXStatus != INSIDE) {
            if (line->end.x < fpX) {
                line->end.x = fpX;
            } else {
                line->end.x = fpXMax;
            }
            line->end.y = line->start.y +
                          (pitch * (line->end.x - line->start.x));
        }
    }
};

template <typename T>
struct VectorT {
    T x;
    T y;

    VectorT(): x(0), y(0) {
    }

    VectorT(T _x, T _y):x(_x), y(_y) {
    }

    VectorT& operator += (const VectorT& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    VectorT& operator -= (const VectorT& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    VectorT& operator *= (T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    VectorT& operator /= (T scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    VectorT operator + (const VectorT& other) {
        return {x + other.x, y + other.y};
    }

    VectorT operator - (const VectorT& other) {
        return {x - other.x, y - other.y};
    }

    T magnitudeSQ() { return ((x * x) + (y * y)); }
    T magnitude() { return T(sqrt(x * x + y * y)); }
    T dot(const VectorT& other) {
        return (x * other.x) + (y * other.y);
    }
    void normalize() { T mag = magnitude(); *this /= mag; }
};

template <typename T>
struct LineT {
    VectorT<T> start;
    VectorT<T> end;

    LineT(T start_x, T start_y, T end_x, T end_y):
            start(start_x, start_y), end(end_x, end_y) {
    }

    bool exists() {
        VectorT<T> diff = end - start;
        return (diff.x.getInt() != 0) || (diff.y.getInt() != 0);
    }
};

#endif  // PRIMITIVES_H_

