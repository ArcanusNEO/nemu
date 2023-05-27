#include "common.h"
#include "ramdisk.h"

#include "fs.h"

#include "memory.h"

#include "am.h"

#define DEFAULT_ENTRY ((void*) 0x8048000)

uintptr_t loader(_Protect* as, const char* filename) {
  int fd = fs_open(filename, 0, 0);
  size_t f_size = fs_filesz(fd);

  Log("loaded: [%d]%s size: %d", fd, filename, f_size);

  void* mend = DEFAULT_ENTRY + f_size;
  for (void* va = DEFAULT_ENTRY; va < mend; va += PGSIZE) {
    void* pa = new_page();
    // Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
    _map(as, va, pa);
    fs_read(fd, pa, min(mend - va, PGSIZE));
  }

  fs_close(fd);
  return (uintptr_t) DEFAULT_ENTRY;
}
