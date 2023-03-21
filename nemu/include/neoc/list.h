#ifndef _LIST_H_
#define _LIST_H_

#include "neoc.h"

typedef struct list_node {
  struct list_node* prev;
  struct list_node* next;
  void* payload;
} list_node_t;

typedef struct list {
  int _uninit          : 1;
  int _release_payload : 1;

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

// int (*empty)(struct list* this);
#define empty empty
// size_t (*size)(struct list* this);
#define size size
// void* (*push_front)(struct list* this, void* payload);
#define push_front push_front
// void* (*push_back)(struct list* this, void* payload);
#define push_back push_back
// void* (*pop_front)(struct list* this);
#define pop_front pop_front
// void* (*pop_back)(struct list* this);
#define pop_back pop_back
// void* (*pop)(struct list* this, list_node_t* selection);
#define pop pop
// void* (*push)(struct list* this, void* payload, list_node_t* prev)
#define push push

header_code(list);

#endif  // _LIST_H_