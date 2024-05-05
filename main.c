#include <stdio.h>

#include "allocators/block_alloc.h"
#include "data_structures/list.h"
#include "utils/types.h"

int main() {
  LbrBlockAllocator alloc;
  LbrBlockAllocatorCreateInfo alloc_info;
  alloc_info.block_size = sizeof(int) * 200;
  alloc_info.num_blocks = 16;
  alloc_info.alignment  = sizeof(int);
  lbrCreateBlockAllocator(&alloc_info, &alloc);

  LbrList list;
  LbrListCreateInfo list_info;
  list_info.alloc_callbacks = alloc.lbr_callbacks;
  list_info.capacity        = 32;
  list_info.type_size       = sizeof(int);

  lbrCreateList(&list_info, &list);

  for (int i = 0; i < 10; i++) {
    lbrListPushBack(&list, &i);
  }

  int* j[16];
  for (usize i = 0; i < 10; i++) {
    j[i] = (int*)lbrListAt(&list, i);
    printf("%d ", *j[i]);
  }
  printf("\n");
  lbrListRemove(&list, j[3]);
  lbrListRemove(&list, j[8]);

  for (usize i = 0; i < 8; i++) {
    printf("%d ", *(int*)lbrListAt(&list, i));
  }
  printf("\n");

  int t = 3;
  lbrListPushBack(&list, &t);

  for (usize i = 0; i < 9; i++) {
    printf("%d ", *(int*)lbrListAt(&list, i));
  }

  lbrDestroyList(&list);

  return 0;
}
