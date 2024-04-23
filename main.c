#include "allocators/alloc_info.h"
#include "allocators/block_alloc.h"
#include "data_structures/stack.h"

#include <stdio.h>

int main() {
	LbrBlockAllocator alloc;
	LbrStack stack;
	LbrAllocInfo info;

	lbrCreateBlockAllocator(&alloc, 16, 256);
	info = lbrBlockAllocatorGetAllocInfo(&alloc, 256);
	lbrCreateStack(&stack, info);
	
	for (int i = 0;; i++) {
		void* p = lbrStackAllocate(&stack, i);
		printf("%d : %p\n",i, p);
	}

	return 0;
}
