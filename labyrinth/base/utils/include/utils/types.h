#pragma once

#include <stddef.h>
#include <stdint.h>

#define LBR_NOP                     __asm("nop")
#define LBR_ALIGN(value, alignment) ((value + alignment - 1) & ~(alignment - 1))

#define LBR_TRUE  1u
#define LBR_FALSE 0u

typedef uint8_t lbr_bool;

// unsigned integers
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef __uint128_t u128;
typedef size_t usize;

typedef uintptr_t uintptr;

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

usize lbrUsizeHash(const usize* p_usize);
u8 lbrUsizeEquality(const usize* p_usize_a, const usize* p_usize_b);
int lbrU32Compare(const void* p_u32_a, const void* p_u32_b);
