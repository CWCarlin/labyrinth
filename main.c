#include "data_structures/list.h"
#include "utils/types.h"

#include <stdio.h>

int main() {
	LbrDataList l;

	lbrCreateDataList(16, &l);
	lbrPushDataList(&l, (void*)0);
	void* i = lbrPopIndexDataList(&l, 0);

	// printf("%llu\n", (usize)i);

	return 0;
}
