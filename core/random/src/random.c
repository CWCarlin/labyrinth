#include "random/random.h"

#include <stdlib.h>
#include <time.h>
#include <math.h> // IWYU pragma: keep

void lbrCreateSeededRNGState(u64 seed, LbrRNGState *state) {
	state->state[0] = seed & 0x000000000000FFFF;
	state->state[1] = seed & 0x00000000FFFF0000;
	state->state[2] = seed & 0x0000FFFF00000000;
	state->state[3] = seed & 0xFFFF000000000000;

	for (u8 i = 0; i < 4; i++) {
		lbrGenerateRandomU64(state, 0, 0);
	}
}

void lbrCreateRNGState(LbrRNGState *state) {
	u64 seed = time(NULL) + clock();
	lbrCreateSeededRNGState(seed, state);
}

u32 lbrGenerateRandomU32(LbrRNGState *state, u32 min, u32 max) {
	u64* s = state->state;
	f64 result = (s[0] + s[3]) / powf(2, 64);
	u64 const t = s[1] << 17;
	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];
	s[2] ^= t;
	s[3] = (s[3] << 30) | (s[3] >> 34);

	result *= max - min;
	result += min;

	return result;
}

u64 lbrGenerateRandomU64(LbrRNGState *state, u64 min, u64 max) {
	u64* s = state->state;
	f64 result = (s[0] + s[3]) / powf(2, 64);
	u64 const t = s[1] << 17;
	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];
	s[2] ^= t;
	s[3] = (s[3] << 30) | (s[3] >> 34);

	result *= max - min;
	result += min;

	return result;
}

f32 lbrGenerateRandomF32(LbrRNGState *state, f32 min, f32 max) {
	u64* s = state->state;
	f64 result = (s[0] + s[3]) / powf(2, 64);
	u64 const t = s[1] << 17;
	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];
	s[2] ^= t;
	s[3] = (s[3] << 30) | (s[3] >> 34);

	result *= max - min;
	result += min;

	return result;
}

f64 lbrGenerateRandomF64(LbrRNGState *state, f64 min, f64 max) {
	u64* s = state->state;
	f64 result = (s[0] + s[3]) / powf(2, 64);
	u64 const t = s[1] << 17;
	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];
	s[2] ^= t;
	s[3] = (s[3] << 30) | (s[3] >> 34);

	result *= max - min;
	result += min;

	return result;
}
