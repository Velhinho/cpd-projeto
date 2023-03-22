#include <stdlib.h>

typedef struct node {
  int elem;
  struct node *next;
  struct node *previous
} node_t;

typedef struct linked {
  node_t *head;
  node_t *tail;
} linked_t;

node_t *new_node(int elem, node_t *next, node_t *previous) {
  node_t *node = (node_t *) malloc(sizeof(node_t));
  node->next = next;
  node->previous = previous;
  return node;
}

linked_t *linked_empty() {
  return NULL;
}

void linked_init(linked_t *linked) {
  linked = (linked_t *) malloc(sizeof(linked));
  linked->head = NULL;
  linked->tail = NULL;
}

void linked_push(linked_t *linked, int elem) {
  if (linked == NULL) {
    linked_init(linked);
    node_t *node = new_node(elem, NULL, NULL);
    linked->head = node;
    linked->tail = node;
  } else {
    node_t *node = new_node(elem, linked->head, NULL);
    node_t *next_node = linked->head;
    next_node->previous = node;
    linked->head = node;
  }
}

int linked_pop(linked_t *linked) {
  node_t *node = linked->tail;
  node_t *prev_node = node->previous;
  prev_node->next = NULL;
  linked->tail = prev_node;
  int elem = node->elem;
  free(node);
  return elem;
}