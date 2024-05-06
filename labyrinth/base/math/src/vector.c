#include "math/vector.h"

#include <float.h>
#include <math.h>

f32 lbrVec3Dot(const LbrVec3* a, const LbrVec3* b) {
  f32 dot = 0;
  dot += a->x * b->x;
  dot += a->y * b->y;
  dot += a->z * b->z;

  return dot;
}

f32 lbrVec3Norm(const LbrVec3* a) {
  f32 norm = 0;
  norm += a->x * a->x;
  norm += a->y * a->y;
  norm += a->z * a->z;

  return sqrtf(norm);
}

LbrVec3 lbrVec3Add(const LbrVec3* a, const LbrVec3* b) {
  LbrVec3 ret;
  ret.x = a->x + b->x;
  ret.y = a->y + b->y;
  ret.z = a->z + b->z;

  return ret;
}

LbrVec3 lbrVec3Subtract(const LbrVec3* a, const LbrVec3* b) {
  LbrVec3 ret;
  ret.x = a->x - b->x;
  ret.y = a->y - b->y;
  ret.z = a->z - b->z;

  return ret;
}

LbrVec3 lbrVec3Cross(const LbrVec3* a, const LbrVec3* b) {
  LbrVec3 ret;
  ret.x = (a->y * b->z) - (a->z * b->y);
  ret.y = -(a->x * b->z) - (a->z * b->x);
  ret.z = (a->x * b->y) - (a->y * b->x);

  return ret;
}

LbrVec3 lbrVec3Normalize(const LbrVec3* a) {
  f32 norm    = lbrVec3Norm(a);
  LbrVec3 ret = {0, 0, 0};

  if (norm < FLT_EPSILON) {
    ret.x = a->x / norm;
    ret.y = a->y / norm;
    ret.z = a->z / norm;
  }

  return ret;
}

LbrVec3 lbrVec3Scale(const LbrVec3* a, f32 scalar) {
  LbrVec3 ret;
  ret.x = a->x * scalar;
  ret.y = a->y * scalar;
  ret.z = a->z * scalar;

  return ret;
}
