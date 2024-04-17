#include "data_structures/list.h"

#include "data_structures/stack.h"
#include "utils/logging.h"
#include "utils/types.h"

#include <stdlib.h>

#include <stdio.h>

struct LbrDataListNode {
	void* data;
	LbrDataListNode* next;
	usize index;
};

void lbrCreateDataList(usize size, LbrDataList* list) {
	lbrCreateDataStack(size, &list->open_nodes);
	list->nodes = malloc(sizeof(LbrDataListNode) * size);
	list->head = malloc(sizeof(LbrDataListNode) * (size + 1));
	list->size = size;
	list->length = 0;

	for (usize i = 0; i < size; i++) {
		lbrPushDataStack(&list->open_nodes, (void*)i);
	}
}

void lbrDestroyDataList(LbrDataList* list) {
	LbrDataListNode* node = list->head->next;
	while(node != NULL) {
		free(node->data);
		node = node->next;
	}
	list->size = 0;
	list->length = 0;
	free(list->head);
	free(list->nodes);
	lbrDestroyDataStack(&list->open_nodes);
}

void lbrPushDataList(LbrDataList* list, void* data) {
	if (list->size == list->length) {
		LOG_ERROR("pushing to a full datalist");
	}
	LbrDataListNode* node = &list->head[list->length];
	usize idx = (usize)lbrPopDataStack(&list->open_nodes);
	node->next = &list->nodes[idx];
	node->next->data = data;
	node->next->index = idx;

	list->length++;
}

void* lbrPopIndexDataList(LbrDataList* list, usize idx) {
	if (idx >= list->length) {
		LOG_ERROR("trying to pop an invalid index from a datalist");
	}
	LbrDataListNode* node = &list->head[idx+1];
	printf("%llu\n", (usize)node->data);
	lbrPushDataStack(&list->open_nodes, (void*)node->index);
	node->next = NULL;
	list->length--;

	if (idx == list->length) {
		list->head[idx].next = NULL;
	} else {
		list->head[idx].next = &list->head[idx + 2];
	}

	return node->data;
}
