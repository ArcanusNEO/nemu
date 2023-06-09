#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB* current = NULL;

uintptr_t loader(_Protect* as, const char* filename);

void load_prog(const char* filename) {
  int i = nr_proc++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void)) entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void*) entry, NULL, NULL);
  // Log("%d", pcb[i].tf->irq);
}

_RegSet* schedule(_RegSet* prev) {
  current->tf = prev;

  // { static unsigned i = 0;
  //   ++i;
  //   if (i <= 99) current = pcb;
  //   else current = pcb + 1;

  //   if (i > 100) i = 0;
  // }
  // current = pcb + !!(current == pcb);
  // Log("0x%08x", current);
  current = pcb;

  _switch(&current->as);
  return current->tf;
}
