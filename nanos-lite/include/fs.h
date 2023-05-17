#ifndef __FS_H__
#define __FS_H__

#include "common.h"

// enum {SEEK_SET, SEEK_CUR, SEEK_END};
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 3

int fs_open(const char* pathname, int flags, int mode);
ssize_t fs_read(int fd, void* buf, size_t len);
ssize_t fs_write(int fd, const void* buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);
int fs_close(int fd);
size_t fs_filesz(int fd);

#endif
