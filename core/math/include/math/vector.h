#pragma once

#include "utility/types.h"

typedef struct lbr_vector3_t {
    f32 x, y, z;
} LbrVector3;

typedef struct lbr_vector4_t {
    f32 x, y, z, w;
} LbrVector4;

typedef LbrVector4 LbrQuaternion;

void lbrVector3Add(const LbrVector3* a, const LbrVector3* b, LbrVector3* out);
void lbrVector3Sub(const LbrVector3* a, const LbrVector3* b, LbrVector3* out);
f32 lbrVector3Dot(const LbrVector3* a, const LbrVector3* b);
f32 lbrVector3Norm(const LbrVector3* a);
void lbrVector3Normalize(LbrVector3* a);
void lbrVector3Scale(LbrVector3* a, f32 scalar);
void lbrVector3Add(const LbrVector3* a, const LbrVector3* b, LbrVector3* out);
void lbrVector3Cross(const LbrVector3* a, const LbrVector3* b, LbrVector3* out);
void lbrVector3QuatRot(const LbrVector3* a, const LbrQuaternion* q, LbrVector3* out);
void lbrQuaternionAxisAngle(LbrQuaternion* q, const LbrVector3* axis, f32 angle);
