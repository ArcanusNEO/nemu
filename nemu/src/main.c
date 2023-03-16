#include "neoc.h"

#include "list.h"

int init_monitor(int, char*[], int, void*);
void ui_mainloop(int);

signed main(int argc, char* argv[]) {
  /* Initialize the monitor. */
  smart_def(list, smart_wp_pool);
  void* initv[] = {smart_wp_pool};
  int is_batch_mode = init_monitor(argc, argv, lengthof(initv), initv);

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);
}
