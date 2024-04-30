#pragma once

#include "math/vector.h"

typedef struct lbr_quaternion_t {
	f32 w, x, y, z;
} LbrQuaternion;

void lbrQuaternionMult(const LbrQuaternion* a, const LbrQuaternion* b, LbrQuaternion* out);
void lbrQuaternionAxisAngle(LbrQuaternion* q, const LbrVector3* axis, f32 angle);
void lbrVector3QuatRot(const LbrVector3* a, const LbrQuaternion* q, LbrVector3* out);
