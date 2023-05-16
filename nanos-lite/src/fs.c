#include "fs.h"

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
  {  "stdin (note that this is not the actual stdin)",   0, 0},
  {"stdout (note that this is not the actual stdout)",   0, 0},
  {"stderr (note that this is not the actual stderr)",   0, 0},
  [FD_FB] = {                                         "/dev/fb",   0, 0},
  [FD_EVENTS] = {                                     "/dev/events",   0, 0},
  [FD_DISPINFO] = {                                  "/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char* pathname, int flags, int mode) {
  TODO();
  return 0;
}

ssize_t fs_read(int fd, void* buf, size_t len) {
  TODO();
  return 0;
}

ssize_t fs_write(int fd, const void* buf, size_t len) {
  Finfo* fp = file_table + fd;
  const char* _buf = buf;

  switch (fd) {
    case FD_STDOUT :
    case FD_STDERR :
      for (size_t i = 0; i < len; ++i) _putc(_buf[i]);
      return len;
  }
  return 0;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  TODO();
  return 0;
}

int fs_close(int fd) {
  TODO();
  return 0;
}

size_t fs_filesz(int fd) {
  TODO();
  return 0;
}
