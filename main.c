#include <stdio.h>

#include "fabric/fabric.h"
#include "fabric/synchronize.h"

void foo() { printf("foo\n"); }

void baz() {
  LbrTask new_tasks[10];

  for (usize i = 0; i < 10; i++) {
    new_tasks[i].pfn_task = foo;
  }

  LbrSemaphore* p_sem = NULL;
  lbrFabricQueueTasks(new_tasks, 10, &p_sem);
  lbrFabricWaitForSemaphore(p_sem, 0);
  lbrFabricFreeSemaphore(&p_sem);
  printf("heee\n");
}

int main() {
  lbrInitializeFabric();

  LbrTask task;
  task.pfn_task = baz;

  LbrSemaphore* semm;

  lbrFabricQueueTasks(&task, 1, &semm);

  printf("%p\n", semm);

  while (semm->count != 0) {
  }

  printf("working!\n");

  return 0;
}
