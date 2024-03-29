// Copyright 2018 Catalin G. Manciu


#include "stringmgr.h"
#include "strings.hpp"

char s_strBuffer[32] = {0};

char* getString(Strings str) {
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

char* getStringBuffer() {
    return s_strBuffer;
}
