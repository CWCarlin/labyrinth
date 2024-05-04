#pragma once

#include "math/vector.h"
#include "utils/types.h"

typedef struct lbr_quaternion_t {
  f32 w, x, y, z;
} LbrQuaternion;

LbrQuaternion lbrQuaternionMulti(const LbrQuaternion* a, const LbrQuaternion* b);
LbrQuaternion lbrQuaternionAxisAngle(const LbrVec3* axis, f32 angle);
LbrVec3 lbrQuaternionVec3Rotate(const LbrVec3* a, LbrQuaternion* q);
