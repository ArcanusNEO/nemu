#include "common.h"
#include "ramdisk.h"

#include "fs.h"

#define DEFAULT_ENTRY ((void*) 0x8048000)

uintptr_t loader(_Protect* as, const char* filename) {
  int fd = fs_open(filename, 0, 0);
  size_t f_size = fs_filesz(fd);
  fs_read(fd, DEFAULT_ENTRY, f_size);
  fs_close(fd);
  return (uintptr_t) DEFAULT_ENTRY;
}
