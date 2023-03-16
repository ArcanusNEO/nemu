#include "monitor/expr.h"
#include "monitor/watchpoint.h"

#include "list.h"

static unsigned wp_counter;
static list_t* wp_pool;

void init_wp_pool(list_t* smart_wp_pool) {
  wp_pool = smart_wp_pool;
  wp_counter = 0;
}

/* TODO: Implement the functionality of watchpoint */

watchpoint_t* new_wp(char expr_str[]) {
  if (expr_str == NULL || expr_str[0] == '\0') return NULL;

  size_t len = strlen(expr_str);
  watchpoint_t* wp = malloc(sizeof(watchpoint_t) + len + 1);

  strcpy(wp->expr_str, expr_str);
  wp->no = ++wp_counter;
  
  call(wp_pool, push_back, wp);
}
