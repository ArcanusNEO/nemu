#ifndef __FS_H__
#define __FS_H__

#include "common.h"

enum {SEEK_SET, SEEK_CUR, SEEK_END};

ssize_t fs_write(uintptr_t fd, uint8_t* buf, size_t len);

#endif
