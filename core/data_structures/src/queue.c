#include "data_structures/queue.h"

#include "utils/logging.h"
#include "utils/types.h"

#include <stdlib.h>	// IWYU pragma: keep

void lbrCreateDataQueue(usize size, LbrDataQueue* queue) {
	queue->data = malloc(sizeof(void*) * size);
	queue->front = 0;
	queue->back = 0;
	queue->size = size;
}

void lbrDestroyDataQueue(LbrDataQueue* queue) {
	for (usize i = queue->back; i >= queue->front; i--) {
		free(queue->data[i]);
	}
	free(queue->data);
	queue->size = 0;
	queue->front = 0;
	queue->back = 0;
}

void lbrPushDataQueue(LbrDataQueue* queue, void* data) {
	if ((queue->back + 1) % queue->size == queue->front) {
		LOG_ERROR("pushing to a full dataqueue");
	}
	queue->data[queue->back] = data;
	queue->back = (queue->back + 1) % queue->size;
}

void* lbrPopDataQueue(LbrDataQueue* queue) {
	if (lbrIsDataQueueEmpty(queue)) {
		LOG_ERROR("popping from an empty dataqueue");
	}
	void* data = queue->data[queue->front];
	queue->front = (queue->front + 1) % queue->size;
	return data;
}

LbrBool lbrIsDataQueueEmpty(LbrDataQueue* queue) {
	if (queue->front == queue->back) return LBR_TRUE;
	return LBR_FALSE;
}
