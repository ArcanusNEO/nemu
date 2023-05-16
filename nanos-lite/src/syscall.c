#include "common.h"
#include "syscall.h"

_RegSet* do_syscall(_RegSet* r) {
  uintptr_t a[4] = {
    SYSCALL_ARG1(r), SYSCALL_ARG2(r), SYSCALL_ARG3(r), SYSCALL_ARG4(r)};

  switch (a[0]) {
    case SYS_none : SYSCALL_ARG1(r) = 1; break;
    case SYS_exit : _halt(a[1]); break;
    case SYS_brk : SYSCALL_ARG1(r) = 0; break;
    // case SYS_write : SYSCALL_ARG1(r) = sys_write(a[1], a[2], a[3]); break;
    default : panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
