// Copyright 2018 Catalin G. Manciu


#include "stringmgr.h"
#include "strings.hpp"

static uint8_t s_strBuffer[32] = {0};

uint8_t* getString(Strings str) {
    uint16_t stringInfo = pgm_read_word(&STRINGINFO[static_cast<int8_t>(str)]);
    uint8_t strLen = (stringInfo >> 11);
    uint8_t strOffset = (stringInfo & ((1 << 11) - 1));
    memcpy_P(s_strBuffer, &STRINGDATA[strOffset], strLen);
    s_strBuffer[strLen] = 0;
    return s_strBuffer;
}

uint8_t getStringLen(Strings str) {
    uint16_t stringInfo = pgm_read_word(&STRINGINFO[static_cast<int8_t>(str)]);
    return (stringInfo >> 11);
}

static inline uint8_t* getStringBuffer() {
    return s_strBuffer;
}
