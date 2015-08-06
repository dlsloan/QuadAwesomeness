#ifndef types_h
#define types_h

#include <stm32f10x.h>

#define NULL (0)
/*
#define bool u32
#define true 0xffffffff
#define false 0
#define NULL 0

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef signed short i16;
typedef signed int i32;
typedef signed long i64;
*/
typedef char* err;

typedef union {
  u16 w;
  struct {
    u8 b1;
    u8 b0;
  } bw;
} u16_u8;

#endif
