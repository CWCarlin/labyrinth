#pragma once

#include "utils/types.h"

typedef struct lbr_vec3_t {
  f32 x, y, z;
} LbrVec3;

f32 lbrVec3Dot(const LbrVec3* a, const LbrVec3* b);
f32 lbrVec3Norm(const LbrVec3* a);
LbrVec3 lbrVec3Add(const LbrVec3* a, const LbrVec3* b);
LbrVec3 lbrVec3Subtract(const LbrVec3* a, const LbrVec3* b);
LbrVec3 lbrVec3Cross(const LbrVec3* a, const LbrVec3* b);
LbrVec3 lbrVec3Normalize(const LbrVec3* a);
LbrVec3 lbrVec3Scale(const LbrVec3* a, f32 scalar);
