#include "data_structures/list.h"
#include "allocators/alloc_info.h"
#include "utility/types.h"
#include <string.h>

void lbrCreateList(LbrList* p_list, usize capacity, usize type_size, LbrAllocCallback alloc_callback) {
    u8* block = lbrAllocCallbackAllocate(&alloc_callback, 
    capacity * (type_size + sizeof(LbrListNode)));
    p_list->alloc_callback = alloc_callback;
    p_list->data = block;
    p_list->capacity = capacity;
    p_list->type_size = type_size;
    p_list->nodes = (LbrListNode*)(block + capacity * type_size);
    p_list->head = NULL;
    p_list->tail = NULL;
    p_list->next = p_list->nodes;

    for (usize i = 0; i < capacity; i++) {
        p_list->nodes[i].data = p_list->data + (i * type_size);
    }
}

void lbrDestroyList(LbrList* p_list) {

}

void lbrListPush(LbrList* p_list, void* p_data_in, usize idx) {

}

void lbrListPushBack(LbrList* p_list, void *p_data_in) {

}

void lbrListRemove(LbrList* p_list, usize idx) {

}

void* lbrListGet(LbrList* p_list, usize idx) {

}

void lbrListClear(LbrList* p_list) {

}
