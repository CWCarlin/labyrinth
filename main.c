#include <stdio.h>

#include "fabric/fabric.h"
#include "fabric/synchronize.h"

void foo() { printf("foo\n"); }

void baz(const usize* i) {
  LbrTask new_tasks[10];
  for (usize i = 0; i < 10; i++) {
    new_tasks[i].pfn_task = foo;
  }

  LbrSemaphore* p_sem = NULL;
  lbrFabricQueueTasks(new_tasks, 10, &p_sem);
  lbrFabricWaitForSemaphore(p_sem, 0);
  lbrFabricFreeSemaphore(&p_sem);
  printf("heee %zu\n", *i);
}

int main() {
  lbrInitializeFabric();

  LbrTask tasks[10];
  usize in[10];
  for (usize i = 0; i < 10; i++) {
    in[i]             = i;
    tasks[i].pfn_task = (PFN_lbrTaskFunction)baz;
    tasks[i].p_input  = &in[i];
  }

  LbrSemaphore* semm;

  lbrFabricQueueTasks(tasks, 10, &semm);

  while (semm->count != 0) {
  }

  printf("working!\n");

  for (;;) {
  }

  return 0;
}
