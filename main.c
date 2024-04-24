#include "allocators/alloc_info.h"
#include "allocators/linear_alloc.h"
#include "data_structures/queue.h"

#include <stdio.h>

typedef struct test {
	u64 one;
	u64 two;
	u64 three;
	u64 four;
} test;


int main() {
	LbrLinearAllocator alloc;
	LbrAllocCallback callback;
	LbrQueue queue;

	lbrCreateLinearAllocator(&alloc, 128);
	callback = lbrLinearAllocatorGetAllocCallback(&alloc);
	lbrCreateQueue(&queue, 128, 32, callback);

	for (int i = 0; i < 4; i++) {
		test t = {i, i, i, i};
		printf("%lu %lu %lu %lu\n", t.one, t.two, t.three, t.four);
		lbrQueuePush(&queue, &t);
	}

	printf("%lu\n", queue.length);

	test t;
	for (int i = 0; i < 4; i++) {
		lbrQueuePop(&queue, &t);
		printf("%lu %lu %lu %lu\n", t.one, t.two, t.three, t.four);
	}

	return 0;
}
