// Copyright 2018 Catalin G. Manciu

#ifndef STDINC_H_
#define STDINC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#if _WIN64
    #include "tinycompatibility.h"
    #define PACKED
#else
    #include <avr/pgmspace.h>
    #include "lib/TinyScreenCompact.h"
    #include "lib/TinyArcade.h"
    #define PACKED __attribute__((__packed__))
#endif
#endif  // STDINC_H_
