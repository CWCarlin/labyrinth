#pragma once

#include "allocators/alloc_types.h"
#include "utils/types.h"

typedef struct lbr_queue_t {
  LbrAllocCallbacks alloc_callbacks;
  u8* data;
  usize capacity;
  usize type_size;
  usize length;
  usize front;
  usize back;
} LbrQueue;

typedef struct lbr_queue_create_info_t {
  usize capacity;
  usize type_size;
  LbrAllocCallbacks alloc_callbacks;
} LbrQueueCreateInfo;

void lbrCreateQueue(LbrQueueCreateInfo* p_info, LbrQueue* p_queue);
void lbrDestroyQueue(LbrQueue* p_queue);
void lbrQueuePush(LbrQueue* p_queue, void* p_data_in);
void lbrQueuePop(LbrQueue* p_queue, void* p_data_out);
void lbrQueueClear(LbrQueue* p_queue);
