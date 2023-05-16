#include "fs.h"

#include "ramdisk.h"

typedef struct {
  char* name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB,
  FD_EVENTS,
  FD_DISPINFO,
  FD_NORMAL
};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {  "stdin (note that this is not the actual stdin)",   1, 0, 0},
  {"stdout (note that this is not the actual stdout)",   1, 0, 0},
  {"stderr (note that this is not the actual stderr)",   1, 0, 0},
  [FD_FB] = {                                         "/dev/fb",   1, 0, 0},
  [FD_EVENTS] = {                                     "/dev/events",   1, 0, 0},
  [FD_DISPINFO] = {                                  "/proc/dispinfo", 128, 0, 0},
#include "files.h"
};

#define NR_FILES (lengthof(file_table))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char* pathname, int flags, int mode) {
  for (size_t i = 0; i < NR_FILES; ++i)
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = file_table[i].disk_offset;
      Log("%d", i);
      return i;
    };
  assert(0);  // fs_open 没有找到 pathname 所指示的文件属于异常情况
  return -1;
}

ssize_t fs_read(int fd, void* buf, size_t len) {
  assert(fd >= 0);

  Finfo* f = file_table + fd;
  off_t eof = f->disk_offset + f->size;
  if (fd >= FD_NORMAL &&
    (f->open_offset >= eof || f->open_offset < f->disk_offset))
    return 0;

  size_t rlen = min(len, (size_t) (eof - f->open_offset));
  ramdisk_read(buf, f->open_offset, rlen);
  f->open_offset += rlen;
  return rlen;
}

ssize_t fs_write(int fd, const void* buf, size_t len) {
  assert(fd >= 0);

  Finfo* f = file_table + fd;
  off_t eof = f->disk_offset + f->size;
  if (fd >= FD_NORMAL &&
    (f->open_offset >= eof || f->open_offset < f->disk_offset))
    return 0;

  switch (fd) {
    case FD_STDOUT :
    case FD_STDERR :
      NULL;
      const char* _buf = buf;
      for (size_t i = 0; i < len; ++i) _putc(_buf[i]);
      return len;
    default :
      NULL;
      size_t rlen = min(len, (size_t) (eof - f->open_offset));
      ramdisk_write(buf, f->open_offset, rlen);
      f->open_offset += rlen;
      return rlen;
  }
  return 0;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  assert(fd >= 0);

  Finfo* f = file_table + fd;

  switch (whence) {
    case SEEK_CUR : f->open_offset += offset; break;
    case SEEK_END :
      NULL;
      off_t eof = f->disk_offset + f->size;
      f->open_offset = eof + offset;
      break;
    case SEEK_SET : f->open_offset = offset; break;
    default : return -1;
  }

  return 0;
}

int fs_close(int fd) {
  return 0;
}

size_t fs_filesz(int fd) {
  return file_table[fd].size;
}
