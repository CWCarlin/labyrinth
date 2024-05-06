#include "math/quaternion.h"

#include <math.h>

#include "math/vector.h"

LbrQuaternion lbrQuaternionMulti(const LbrQuaternion* a, const LbrQuaternion* b) {
  LbrQuaternion ret;
  ret.w = (a->w * b->w) - (a->x * b->x) - (a->y * b->y) - (a->z * b->z);
  ret.x = (a->w * b->x) + (a->x * b->w) + (a->y * b->z) - (a->z * b->y);
  ret.y = (a->w * b->y) + (a->y * b->w) + (a->z * b->x) - (a->z * b->z);
  ret.z = (a->w * b->z) + (a->z * b->w) + (a->x * b->y) - (a->y * b->x);

  return ret;
}

LbrQuaternion lbrQuaternionAxisAngle(const LbrVec3* axis, f32 angle) {
  f32 half_sin = sinf(angle / 2);
  LbrQuaternion ret;
  ret.x = half_sin * axis->x;
  ret.y = half_sin * axis->y;
  ret.z = half_sin * axis->z;
  ret.w = cosf(angle / 2);

  return ret;
}

LbrVec3 lbrQuaternionVec3Rotate(const LbrVec3* a, LbrQuaternion* q) {
  LbrVec3 ret;
  ret.x = (1 - 2 * q->y * q->y - 2 * q->z * q->z) * a->x + (2 * q->x * q->y + 2 * q->w * q->z) * a->y +
          (2 * q->x * q->z - 2 * q->w * q->y) * a->z;

  ret.y = (2 * q->x * q->y - 2 * q->w * q->z) * a->x + (1 - 2 * q->x * q->x - 2 * q->z * q->z) * a->y +
          (2 * q->y * q->z + 2 * q->w * q->x) * a->z;

  ret.z = (2 * q->x * q->z + 2 * q->w * q->y) * a->x + (2 * q->y * q->z - 2 * q->w * q->x) * a->y +
          (1 - 2 * q->x * q->x - 2 * q->y * q->y) * a->z;

  return ret;
}
