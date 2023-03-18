#ifndef _LIST_H_
#define _LIST_H_

#include "neoc.h"

typedef struct list_node {
  struct list_node* prev;
  struct list_node* next;
  void* payload;
} list_node_t;

typedef struct list {
  list_node_t* _;
  size_t _size;
  int _release_payload;

  void (*set_release_payload)(struct list* this, int flag);
  int (*get_release_payload)(struct list* this);
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

// void (*set_release_payload)(struct list* this, int flag);
#define set_release_payload set_release_payload
// int (*get_release_payload)(struct list* this);
#define get_release_payload get_release_payload
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