#ifndef _LIST_H_
#define _LIST_H_

#include "neoc.h"

typedef struct list_node {
  struct list_node* prev;
  struct list_node* next;
  void* payload;
} list_node_t;

typedef struct list {
  int _uninit;
  void (*_payload_releaser)(void*);

  list_node_t* _;
  size_t _size;

  int (*empty)(struct list* this);
  size_t (*size)(struct list* this);
  void* (*push_front)(struct list* this, void* payload);
  void* (*push_back)(struct list* this, void* payload);
  void* (*pop_front)(struct list* this);
  void* (*pop_back)(struct list* this);
  void* (*pop)(struct list* this, list_node_t* selection);
  void* (*push)(struct list* this, void* payload, list_node_t* prev);

} list_t;

// for auto suggestions
#if !defined(NEOC_NO_AUTO_SUGGESTIONS) && \
  (defined(NEOC_DEBUG) || !defined(NEOC_RELEASE))
  #define empty      empty
  #define size       size
  #define push_front push_front
  #define push_back  push_back
  #define pop_front  pop_front
  #define pop_back   pop_back
  #define pop        pop
  #define push       push
#endif

header_code(list);

#endif  // _LIST_H_