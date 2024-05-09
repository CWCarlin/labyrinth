#include <stdio.h>

#include "fabric/fabric.h"
#include "utils/types.h"

LbrFiber c[2];

int j = 0;

void foo() {
  printf("foooo\n");
  j--;
  lbrFabricReturn();
}

int main() {
  lbrInitializeFabric();

  LbrTask tasks[100];

  for (int i = 0; i < 100; i++) {
    tasks[i].entry_point = foo;
    j++;
  }

  lbrFabricQueueTasks(tasks, 100);

  while (j > 0) {
  }

  printf("huh?\n");

  return 0;
}
