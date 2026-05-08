#ifndef XCHESS_UTILS_H
#define XCHESS_UTILS_H
#include "common.h"

u32 FNV(const void* key,u32 h);
u8* read_tga(const char* filename,i32* out_width,i32* out_height);

#endif
