/*
 * The Nexus Abstract Architecture
 * Minmal architectural-independent library for implementing system software
 *
 * Please refer to the AM specification
 */
#ifndef __AM_H__
#define __AM_H__

#include <stdint.h>
#include <sys/types.h>
#include <arch.h>

#ifndef NULL
# define NULL ((void *)0)
#endif

#define MAX_CPU 8

typedef struct _Area {
  void *start, *end;
} _Area; 

#define _KEYS(_) \
  _(ESCAPE) _(F1) _(F2) _(F3) _(F4) _(F5) _(F6) _(F7) _(F8) _(F9) _(F10) _(F11) _(F12) \
  _(GRAVE) _(1) _(2) _(3) _(4) _(5) _(6) _(7) _(8) _(9) _(0) _(MINUS) _(EQUALS) _(BACKSPACE) \
  _(TAB) _(Q) _(W) _(E) _(R) _(T) _(Y) _(U) _(I) _(O) _(P) _(LEFTBRACKET) _(RIGHTBRACKET) _(BACKSLASH) \
  _(CAPSLOCK) _(A) _(S) _(D) _(F) _(G) _(H) _(J) _(K) _(L) _(SEMICOLON) _(APOSTROPHE) _(RETURN) \
  _(LSHIFT) _(Z) _(X) _(C) _(V) _(B) _(N) _(M) _(COMMA) _(PERIOD) _(SLASH) _(RSHIFT) \
  _(LCTRL) _(APPLICATION) _(LALT) _(SPACE) _(RALT) _(RCTRL) \
  _(UP) _(DOWN) _(LEFT) _(RIGHT) _(INSERT) _(DELETE) _(HOME) _(END) _(PAGEUP) _(PAGEDOWN)

#define _KEY_NAME(k) _KEY_##k,

enum {
  _KEY_NONE = 0,
  _KEYS(_KEY_NAME)
};

#define _EVENTS(_) \
  _(IRQ_TIME) _(IRQ_IODEV) \
  _(ERROR) _(PAGE_FAULT) _(BUS_ERROR) _(NUMERIC) \
  _(TRAP) _(SYSCALL)

#define _EVENT_NAME(ev) _EVENT_##ev,

enum {
  _EVENT_NULL = 0,
  _EVENTS(_EVENT_NAME)
};

typedef struct _RegSet _RegSet;

typedef struct _Event {
  int event;
  intptr_t cause;
} _Event;

typedef struct _Screen {
  int width, height;
} _Screen;

typedef struct _Protect {
  _Area area; 
  void *ptr;
} _Protect;

#ifdef __cplusplus
extern "C" {
#endif

// =======================================================================
// [0] Turing Machine: code execution & a heap memory
// =======================================================================

void _putc(char ch);
void _halt(int code);
extern _Area _heap;

// =======================================================================
// [1] IO Extension (IOE)
// =======================================================================

void _ioe_init();
unsigned long _uptime();
int _read_key();
void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h);
void _draw_sync();
extern _Screen _screen;

// =======================================================================
// [2] Asynchronous Extension (ASYE)
// =======================================================================

void _asye_init(_RegSet* (*l)(_Event ev, _RegSet *regs));
_RegSet *_make(_Area kstack, void *entry, void *arg);
void _trap();
int _istatus(int enable);

// =======================================================================
// [3] Protection Extension (PTE)
// =======================================================================

void _pte_init(void*(*palloc)(), void (*pfree)(void*));
void _protect(_Protect *p);
void _release(_Protect *p);
void _map(_Protect *p, void *va, void *pa);
void _unmap(_Protect *p, void *va);
void _switch(_Protect *p);
_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]);

// =======================================================================
// [4] Multi-Processor Extension (MPE)
// =======================================================================

void _mpe_init(void (*entry)());
int _cpu();
intptr_t _atomic_xchg(volatile intptr_t *addr, intptr_t newval);
void _barrier();
extern int _NR_CPU;

#ifdef __cplusplus
}
#endif

#endif

/*------------------------------------NEOC------------------------------------*/

#ifndef _NEOC_H_
#define _NEOC_H_

#define crpc (crstack->_pc)

