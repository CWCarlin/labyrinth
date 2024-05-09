#include <stdio.h>

#include "fabric/fabric.h"
#include "fabric/lock.h"
#include "utils/types.h"

LbrFiber c[2];

static volatile LbrSpinLock lock = {100};

void foo() {
  lock.acquired--;
  printf("fooo %d\n", lock.acquired);

  lbrFabricReturn();
}

int main() {
  lbrInitializeFabric();

  LbrTask tasks[100];

  for (int i = 0; i < 100; i++) {
    tasks[i].entry_point = (PFN_lbrEntryFunction)foo;
  }

  lbrFabricQueueTasks(tasks, 100);

  while (lock.acquired) {
    // printf("%d\n", lock.acquired);
  }

  printf("huh?\n");

  return 0;
}
