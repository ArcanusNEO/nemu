#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#include "neoc.h"

typedef struct watchpoint {
  unsigned no;

  /* TODO: Add more members if necessary */

  uint32_t val;
  char expr_str[0];

} watchpoint_t;

void init_wp_pool(list_t* smart_wp_pool);
watchpoint_t* new_wp(char expr_str[]);
bool travel_wp(bool summary);
void delete_wp(unsigned no);

#endif