#define coroutine   \
  {                 \
    switch (crpc) { \
      case 0 :;

#define crret(ret...) \
  do {                \
    crpc = __LINE__;  \
    return ret;       \
    case __LINE__ :;  \
  } while (0)

#define crclose(ret...) \
  }                     \
  crret(ret);           \
  }                     \
  return ret

#define reducei(reducei_helper, ...)                   \
  ({                                                   \
    typeof(__VA_ARGS__) _a_[] = {__VA_ARGS__};         \
    typeof(__VA_ARGS__) _r_ = _a_[0];                  \
    for (size_t _i_ = 1; _i_ < lengthof(_a_); ++_i_) { \
      reducei_helper(_r_, _a_[_i_]);                   \
    }                                                  \
    _r_;                                               \
  })

#define max_helper(i, j)      \
  do {                        \
    if ((i) < (j)) (i) = (j); \
  } while (0)
#define max(...) reducei(max_helper, __VA_ARGS__)

#define min_helper(i, j)      \
  do {                        \
    if ((i) > (j)) (i) = (j); \
  } while (0)
#define min(...) reducei(min_helper, __VA_ARGS__)

#define swap(vari, varj)           \
  do {                             \
    typeof(&(vari)) _i_ = &(vari); \
    typeof(&(varj)) _j_ = &(varj); \
    typeof(vari) _t_ = *_i_;       \
    *_i_ = *_j_;                   \
    *_j_ = _t_;                    \
  } while (0)

#define likely(cond)   __builtin_expect(!!(cond), 1)
#define unlikely(cond) __builtin_expect(!!(cond), 0)

#define alphabet_order(ch)                        \
  (((ch) >= 'A' && (ch) <= 'Z')    ? (ch) - 'A' : \
      ((ch) >= 'a' && (ch) <= 'z') ? (ch) - 'a' : \
                                     -1)

#define true_inline inline __attribute__((always_inline))

#define quote_helper(content) #content
#define quote(content)        quote_helper(content)

#define lengthof(array) (sizeof(array) / sizeof((array)[0]))

#ifndef offsetof
  #define offsetof(struct_type, mbr) ((size_t) (&((struct_type*) 0)->mbr))
#endif

#define tostruct(struct_type, mbr, mbrptr) \
  ((struct_type*) ((size_t) mbrptr - offsetof(struct_type, mbr)))

#define lambda(retty, fn) ({ retty _lambda_fn_ fn _lambda_fn_; })

#define call(instance, mbrfn, args...) ((instance)->mbrfn(instance, ##args))

#define static_call(struct_name, fn, args...) ((struct_name##_##fn)(args))

#define bind_fn(struct_name, instance, mbrfn) \
  (instance->mbrfn = struct_name##_##mbrfn)

#define header_code(struct_name)               \
  void*(struct_name##_init)(void* instance);   \
  void*(struct_name##_uninit)(void* instance); \
  void*(struct_name##_release)(void* pinstance);

#define header_static_code(struct_name)               \
  static void*(struct_name##_init)(void* instance);   \
  static void*(struct_name##_uninit)(void* instance); \
  static void*(struct_name##_release)(void* pinstance);

#define release_code(struct_name)                                           \
  void*(struct_name##_release)(void* pinstance) {                           \
    if (pinstance == NULL || *(void**) pinstance == NULL) return pinstance; \
    struct struct_name* _instance_ = *(struct struct_name**) pinstance;     \
    free((struct_name##_uninit)(_instance_));                               \
    return pinstance;                                                       \
  }

#define release_static_code(struct_name) static release_code(struct_name)

// static void* _generic_release_(void* pinstance) {
//   free(*(void**) pinstance);
//   return pinstance;
// }

#define smart                    __attribute__((cleanup(_generic_release_)))
#define smart_class(struct_name) __attribute__((cleanup(struct_name##_release)))
#define auto_class(struct_name)  __attribute__((cleanup(struct_name##_uninit)))

#define smart_def(struct_name, id)                  \
  smart_class(struct_name) struct struct_name* id = \
    (struct_name##_init)(malloc(sizeof(struct struct_name)))

#define attr_packed __attribute__((packed))
#define inheritable __attribute__((packed))

typedef struct {
} null_t;

#endif  // _NEOC_H_
