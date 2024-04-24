#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_queue_t {
    LbrAllocCallback alloc_callback;
    u8* data;
    usize capacity;
    usize type_size;
    usize front;
    usize back;
} LbrQueue;

void lbrCreateQueue(LbrQueue* p_queue, usize capacity, usize type_size, LbrAllocCallback alloc_callback);
void lbrDestroyQueue(LbrQueue* p_queue);
void lbrQueuePush(LbrQueue* p_queue, void* data);
void lbrQueuePop(LbrQueue* p_queue, void* data);
void lbrQueueClear(LbrQueue* p_queue);
