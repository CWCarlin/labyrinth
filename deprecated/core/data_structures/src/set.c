#include "data_structures/set.h"

#include <string.h> // IWYU pragma: keep

#include "allocators/alloc_info.h"
#include "utility/logging.h"

void lbrCreateSet(LbrSet *p_set, usize capacity, usize type_size,
	LbrAllocCallback alloc_callback, PFN_lbrHashFunc hash, PFN_lbrEqualityFunc equals) {
	p_set->alloc_callback = alloc_callback;
	p_set->data = lbrAllocCallbackAllocate(&alloc_callback, capacity * type_size);
	p_set->capacity = capacity;
	p_set->type_size = type_size;
	p_set->hash = hash;
	p_set->equals = equals;
	lbrSetClear(p_set);
}

void lbrDestroySet(LbrSet *p_set) {
	lbrAllocCallbackFree(&p_set->alloc_callback, p_set->data);
	p_set->capacity = 0;
	p_set->type_size = 0;
	p_set->length = 0;
}

void lbrSetInsert(LbrSet *p_set, void *p_data_in) {
	if (p_set->length == p_set->capacity) {
		LOG_ERROR("attempting to insert into a full set");
	}
	usize idx = p_set->hash(p_data_in) % p_set->capacity;
	while (!p_set->equals(p_data_in, p_set->data + (idx * p_set->type_size))) {

		u8 open_block = 1;
		for (usize i = 0; i < p_set->type_size; i++) {
			if (*(p_set->data + (idx * p_set->type_size) + i) != 0) {
				open_block = 0;
				break;
			}
		}

		if (open_block) {
			memcpy(p_set->data + (idx * p_set->type_size), p_data_in, p_set->type_size);
			p_set->length++;
			break;
		}

		idx = (idx + 1) % p_set->capacity;
	}
}

void lbrSetRemove(LbrSet* p_set, void *p_data) {
	usize idx = p_set->hash(p_data) % p_set->capacity;
	while (!p_set->equals(p_data, p_set->data + (idx * p_set->type_size))) {
		u8 open_block = 1;
		for (usize i = 0; i < p_set->type_size; i++) {
			if (*(p_set->data + (idx * p_set->type_size) + i) != 0) {
				open_block = 0;
				break;
			}
		}
		if (open_block) {
			break;
		}
		idx = (idx + 1) % p_set->capacity;
	}

	if (p_set->equals(p_data, p_set->data + (idx * p_set->type_size))) {
		memset(p_set->data + (idx * p_set->type_size), 0, p_set->type_size);
		p_set->length--;
	}
}

u8 lbrSetContains(LbrSet *p_set, void *p_data) {
	usize idx = p_set->hash(p_data) % p_set->capacity;
	while (!p_set->equals(p_data, p_set->data + (idx * p_set->type_size))) {
		u8 open_block = 1;
		for (usize i = 0; i < p_set->type_size; i++) {
			if (*(p_set->data + (idx * p_set->type_size) + i) != 0) {
				open_block = 0;
				break;
			}
		}
		if (open_block) {
			break;
		}
		idx = (idx + 1) % p_set->capacity;
	}

	if (p_set->equals(p_data, p_set->data + (idx * p_set->type_size))) {
		return 1;
	}

	return 0;
}

void lbrSetClear(LbrSet *p_set) {
	memset(p_set->data, 0, p_set->capacity * p_set->type_size);
	p_set->length = 0;
}
