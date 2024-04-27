#include "data_structures/list.h"

#include <string.h>
#include <stdio.h>
#include <vcruntime_string.h>

#include "allocators/alloc_info.h"
#include "utility/logging.h"
#include "utility/types.h"


void lbrCreateList(LbrList* p_list, usize capacity, usize type_size, LbrAllocCallback alloc_callback) {
    p_list->alloc_callback = alloc_callback;
    p_list->data = lbrAllocCallbackAllocate(&alloc_callback, capacity * (sizeof(LbrListNode) + type_size));
    p_list->capacity = capacity;
    p_list->type_size = type_size;
    lbrListClear(p_list);
}

void lbrDestroyList(LbrList* p_list) {
    lbrAllocCallbackFree(&p_list->alloc_callback, p_list->data);
    p_list->capacity = 0;
    p_list->type_size = 0;
    p_list->length = 0;
}

void lbrListPushBack(LbrList* p_list, void *p_data_in) {
    if (p_list->length == p_list->capacity) {
        LOG_ERROR("attempting to push data into full list");
    }
    
    LbrListNode* node = (LbrListNode*)p_list->next;
    p_list->next = (u64*)*p_list->next;
    if (!p_list->next) {
        p_list->next = (u64*)((u8*)node + sizeof(LbrListNode) + p_list->type_size);
    }

    if (!p_list->head) {
        p_list->head = node;
    }

    if (!p_list->tail) {
        p_list->tail = node;
    } else {
        node->prev = p_list->tail;
        p_list->tail->next = node;
        p_list->tail = node;
    }

    memcpy((u8*)node+sizeof(LbrListNode), p_data_in, p_list->type_size);

    p_list->length++;
}

void* lbrListAt(LbrList* p_list, usize idx) {
    if (idx > p_list->capacity) {
        LOG_ERROR("attempting to get an index that doesn't exist from list");
    }
    LbrListNode* p = p_list->head;
    for (usize i = 0; i < idx; i++) {
        p = p->next;
    }

    return (u8*)p + sizeof(LbrListNode);
}

void lbrListRemove(LbrList* p_list, void* block) {
    u8* p = (u8*)block - sizeof(LbrListNode);

    if (p < p_list->data || p > p_list->data + p_list->capacity * ((sizeof(LbrListNode) + p_list->type_size))) {
        LOG_ERROR("attempting to remove pointer from a list that does not own it");
    }

    if ((p - p_list->data) % (sizeof(LbrListNode) + p_list->type_size)) {
        LOG_ERROR("attempting to remove pointer that is not aligned with data");
    }

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
    
    u64* n = (u64*)node;
    *n = (u64)p_list->next;
    p_list->next = n;
    p_list->length--;
}

void lbrListClear(LbrList* p_list) {
    memset(p_list->data, 0, p_list->capacity * (sizeof(LbrListNode) + p_list->type_size));
    p_list->length = 0;
    p_list->head = NULL;
    p_list->tail = NULL;
    p_list->next = (u64*)p_list->data;
}
