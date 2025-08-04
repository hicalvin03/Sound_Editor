#ifndef LOAD_WAV_H
#define LOAD_WAV_H

#include <stdint.h>
#include <stddef.h>

// Function declarations
void wav_load(const char* filename, int16_t* dest);
void wav_save(const char* fname, int16_t* src, size_t len);

#endif