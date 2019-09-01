// Copyright 2019 Catalin G. Manciu

#ifndef FILESYSTEM_WIN_H_
#define FILESYSTEM_WIN_H_

#include <stdio.h>

bool initializeFileSystem() {
    return true;
}

bool save(const char* name, const void* data, size_t size) {
    FILE* fp;
    errno_t result = fopen_s(&fp, name, "wb");
    if (result != 0) {
        return false;
    }
    bool success = fwrite(data, 1, size, fp) == size;
    fclose(fp);
    return success;
}

bool load(const char* name, void* dest, size_t size) {
    FILE* fp;
    errno_t result = fopen_s(&fp, name, "rb");
    if (result != 0) {
        return false;
    }
    bool success = fread(dest, 1, size, fp) == size;
    fclose(fp);
    return success;
}

#endif  // FILESYSTEM_WIN_H_
