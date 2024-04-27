#include "allocators/block_alloc.h"

#include <stdio.h>


void* t[5];

int main() {
	LbrBlockAllocator alloc;
	lbrCreateBlockAllocator(&alloc, 16, 32);

	for (int i = 0; i < 4; i++) {
		t[i] = lbrBlockAllocatorAllocate(&alloc);
		printf("%d : %p\n", i, t[i]);
	}

	lbrBlockAllocatorFree(&alloc, t[3]);
	lbrBlockAllocatorFree(&alloc, t[1]);

	void* one = lbrBlockAllocatorAllocate(&alloc);
	void* two = lbrBlockAllocatorAllocate(&alloc);
	void* three = lbrBlockAllocatorAllocate(&alloc);

	printf("%p\n", t[0]);
	printf("%p\n", t[2]);
	printf("%p\n", one);
	printf("%p\n", two);
	printf("%p\n", three);

	return 0;
}
