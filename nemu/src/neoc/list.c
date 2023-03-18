#include "list.h"

static void* list_pop(struct list* this, list_node_t* selection) {
  if (this->_ == NULL) return NULL;
  if (selection == NULL) selection = this->_->prev;

  list_node_t* p = selection->prev;
  list_node_t* n = selection->next;
  void* payload = selection->payload;

  p->next = n;
  n->prev = p;

  if (this->_ == selection) this->_ = n;

  --this->_size;

  if (this->_release_payload) free(payload);
  free(selection);

  if (this->_size == 0) this->_ = NULL;

  return payload;
}

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

static void* _insert_first_(struct list* this, void* payload) {
  this->_ = malloc(sizeof(list_node_t));
  this->_->payload = payload;
  this->_->prev = this->_->next = this->_;
  this->_size = 1;
  return payload;
}

static void _insert_(list_node_t* node, list_node_t* prev, list_node_t* next) {
  node->next = next;
  node->prev = prev;
  prev->next = node;
  next->prev = node;
}

static void* list_push(struct list* this, void* payload, list_node_t* prev) {
  if (this->_ == NULL) return _insert_first_(this, payload);
  if (prev == NULL) prev = this->_->prev;

  list_node_t* next = prev->next;

  list_node_t* p = malloc(sizeof(list_node_t));
  p->payload = payload;

  _insert_(p, prev, next);
  ++this->_size;

  return payload;
}

static void* list_push_back(struct list* this, void* payload) {
  return list_push(this, payload, NULL);
}

static void* list_push_front(struct list* this, void* payload) {
  list_push_back(this, payload);
  this->_ = this->_->prev;
  return payload;
}

static void* list_pop_back(struct list* this) {
  return list_pop(this, NULL);
}

static void* list_pop_front(struct list* this) {
  return list_pop(this, this->_);
}

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
  bind_fn(list, this, pop);
  bind_fn(list, this, push);
}

void list_uninit(void* instance) {
  list_t* this = instance;

  while (!list_empty(this)) list_pop_back(this);
}

destroy_code(list);
