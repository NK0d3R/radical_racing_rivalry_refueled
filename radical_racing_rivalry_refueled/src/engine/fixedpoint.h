// Copyright 2018 Catalin G. Manciu

#ifndef FIXEDPOINT_H_
#define FIXEDPOINT_H_

#include "../stdinc.h"

template<typename T, typename T2, int shift>
struct FPValue {
    T value;

#define UPSHIFT(x)      ((x) << shift)
#define DOWNSHIFT(x)    ((x) >> shift)
    constexpr FPValue(): value(0) {}

#define INTEGRAL_CTOR(type)                             \
    constexpr FPValue(type v): value(UPSHIFT((T)v)) {}

#define FLOAT_CTOR(type)                                \
    constexpr FPValue(type v):                          \
                  value((T)(v * (type)UPSHIFT(1)))  {}

    INTEGRAL_CTOR(int16_t);
    INTEGRAL_CTOR(int32_t);
    FLOAT_CTOR(float);

    explicit FPValue(const void* progmem) {
        memcpy_P(this, progmem, sizeof(*this));
    }

    constexpr FPValue(const FPValue& other): value(other.value) {}
    constexpr FPValue(const FPValue&& other): value(other.value) {}

    inline FPValue& operator=(const FPValue& other) {
        value = other.value;
        return *this;
    }

    inline FPValue& operator=(const FPValue&& other) {
        value = other.value;
        return *this;
    }

    FPValue& operator*=(const FPValue& other) {
        value = DOWNSHIFT((T2)value * (T2)other.value);
        return *this;
    }

    FPValue& operator/=(const FPValue& other) {
        value = UPSHIFT((T2)value) / (T2)other.value;
        return *this;
    }

    FPValue& operator+=(const FPValue& other) {
        value += other.value;
        return *this;
    }

    FPValue& operator-=(const FPValue& other) {
        value -= other.value;
        return *this;
    }

    FPValue& operator*=(T val) {
        value *= val;
        return *this;
    }

    FPValue& operator/=(T val) {
        value /= val;
        return *this;
    }

#define BINARY_OP(op)                                               \
    friend FPValue operator op(FPValue lhs, const FPValue& rhs) {   \
        lhs op##= rhs;                                              \
        return lhs;                                                 \
    }

    BINARY_OP(+);
    BINARY_OP(-);
    BINARY_OP(/);
    BINARY_OP(*);

#define BINARY_OP(op)                                               \
    friend FPValue operator op(FPValue lhs, T rhs) {                \
        lhs op##= rhs;                                              \
        return lhs;                                                 \
    }

    BINARY_OP(*);
    BINARY_OP(/);

#define COMPARISON_OP(op)                                           \
    friend bool operator op(const FPValue& lhs,                     \
                            const FPValue& rhs) {                   \
        return (lhs.value op rhs.value);                            \
    }

    COMPARISON_OP(==);
    COMPARISON_OP(!=);
    COMPARISON_OP(<=);
    COMPARISON_OP(>=);
    COMPARISON_OP(<);
    COMPARISON_OP(>);

    inline T getInt() { return (value >> shift); }

    FPValue& clampUpper(const FPValue& upper) {
        if (*this > upper) {
            *this = upper;
        }
        return *this;
    }

    FPValue& clampLower(const FPValue& lower) {
        if (*this < lower) {
            *this = lower;
        }
        return *this;
    }

    FPValue& clamp(const FPValue& lower, const FPValue& upper) {
        return clampLower(lower).clampUpper(upper);
    }
};

#endif  // FIXEDPOINT_H_
