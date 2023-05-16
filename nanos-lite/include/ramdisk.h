#ifndef _RAMDISK_H_
#define _RAMDISK_H_
#include "common.h"

void ramdisk_read(void* buf, off_t offset, size_t len);
void ramdisk_write(const void* buf, off_t offset, size_t len);
void init_ramdisk();
size_t get_ramdisk_size();

#endif  // _RAMDISK_H_