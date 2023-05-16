#include "common.h"
#include "syscall.h"

_RegSet* do_syscall(_RegSet* r) {
  uintptr_t a[4] = {
    SYSCALL_ARG1(r), SYSCALL_ARG2(r), SYSCALL_ARG3(r), SYSCALL_ARG4(r)};

  switch (a[0]) {
    
    default : panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
