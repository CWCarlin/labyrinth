#pragma once

#include <stddef.h>
#include <stdint.h>

#define LBR_NOP                     __asm("nop")
#define LBR_ALIGN(bytes, alignment) (bytes = (bytes + alignment - 1) & ~(alignment - 1))

// unsigned integers
typedef uintptr_t uptr;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef __uint128_t u128;
typedef size_t usize;

// signed integers
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef __int128_t i128;

// floating points
typedef float f32;
typedef double f64;

void* LbrNOP();
