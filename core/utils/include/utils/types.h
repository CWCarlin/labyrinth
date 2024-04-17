#pragma once

#include <stdint.h> // IWYU pragma: keep

// boolean
typedef uint8_t LbrBool;
#define LBR_TRUE 1
#define LBR_FALSE 0


// unsigned integers
typedef size_t usize;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef __uint128_t u128;

// signed integers
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef __int128_t i128;

// floating points
typedef float f32;
typedef double f64;
