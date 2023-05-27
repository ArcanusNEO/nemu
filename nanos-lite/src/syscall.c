#include "common.h"
#include "syscall.h"

#include "fs.h"

#include "memory.h"

_RegSet* do_syscall(_RegSet* r) {
  uintptr_t a[4] = {
    SYSCALL_ARG1(r), SYSCALL_ARG2(r), SYSCALL_ARG3(r), SYSCALL_ARG4(r)};

  switch (a[0]) {
    case SYS_none : SYSCALL_ARG1(r) = 1; break;
    case SYS_exit : _halt(a[1]); break;
    case SYS_brk : SYSCALL_ARG1(r) = mm_brk(a[1]); break;
    case SYS_write :
      SYSCALL_ARG1(r) = fs_write(a[1], (const void*) a[2], a[3]);
      break;
    case SYS_read : SYSCALL_ARG1(r) = fs_read(a[1], (void*) a[2], a[3]); break;
    case SYS_open :
      SYSCALL_ARG1(r) = fs_open((const char*) a[1], a[2], a[3]);
      break;
    case SYS_close : SYSCALL_ARG1(r) = fs_close(a[1]); break;
    case SYS_lseek : SYSCALL_ARG1(r) = fs_lseek(a[1], a[2], a[3]); break;
    default : panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
