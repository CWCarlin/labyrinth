#include "data_structures/queue.h"

#include <string.h> // IWYU pragma: keep

#include "allocators/alloc_info.h"
#include "utility/logging.h"

void lbrCreateQueue(LbrQueue* p_queue, usize capacity, usize type_size, LbrAllocCallback alloc_callback) {
	p_queue->alloc_callback = alloc_callback;
	p_queue->data = lbrAllocCallbackAllocate(&alloc_callback, capacity);
	p_queue->capacity = capacity;
	p_queue->type_size = type_size;
	lbrQueueClear(p_queue);
}

void lbrDestroyQueue(LbrQueue* p_queue) {
	lbrAllocCallbackFree(&p_queue->alloc_callback, p_queue->data);
	p_queue->capacity = 0;
	p_queue->type_size = 0;
	p_queue->length = 0;
	p_queue->front = 0;
	p_queue->back = 0;
}

void lbrQueuePush(LbrQueue* p_queue, void* p_data_in) {
	if (p_queue->length == p_queue->capacity) {
		LOG_ERROR("attempting to push data to full queue");
	}
	p_queue->length++;
	memcpy(p_queue->data + (p_queue->back * p_queue->type_size), p_data_in, p_queue->type_size);
	p_queue->back++;
	if (p_queue->back == p_queue->capacity) {
		p_queue->back = 0;
	}
}

void lbrQueuePop(LbrQueue* p_queue, void* p_data_out) {
	if (p_queue->length == 0) {
		LOG_ERROR("attempting to pop data from an empty queue");
	}
	p_queue->length--;
	memcpy(p_data_out, p_queue->data + (p_queue->front * p_queue->type_size), p_queue->type_size);
	p_queue->front++;
	if (p_queue->front == p_queue->capacity) {
		p_queue->front = 0;
	}
}

void lbrQueueClear(LbrQueue* p_queue) {
	p_queue->front = 0;
	p_queue->back = 0;
	p_queue->length = 0;
}
