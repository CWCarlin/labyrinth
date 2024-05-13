#include "math/prime.h"

#include <math.h>

usize lbrGetNextPrime(usize num) {
  if (num == 1 || num == 2) {
    return num + 1;
  }

  usize next_num = num + 1 + (num % 2);
  for (;;) {
    if (lbrIsPrime(next_num)) {
      return next_num;
    }
    next_num += 2;
  }
}

u8 lbrIsPrime(usize num) {
  if (num <= 1 || num % 2 == 0) {
    return 0;
  }

  if (num == 2) {
    return 1;
  }

  usize num_sqrt = (usize)floorf(sqrtf((float)num));

  for (usize i = 3; i < num_sqrt + 1; i += 2) {
    if (num % i == 0) {
      return 0;
    }
  }

  return 1;
}
