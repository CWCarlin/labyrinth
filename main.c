#include <pthread.h>
#include <stdio.h>

#include "fabric/fabric.h"
#include "fabric/fiber.h"
#include "fabric/lock.h"

LbrSpinLock lock = {100};

void test(usize* i) {
  *i = pthread_self();
  lbrSpinLockDecrement(&lock);

  lbrFabricReturn();
}

int main() {
  lbrInitializeFabric();

  LbrTask tasks[100];
  usize izes[100];

  for (int i = 0; i < 100; i++) {
    tasks[i].pfn_entry_point = (PFN_lbrFiberEntryFunction)test;
    tasks[i].p_data_in       = &izes[i];
  }

  lbrFabricQueueTasks(tasks, 100, HIGH);

  while (lock.acquired) {
  }

  for (int i = 0; i < 100; i++) {
    printf("%d ", izes[i]);
  }

  return 0;
}
