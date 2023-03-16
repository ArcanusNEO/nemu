#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#include "list.h"

typedef struct watchpoint {
  int no;

  /* TODO: Add more members if necessary */

  char expr_str[0];

} watchpoint_t;

void init_wp_pool(list_t* smart_wp_pool);
watchpoint_t* new_wp(char expr_str[]);

#endif
