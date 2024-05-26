#include "utils/types.h"

void* LbrNOP() { return NULL; }

usize lbrUsizeHash(const usize* p_usize) { return *p_usize; }
u8 lbrUsizeEquality(const usize* p_usize_a, const usize* p_usize_b) { return *p_usize_a == *p_usize_b; }

int lbrU32Compare(const void* p_u32_a, const void* p_u32_b) {
  int result = 0;
  if (*(u32*)p_u32_a > *(u32*)p_u32_b) {
    result = 1;
  } else if (*(u32*)p_u32_a < *(u32*)p_u32_b) {
    result = -1;
  }

  return result;
}
