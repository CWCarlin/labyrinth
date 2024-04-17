#pragma once
#include "utils/types.h"

typedef struct {
	void** data;
	usize top;
	usize size;
} LbrDataStack;

void lbrCreateDataStack(usize size, LbrDataStack* stack);
void lbrDestroyDataStack(LbrDataStack* stack);
void lbrPushDataStack(LbrDataStack* stack, void* data);
void* lbrPopDataStack(LbrDataStack* stack);
LbrBool lbrIsDataStackEmpty(const LbrDataStack* stack);
