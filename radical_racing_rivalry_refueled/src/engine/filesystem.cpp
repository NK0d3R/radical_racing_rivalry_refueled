// Copyright 2019 Catalin G. Manciu

#include "filesystem.h"

#if _WIN64
    #include "filesystem_win.h"
#else
    #include "filesystem_tiny.h"
#endif
