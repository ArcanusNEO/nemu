#ifndef _DEVICE_H_
#define _DEVICE_H_
#include "common.h"

size_t events_read(void* buf, size_t len);
void dispinfo_read(void* buf, off_t offset, size_t len);
void fb_read(void* buf, off_t offset, size_t len);
void fb_write(const void* buf, off_t offset, size_t len);
void init_device();

#endif  // _DEVICE_H_