#include "allocators/alloc_info.h"
#include "allocators/linear_alloc.h"
#include "data_structures/stack.h"

#include <stdio.h>

typedef struct test {
	u64 one;
	u64 two;
	u64 three;
	u64 four;
} test;


int main() {
	LbrLinearAllocator alloc;
	LbrAllocInfo info;
	LbrStack stack;

	lbrCreateLinearAllocator(&alloc, 128);
	info = lbrLinearAllocatorGetAllocInfo(&alloc, 128);
	lbrCreateStack(&stack, 128, info);

	struct test t = {1, 2, 3, 4};

	lbrStackPush(&stack, &t);

	struct test p;
	lbrStackPop(&stack, &p);
	printf("%lu %lu\n", p.one, p.two);
	return 0;
}
