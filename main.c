#include <stdio.h>

#include "fabric/fabric.h"
#include "fabric/fiber.h"
#include "fabric/lock.h"
#include "utils/types.h"

LbrSpinLock lock = {100};

void test() {
  lbrSpinLockDecrement(&lock);

  printf("Hello Fiber\n");

  lbrFabricReturn();
}

int main() {
  lbrInitializeFabric();

  LbrTask tasks[100];

  for (int i = 0; i < 100; i++) {
    tasks[i].p_entry_point = (PFN_lbrEntryFunction)test;
  }

  lbrFabricQueueTasks(tasks, 100);

  while (lock.acquired) {
    printf("%d\n", lock.acquired);
  }

  printf("Done!\n");

  return 0;
}
