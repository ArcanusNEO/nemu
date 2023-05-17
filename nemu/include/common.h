#ifndef __COMMON_H__
#define __COMMON_H__

#define DEBUG
#define DIFF_TEST

/* You will define this macro in PA2 */
#define HAS_IOE

#include "debug.h"
#include "macro.h"
#include "neoc.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

// typedef uint8_t bool;

typedef uint32_t rtlreg_t;

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

typedef uint16_t ioaddr_t;

// #define false 0
// #define true  1

#ifndef DISABLE_COLOR
  #define c_normal "\x1b[0m"
  #define c_bold   "\x1b[1m"
  #define c_red    "\x1b[31m"
  #define c_green  "\x1b[32m"
  #define c_yellow "\x1b[33m"
  #define c_blue   "\x1b[34m"
  #define c_purple "\x1b[35m"
  #define c_cyan   "\x1b[36m"
#else
  #define c_normal ""
  #define c_bold   ""
  #define c_red    ""
  #define c_green  ""
  #define c_yellow ""
  #define c_blue   ""
  #define c_pueple ""
  #define c_cyan   ""
#endif

#endif
