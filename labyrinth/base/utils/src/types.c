#include "utils/types.h"

void* LbrNOP() { return NULL; }

usize lbrUsizeHash(const usize* p_usize) { return *p_usize; }
u8 lbrUsizeEquality(const usize* p_usize_a, const usize* p_usize_b) { return *p_usize_a == *p_usize_b; }
