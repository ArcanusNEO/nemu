#include "monitor/expr.h"
#include "monitor/watchpoint.h"

#include "cpu/reg.h"
#include "memory/memory.h"

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
  bool res;
  wp->val = expr(expr_str, &res);

  if (res) {
    wp->no = ++wp_counter;
    call(wp_pool, push_back, wp);
  } else {
    free(wp);
    wp = NULL;
  }

  return wp;
}

bool travel_wp(bool summary) {
  list_node_t* i = wp_pool->_;
  bool ret = false;

  do {
    watchpoint_t* wp = i->payload;
    uint32_t old_val = wp->val;

    if (summary) {
      printf("Watchpoint #%u: %s\n", wp->no, wp->expr_str);
      printf("Value: 0x%08x\t%d\n\n", wp->val, wp->val);
    } else {
      uint32_t val = expr(wp->expr_str, NULL);
      wp->val = val;

      if (val != old_val) {
        puts("");

        printf("Watchpoint #%u: %s\n", wp->no, wp->expr_str);
        printf("Old value: 0x%08x\t%d\n", old_val, old_val);
        printf("New value: 0x%08x\t%d\n", val, val);

        vaddr_t addr = cpu.eip;
        printf("At 0x%08x: 0x%08x\n", addr, vaddr_read(addr, 4));

        puts("");

        ret = true;
      }
    }

    i = i->next;

  } while (i != wp_pool->_);

  return ret;
}
