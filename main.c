#include <stdio.h>
#include <stdlib.h>

#include "fabric/fiber.h"
#include "utils/types.h"

LbrRegisterContext c[2];

void foo() {
  printf("foooo\n");

  lbrRegisterSwapContext(&c[1], &c[0]);
}

int main() {
  u8 data[4096];
  u8* sp = data + sizeof data;
  sp     = (u8*)((uintptr)sp & -16);  // NOLINT
  sp -= 128;

  LbrRegisterContext* cc = calloc(1, sizeof(LbrRegisterContext));

  lbrRegisterGetContext(&c[0]);

  cc->rip = (uintptr*)foo;
  cc->rsp = (uintptr*)sp;

  c[1] = *cc;

  lbrRegisterSwapContext(&c[0], &c[1]);

  printf("huh?\n");

  return 0;
}
