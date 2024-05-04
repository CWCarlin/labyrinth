#include <stdio.h>

#include "math/vector.h"

int main() {
  LbrVec3 vec_1 = {1, 0, 0};
  LbrVec3 vec_2 = {1, 1, 0};
  vec_2         = lbrVec3Normalize(&vec_2);
  LbrVec3 v     = lbrVec3Cross(&vec_1, &vec_2);

  printf("%f %f %f\n", v.x, v.y, v.z);
  printf("%f\n", lbrVec3Dot(&vec_1, &v));

  return 0;
}
