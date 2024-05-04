#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_list_node_t {
    struct lbr_list_node_t* prev;
    struct lbr_list_node_t* next;
} LbrListNode;

typedef struct lbr_list_t {
    LbrAllocCallback alloc_callback;
    u8* data;
    usize capacity;
    usize type_size;
    usize length;

    LbrListNode* head;
    LbrListNode* tail;
    u64* next;
} LbrList;

void lbrCreateList(LbrList* p_list, usize capacity, usize type_size, LbrAllocCallback alloc_callback);
void lbrDestroyList(LbrList* p_list);
void lbrListPushBack(LbrList* p_list, void* p_data_in);
void* lbrListAt(LbrList* p_list, usize idx);
void lbrListRemove(LbrList* p_list, void* block);
void lbrListClear(LbrList* p_list);
