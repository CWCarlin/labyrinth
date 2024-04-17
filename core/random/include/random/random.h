#pragma once

#include "utils/types.h"

typedef struct {
	u64 state[4];
} LbrRNGState;

void lbrCreateSeededRNGState(u64 seed, LbrRNGState *state);
void lbrCreateRNGState(LbrRNGState *state);
u32 lbrGenerateRandomU32(LbrRNGState *state, u32 min, u32 max);
u64 lbrGenerateRandomU64(LbrRNGState *state, u64 min, u64 max);
f32 lbrGenerateRandomF32(LbrRNGState *state, f32 min, f32 max);
f64 lbrGenerateRandomF64(LbrRNGState *state, f64 min, f64 max);
