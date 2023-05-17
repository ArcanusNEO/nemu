#include "fs.h"

#include "device.h"
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
  {  "stdin (note that this is not the actual stdin)",   0, 0, 0},
  {"stdout (note that this is not the actual stdout)",   0, 0, 0},
  {"stderr (note that this is not the actual stderr)",   0, 0, 0},
  [FD_FB] = {                                         "/dev/fb",   0, 0, 0},
  [FD_EVENTS] = {                                     "/dev/events",   0, 0, 0},
  [FD_DISPINFO] = {                                  "/proc/dispinfo", 128, 0, 0},
#include "files.h"
};

#define NR_FILES (lengthof(file_table))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  extern _Screen _screen;
  file_table[FD_FB].size = _screen.height * _screen.width * 4;
}

int fs_open(const char* pathname, int flags, int mode) {
  for (size_t i = 0; i < NR_FILES; ++i)
    if (strcmp(pathname, file_table[i].name) == 0) {
      // Log("%s", file_table[i].name);
      file_table[i].open_offset = file_table[i].disk_offset;
      return i;
    };
  assert(0);  // fs_open 没有找到 pathname 所指示的文件属于异常情况
  return -1;
}

#define io_helper(fn)                                                \
  ({                                                                 \
    size_t rlen = max(0, min(f->size, len, (eof - f->open_offset))); \
    Log("%d", f->size);                                              \
    fn(buf, f->open_offset, rlen);                                   \
    f->open_offset += rlen;                                          \
    rlen;                                                            \
  })

ssize_t fs_read(int fd, void* buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);

  Finfo* f = file_table + fd;
  off_t eof = f->disk_offset + f->size;

  switch (fd) {
    case FD_STDIN :
    case FD_STDOUT :
    case FD_STDERR : return 0;
    case FD_FB : return io_helper(fb_read);
    case FD_EVENTS : break;
    case FD_DISPINFO : return io_helper(dispinfo_read);
    default : return io_helper(ramdisk_read);
  }

  return -1;
}

ssize_t fs_write(int fd, const void* buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);

  Finfo* f = file_table + fd;
  off_t eof = f->disk_offset + f->size;

  switch (fd) {
    case FD_STDIN : return 0;
    case FD_STDOUT :
    case FD_STDERR :
      NULL;
      const char* _buf = buf;
      for (size_t i = 0; i < len; ++i) _putc(_buf[i]);
      return len;
    case FD_FB : return io_helper(fb_write);
    case FD_EVENTS : return 0;
    case FD_DISPINFO : return 0;
    default : return io_helper(ramdisk_write);
  }

  return -1;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  assert(fd >= 0 && fd < NR_FILES);

  Finfo* f = file_table + fd;

  switch (whence) {
    case SEEK_CUR : f->open_offset += offset; break;
    case SEEK_END :
      NULL;
      off_t eof = f->disk_offset + f->size;
      f->open_offset = eof + offset;
      break;
    case SEEK_SET : f->open_offset = f->disk_offset + offset; break;
    default : return -1;
  }

  return f->open_offset - f->disk_offset;
}

int fs_close(int fd) {
  return 0;
}

size_t fs_filesz(int fd) {
  assert(fd >= 0 && fd < NR_FILES);

  return file_table[fd].size;
}
