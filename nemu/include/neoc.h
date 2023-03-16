#ifndef _NEOC_H_
#define _NEOC_H_

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#define alphabet_order(ch)                        \
  (((ch) >= 'A' && (ch) <= 'Z')    ? (ch) - 'A' : \
      ((ch) >= 'a' && (ch) <= 'z') ? (ch) - 'a' : \
                                     -1)

#define true_inline __attribute__((always_inline)) inline

#define quote_helper(content) #content
#define quote(content)        quote_helper(content)

#define lengthof(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifndef offsetof
  #define offsetof(stty, mbr) ((size_t) (&((stty*) 0)->mbr))
#endif

#define tostruct(stty, mbr, mbrptr) \
  ((stty*) ((size_t) mbrptr - offsetof(stty，mbr)))

#define lambda(retty, func) ({ retty _lambda_func_ func _lambda_func_; })

#define call(ptr, mbrfunc, args...) (ptr->mbrfunc(ptr, ##args))

#define static_call(st, func, args...) ((st##_##func)(args))

#define bind_fn(st, ptr, mbrfunc) (ptr->mbrfunc = st##_##mbrfunc)

#define create(st)                         \
  ({                                       \
    void* ptr = malloc(sizeof(struct st)); \
    (st##_##init)(ptr);                    \
    ptr;                                   \
  })

#define destroy(st, ptr)  \
  ({                      \
    (st##_##uninit)(ptr); \
    free(ptr);            \
  })

#define destroy_code(st)                  \
  void(st##_##destroy)(void* pinstance) { \
    (st##_##uninit)(*(void**) pinstance); \
    free(*(void**) pinstance);            \
  }

true_inline void _generic_release_(void* ptr) {
  free(*(void**) ptr);
}

#define smart __attribute__((cleanup(_generic_release_)))

#define smart_class(st) __attribute__((cleanup(st##_##destroy)))

#define smart_def(st, name)                                    \
  smart_class(st) struct st* name = malloc(sizeof(struct st)); \
  (st##_##init)(name);

#define attr_packed __attribute__((packed))

#endif  // _NEOC_H_