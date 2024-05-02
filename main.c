#include <stdio.h>
#include <vulkan/vulkan.h>

#include "allocators/alloc_info.h"
#include "allocators/linear_alloc.h"
#include "data_structures/hash.h"
#include "data_structures/map.h"

typedef struct {
  int one, two;
} test;

usize inthash(int* i) { return *i; }

u8 intequals(int* a, int* b) { return *a == *b; }

int main() {
  LbrLinearAllocator alloc;
  lbrCreateLinearAllocator(&alloc, 1024);
  LbrAllocCallback callback = lbrLinearAllocatorGetAllocCallback(&alloc);

  LbrMap map;
  lbrCreateMap(&map, 16, sizeof(int), sizeof(test), callback,
               (PFN_lbrHashFunc)&inthash, (PFN_lbrEqualityFunc)&intequals);

  int i = 1;
  test t = {1, 1};

  lbrMapInsert(&map, &i, &t);

  printf("%llu\n", map.length);

  i = 1;
  test t_2 = *(test*)lbrMapGetValue(&map, &i);
  printf("%d %d\n", t_2, t_2.two);

  return 0;
}
