#include "data_structures/queue.h"

#include <string.h>

#include "allocators/alloc_types.h"
#include "utils/logging.h"

void lbrCreateQueue(LbrQueueCreateInfo* p_info, LbrQueue* p_queue) {
  usize capacity  = p_info->capacity;
  usize type_size = p_info->type_size;

  p_queue->alloc_callbacks = p_info->alloc_callbacks;
  p_queue->data            = lbrAllocCallbacksAllocate(&p_info->alloc_callbacks, capacity * type_size);
  p_queue->capacity        = capacity;
  p_queue->type_size       = type_size;
  lbrQueueClear(p_queue);
}

void lbrDestroyQueue(LbrQueue* p_queue) {
  lbrAllocCallbacksFree(&p_queue->alloc_callbacks, p_queue->data);
  p_queue->capacity  = 0;
  p_queue->type_size = 0;
  p_queue->length    = 0;
  p_queue->front     = 0;
  p_queue->back      = 0;
}

void lbrQueuePush(LbrQueue* p_queue, void* p_data_in) {
  LBR_ASSERT(p_queue->length != p_queue->capacity);
  p_queue->length++;
  memcpy(p_queue->data + (p_queue->back * p_queue->type_size), p_data_in, p_queue->type_size);
  p_queue->back++;
  if (p_queue->back == p_queue->capacity) {
    p_queue->back = 0;
  };
}

void lbrQueuePop(LbrQueue* p_queue, void* p_data_out) {
  LBR_ASSERT(p_queue->length != 0);
  p_queue->length--;
  memcpy(p_data_out, p_queue->data + (p_queue->front * p_queue->type_size), p_queue->type_size);
  p_queue->front++;

  if (p_queue->front == p_queue->capacity) {
    p_queue->front = 0;
  }
}

void lbrQueueClear(LbrQueue* p_queue) {
  p_queue->front  = 0;
  p_queue->back   = 0;
  p_queue->length = 0;
}
