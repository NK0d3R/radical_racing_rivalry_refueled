// Copyright 2018 Catalin G. Manciu

#ifndef STRINGMGR_H_
#define STRINGMGR_H_

#include "../stdinc.h"
#include "../res/strings.h"

uint8_t* getString(Strings str);
uint8_t* getStringBuffer();
uint8_t  getStringLen(Strings str);

#endif  // STRINGMGR_H_
