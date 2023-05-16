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
