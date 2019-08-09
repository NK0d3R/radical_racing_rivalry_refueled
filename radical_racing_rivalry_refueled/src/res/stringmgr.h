// Copyright 2018 Catalin G. Manciu

#ifndef STRINGMGR_H_
#define STRINGMGR_H_

#include "../stdinc.h"
#include "../res/strings.h"

char* getString(Strings str);
char* getStringBuffer();
uint8_t  getStringLen(Strings str);

#endif  // STRINGMGR_H_
