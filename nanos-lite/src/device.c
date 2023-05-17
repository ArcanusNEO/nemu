#include "common.h"

#include "device.h"

#define NAME(key) [_KEY_##key] = #key,

static const char* keyname[256]
  __attribute__((used)) = {[_KEY_NONE] = "NONE", _KEYS(NAME)};

size_t events_read(void* buf, size_t len) {
  // int key = _read_key();
  // if (key == _KEY_NONE) {
  //   unsigned long uptime = _uptime;
  //   snprintf(buf, len, "t %lu\n", uptime);
  // } else {
  //   strcpy(buf, keyname[key]);
  // }
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void* buf, off_t offset, size_t len) {
  memset(buf, 0, len);
  strncpy(buf, dispinfo + offset, len);
}

ssize_t _read_pixel(uint32_t* pixels, off_t offset, size_t len);
ssize_t _write_pixel(const uint32_t* pixels, off_t offset, size_t len);

void fb_read(void* buf, off_t offset, size_t len) {
  _read_pixel(buf, offset, len);
}

void fb_write(const void* buf, off_t offset, size_t len) {
  _write_pixel(buf, offset, len);
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  extern _Screen _screen;
  snprintf(dispinfo, lengthof(dispinfo), "WIDTH:%d\nHEIGHT:%d\n", _screen.width,
    _screen.height);
  dispinfo[lengthof(dispinfo) - 1] = '\0';
}
