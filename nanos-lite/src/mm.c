#include "memory.h"
#include "proc.h"

static void* pf = NULL;

void* new_page(void) {
  assert(pf < (void*) _heap.end);
  void* p = pf;
  pf += PGSIZE;
  return p;
}

void free_page(void* p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uint32_t new_brk) {
  // Log("New brk: 0x%08x", new_brk);
  if (current->cur_brk == 0) current->max_brk = new_brk;
  else if (new_brk > current->max_brk) {
    uintptr_t va;
    for (va = (current->max_brk + PGMASK) & ~PGMASK; va < new_brk;
         va += PGSIZE) {
      void* pa = new_page();
      // Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
      _map(&current->as, (void*) va, pa);
    }
    current->max_brk = va;
  }
  current->cur_brk = new_brk;
  return 0;
}

void init_mm() {
  pf = (void*) PGROUNDUP((uintptr_t) _heap.start);
  Log("free physical pages starting from %p", pf);

  _pte_init(new_page, free_page);
}
