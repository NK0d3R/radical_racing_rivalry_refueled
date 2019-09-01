// Copyright 2019 Catalin G. Manciu

#ifndef FILESYSTEM_TINY_H_
#define FILESYSTEM_TINY_H_

#include <SPI.h>
#include <SdFat.h>

SdFat sd;

bool initializeFileSystem() {
    return sd.begin(10, SPI_FULL_SPEED);
}

bool save(const char* name, const void* data, size_t size) {
    SdFile file;
    bool result = file.open(name, O_RDWR | O_CREAT);
    if (!result) {
        return false;
    }
    result = (file.write(data, size) == size);
    if (result) {
        result = file.sync();
    }
    file.close();
    return result;
}

bool load(const char* name, void* dest, size_t size) {
    SdFile file;
    bool result = file.open(name, O_READ);
    if (!result) {
        return false;
    }
    result = (file.read(dest, size) == size);
    file.close();
    return result;
}

#endif  // FILESYSTEM_TINY_H_
