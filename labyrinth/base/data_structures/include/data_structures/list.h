#pragma once

#include "allocators/alloc_types.h"
#include "utils/types.h"

typedef struct lbr_list_node_t {
  struct lbr_list_node_t* prev;
  struct lbr_list_node_t* next;
} LbrListNode;

typedef struct lbr_list_t {
  LbrAllocCallbacks alloc_callbacks;
  u8* data;
  usize capacity;
  usize type_size;
  usize length;
  LbrListNode* head;
  LbrListNode* tail;
  u8** next;
} LbrList;

typedef struct lbr_list_create_info_t {
  usize capacity;
  usize type_size;
  LbrAllocCallbacks alloc_callbacks;
} LbrListCreateInfo;

void lbrDefineList(LbrListCreateInfo* p_info, LbrList* p_list);
void lbrDestroyList(LbrList* p_list);
void lbrListPushBack(LbrList* p_list, void* p_data_in);
void* lbrListAt(LbrList* p_list, usize idx);
void lbrListRemove(LbrList* p_list, void* block);
void lbrListClear(LbrList* p_list);
