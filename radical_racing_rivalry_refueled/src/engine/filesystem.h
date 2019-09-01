// Copyright 2019 Catalin G. Manciu

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "../stdinc.h"

bool initializeFileSystem();
bool save(const char* name, const void* data, size_t size);
bool load(const char* name, void* dest, size_t size);

#endif  // FILESYSTEM_H_
