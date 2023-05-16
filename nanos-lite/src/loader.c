#include "common.h"
#include "ramdisk.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  size_t sz = get_ramdisk_size();
  ramdisk_read(DEFAULT_ENTRY, 0, sz);
  return (uintptr_t) DEFAULT_ENTRY;
}
