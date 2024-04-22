#include "allocators/block_alloc.h"
#include "utility/types.h"

#include <stdio.h>

u64 test[15];

int main() {
	LbrBlockAllocator alloc;
	lbrCreateBlockAllocator(&alloc, 16, 16);

	for (int i = 0; i < 15; i++) {
		void* p = lbrBlockAllocatorAllocate(&alloc);
		printf("%d: %p\n", i, p);
		test[i] = (u64)p;
	}

	printf("\n");
	lbrBlockAllocatorFree(&alloc, (void*)test[3]);
	lbrBlockAllocatorFree(&alloc, (void*)test[7]);
	lbrBlockAllocatorFree(&alloc, (void*)test[1]);
	lbrBlockAllocatorFree(&alloc, (void*)test[0]);

	void* p = lbrBlockAllocatorAllocate(&alloc);
	printf("%d: %p\n", 0, p);
	p = lbrBlockAllocatorAllocate(&alloc);
	printf("%d: %p\n", 1, p);
	p = lbrBlockAllocatorAllocate(&alloc);
	printf("%d: %p\n", 7, p);
	p = lbrBlockAllocatorAllocate(&alloc);
	printf("%d: %p\n", 3, p);
}
