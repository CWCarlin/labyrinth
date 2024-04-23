#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_queue_t {
    LbrAllocInfo alloc_info;
    u8* data;
    usize front;
    usize back;
    usize size;
} LbrQueue;

void lbrCreateQueue(LbrQueue* p_queue, LbrAllocInfo alloc_info);
void lbrDestroyQueue(LbrQueue* p_queue);
void lbrQueuePush(LbrQueue* p_queue);
void* lbrQueuePop(LbrQueue* p_queue);
void lbrQueueClear(LbrQueue* p_queue);
