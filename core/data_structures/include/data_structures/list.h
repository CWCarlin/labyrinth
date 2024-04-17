#pragma once
#include "utils/types.h"
#include "data_structures/stack.h"

typedef struct LbrDataListNode LbrDataListNode;

typedef struct {
	LbrDataStack open_nodes;
	LbrDataListNode* nodes;
	LbrDataListNode* head;
	usize size;
	usize length;
} LbrDataList;

void lbrCreateDataList(usize size, LbrDataList* list);
void lbrDestroyDataList(LbrDataList* list);
void lbrPushDataList(LbrDataList* list, void* data);
void* lbrPopIndexDataList(LbrDataList* list, usize idx);
