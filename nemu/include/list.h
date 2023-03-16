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

static void list_set_release_payload(struct list* this, int flag) {
  this->_release_payload = flag;
}

static int list_get_release_payload(struct list* this) {
  return this->_release_payload;
}

static int list_empty(struct list* this) {
  return this->_ == NULL;
}

static size_t list_size(struct list* this) {
  return this->_size;
}

static void* insert_first(struct list* this, void* payload) {
  this->_ = malloc(sizeof(list_node_t));
  this->_->payload = payload;
  this->_->prev = this->_->next = this->_;
  this->_size = 1;
  return payload;
}

static int instert(list_node_t* node, list_node_t* prev, list_node_t* next) {
  node->next = next;
  node->prev = prev;
  prev->next = node;
  next->prev = node;
}

static void* list_push_back(struct list* this, void* payload) {
  if (this->_ == NULL) return insert_first(this, payload);

  list_node_t* p = malloc(sizeof(list_node_t));
  p->payload = payload;
  instert(p, this->_->prev, this->_);
  ++this->_size;
  return payload;
}

static void* list_push_front(struct list* this, void* payload) {
  list_push_back(this, payload);
  this->_ = this->_->prev;
  return payload;
}

static void* list_pop_back(struct list* this) {
  if (this->_ == NULL) return NULL;

  list_node_t* mid = this->_->prev;
  list_node_t* p = this->_->prev->prev;
  void* payload = mid->payload;

  p->next = this->_;
  this->_->prev = p;

  --this->_size;

  if (this->_release_payload) free(payload);
  free(mid);

  if (this->_size == 0) this->_ = NULL;

  return payload;
}

static void* list_pop_front(struct list* this) {
  if (this->_ == NULL) return NULL;

  this->_ = this->_->next;
  return list_pop_back(this);
}

header_code(list);

void list_init(void* instance) {
  list_t* this = instance;
  this->_ = NULL;
  this->_size = 0;
  this->_release_payload = 1;

  bind_fn(list, this, set_release_payload);
  bind_fn(list, this, get_release_payload);
  bind_fn(list, this, empty);
  bind_fn(list, this, size);
  bind_fn(list, this, push_front);
  bind_fn(list, this, push_back);
  bind_fn(list, this, pop_front);
  bind_fn(list, this, pop_back);
}

void list_uninit(void* instance) {
  list_t* this = instance;

  while (call(this, pop_back) != NULL) { }
}

destroy_code(list);