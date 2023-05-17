#include <am.h>
#include <x86.h>

#define RTC_PORT   0x48  // Note that this is not standard
#define KEY_REG    0x60
#define KEY_STATUS 0x64

static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  return inl(RTC_PORT) - boot_time;
}

uint32_t* const fb = (uint32_t*) 0x40000;

_Screen _screen = {
  .width = 400,
  .height = 300,
};

extern void* memcpy(void*, const void*, int);

#define scrcpy(dst, src)                                          \
  ({                                                              \
    ssize_t scrsz = (ssize_t) _screen.width * _screen.height * 4; \
    size_t rlen = max(0, min(len, scrsz - offset, scrsz));        \
    if (rlen != 0) memcpy(dst, src, rlen);                        \
    rlen;                                                         \
  })

ssize_t _read_pixel(uint32_t* pixels, off_t offset, size_t len) {
  return scrcpy(pixels, (void*) fb + offset);
}

ssize_t _write_pixel(const uint32_t* pixels, off_t offset, size_t len) {
  return scrcpy((void*) fb + offset, pixels);
}

void _draw_rect(const uint32_t* pixels, int x, int y, int w, int h) {
  for (size_t i = 0; i < h && i + y < _screen.height; ++i)
    for (size_t j = 0; j < w && j + x < _screen.width; ++j)
      fb[(i + y) * _screen.width + (j + x)] = pixels[i * w + j];
}

void _draw_sync() { }

int _read_key() {
  if (inb(KEY_STATUS)) return inl(KEY_REG);
  return _KEY_NONE;
}
