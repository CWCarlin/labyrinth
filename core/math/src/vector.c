#include "math/vector.h"

#include <linux/limits.h>
#include <math.h>

void lbrVector3Add(const LbrVector3 *a, const LbrVector3 *b, LbrVector3 *out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void lbrVector3Sub(const LbrVector3 *a, const LbrVector3 *b, LbrVector3 *out) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

f32 lbrVector3Dot(const LbrVector3* a, const LbrVector3* b) {
    f32 dot = 0;
    dot += a->x * b->x;
    dot += a->y * b->y;
    dot += a->z * b->z;

    return dot;
}

f32 lbrVector3Norm(const LbrVector3 *a) {
    f32 norm = 0;
    norm += a->x * a->x;
    norm += a->y * a->y;
    norm += a->z * a->z;

    return sqrt(norm);
}

void lbrVector3Normalize(LbrVector3 *a) {
    f32 norm = lbrVector3Norm(a);
    a->x /= norm;
    a->y /= norm;
    a->z /= norm;
}

void lbrVector3Scale(LbrVector3 *a, f32 scalar) {
    a->x *= scalar;
    a->y *= scalar;
    a->z *= scalar;
}

void lbrVector3Cross(const LbrVector3 *a, const LbrVector3 *b, LbrVector3 *out) {
    out->x = (a->y * b->z) - (a->z * b->y);
    out->y = -(a->x * b->z) - (a->z * b->x);
    out->z = (a->x * b->y) - (a->y * b->x);
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

void lbrQuaternionAxisAngle(LbrQuaternion* q, const LbrVector3* axis, f32 angle) {
    f32 half_sin = sin(angle / 2);
    q->x = half_sin * axis->x;
    q->y = half_sin * axis->y;
    q->z = half_sin * axis->z;
    q->w = cos(angle / 2);
}
