#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_list_node_t {
    u8* data;
    struct lbr_list_node_t* next;
    struct lbr_list_node_t* prev;
} LbrListNode;

typedef struct lbr_list_t {
    LbrAllocCallback alloc_callback;
    u8* data;
    usize capacity;
    usize type_size;

    LbrListNode* nodes;
    LbrListNode* head;
    LbrListNode* tail;
    LbrListNode* next;
} LbrList;

void lbrCreateList(LbrList* p_list, usize capacity, usize type_size, LbrAllocCallback alloc_callback);
void lbrDestroyList(LbrList* p_list);
void lbrListPush(LbrList* p_list, void* p_data_in, usize idx);
void lbrListPushBack(LbrList* p_list, void* p_data_in);
void lbrListRemove(LbrList* p_list, usize idx);
void* lbrListGet(LbrList* p_list, usize idx);
void lbrListClear(LbrList* p_list);
