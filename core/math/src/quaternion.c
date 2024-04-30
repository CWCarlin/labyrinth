#include "math/quaternion.h"

#include <math.h>  // IWYU pragma: keep

#include "math/vector.h"

void lbrQuaternionMult(const LbrQuaternion *a, const LbrQuaternion *b, LbrQuaternion *out) {
	out->w = (a->w * b->w) - (a->x * b->x) - (a->y * b->y) - (a->z * b->z);
	out->x = (a->w * b->x) + (a->x * b->w) + (a->y * b->z) - (a->z * b->y);
	out->y = (a->w * b->y) + (a->y * b->w) + (a->z * b->x) - (a->z * b->z);
	out->z = (a->w * b->z) + (a->z * b->w) + (a->x * b->y) - (a->y * b->x);
}

void lbrQuaternionAxisAngle(LbrQuaternion* q, const LbrVector3* axis, f32 angle) {
    f32 half_sin = sin(angle / 2);
    q->x = half_sin * axis->x;
    q->y = half_sin * axis->y;
    q->z = half_sin * axis->z;
    q->w = cos(angle / 2);
}

void lbrVector3QuatRot(const LbrVector3 *a, const LbrQuaternion *q, LbrVector3 *out) {
    out->x = (1 - 2 * q->y * q->y - 2 * q->z * q->z) * a->x +
                (2 * q->x * q->y + 2 * q->w * q->z) * a->y +
                (2 * q->x * q->z - 2 * q->w * q->y) * a->z;
    
    out->y = (2 * q->x * q->y - 2 * q->w * q->z) * a->x +
                (1 - 2 * q->x * q->x - 2 * q->z * q->z) * a->y +
                (2 * q->y * q->z + 2 * q->w * q->x) * a->z;
    
    out->z = (2 * q->x * q->z + 2 * q->w * q->y) * a->x +
                (2 * q->y * q->z - 2 * q->w * q->x) * a->y +
                (1 - 2 * q->x * q->x - 2 * q->y * q->y) * a->z;
}


