#pragma once
#include "utils/types.h"

typedef struct {
	void** data;
	usize size;
	usize front;
	usize back;
} LbrDataQueue;

void lbrCreateDataQueue(usize size, LbrDataQueue* queue);
void lbrDestroyDataQueue(LbrDataQueue* queue);
void lbrPushDataQueue(LbrDataQueue* queue, void* data);
void* lbrPopDataQueue(LbrDataQueue* queue);
LbrBool lbrIsDataQueueEmpty(LbrDataQueue* queue);
