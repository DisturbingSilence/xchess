#ifndef XCHESS_COMMON_H
#define XCHESS_COMMON_H

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef signed long long i64;
typedef signed int i32;
typedef signed short i16;
typedef signed char i8;

static_assert(sizeof(u64) == 8 && sizeof(u64) == sizeof(i64));
static_assert(sizeof(u32) == 4 && sizeof(u32) == sizeof(i32));
static_assert(sizeof(u16) == 2 && sizeof(u16) == sizeof(i16));
static_assert(sizeof(u8) == 1 && sizeof(u8) == sizeof(i8));
#endif
