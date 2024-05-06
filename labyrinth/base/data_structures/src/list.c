#include "data_structures/list.h"

#include <string.h>

#include "allocators/alloc_types.h"
#include "utils/logging.h"

void lbrCreateList(LbrListCreateInfo* p_info, LbrList* p_list) {
  usize capacity  = p_info->capacity;
  usize type_size = p_info->type_size;

  p_list->alloc_callbacks = p_info->alloc_callbacks;
  p_list->data            = lbrAllocCallbacksAllocate(&p_info->alloc_callbacks, capacity * (sizeof(LbrListNode) + type_size));
  p_list->capacity        = capacity;
  p_list->type_size       = type_size;
  lbrListClear(p_list);
}

void lbrDestroyList(LbrList* p_list) {
  lbrAllocCallbacksFree(&p_list->alloc_callbacks, p_list->data);
  p_list->capacity  = 0;
  p_list->type_size = 0;
  p_list->length    = 0;
}

void lbrListPushBack(LbrList* p_list, void* p_data_in) {
  LBR_ASSERT(p_list->length != p_list->capacity);

  LbrListNode* node = (LbrListNode*)p_list->next;
  p_list->next      = (u8**)*p_list->next;

  if (!p_list->next) {
    p_list->next = (u8**)((u8*)(node + 1) + p_list->type_size);
  }

  if (!p_list->head) {
    p_list->head = node;
  }

  if (!p_list->tail) {
    p_list->tail = node;
  } else {
    node->prev         = p_list->tail;
    p_list->tail->next = node;
    p_list->tail       = node;
  }

  memcpy(node + 1, p_data_in, p_list->type_size);
  p_list->length++;
}

void* lbrListAt(LbrList* p_list, usize idx) {
  LBR_ASSERT(idx <= p_list->capacity);
  LbrListNode* p = p_list->head;
  for (usize i = 0; i < idx; i++) {
    p = p->next;
  }

  return p + 1;
}

void lbrListRemove(LbrList* p_list, void* block) {
  u8* p = (u8*)block - sizeof(LbrListNode);
  LBR_ASSERT(p >= p_list->data);
  LBR_ASSERT(p < p_list->data + p_list->capacity * ((sizeof(LbrList) + p_list->type_size)));
  LBR_ASSERT((p - p_list->data) % (sizeof(LbrListNode) + p_list->type_size) == 0)

  LbrListNode* node = (LbrListNode*)p;
  if (node == p_list->head) {
    p_list->head = node->next;
  } else {
    node->prev->next = node->next;
  }

  if (node == p_list->tail) {
    p_list->tail = node->prev;
  } else {
    node->next->prev = node->prev;
  }

  u8** n       = (u8**)node;
  *n           = (u8*)p_list->next;
  p_list->next = n;
  p_list->length--;
}

void lbrListClear(LbrList* p_list) {
  memset(p_list->data, 0, p_list->capacity * (sizeof(LbrListNode) + p_list->type_size));
  p_list->length = 0;
  p_list->head   = NULL;
  p_list->tail   = NULL;
  p_list->next   = (u8**)p_list->data;
}
