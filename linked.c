#include <stdlib.h>
#include "linked.h"

typedef struct node {
  int elem;
  struct node *next;
  struct node *previous;
} node_t;

typedef struct linked {
  node_t *head;
  node_t *tail;
} linked_t;

node_t *new_node(int elem, node_t *next, node_t *previous) {
  node_t *node = (node_t *) malloc(sizeof(node_t));
  node->elem = elem;
  node->next = next;
  node->previous = previous;
  return node;
}

linked_t *linked_empty() {
  linked_t *linked = (linked_t *) malloc(sizeof(linked_t));
  linked->head = NULL;
  linked->tail = NULL;
  return linked;
}

void linked_push(linked_t *linked, int elem) {
  if (linked->head == NULL && linked->tail == NULL) {
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
  if (linked->head == linked->tail) {
    node_t *node = linked->head;
    int elem = node->elem;
    linked->head = NULL;
    linked->tail = NULL;
    free(node);
    return elem;
  } else {
    node_t *node = linked->tail;
    node_t *prev_node = node->previous;
    prev_node->next = NULL;
    linked->tail = prev_node;
    int elem = node->elem;
    free(node);
    return elem;
  }
}