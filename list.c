#include "list.h"

struct list {
  int count;
  int capacity;
  int *elements;
};

list_t *list_empty() {
  list_t *list = (list_t *) malloc(sizeof(list_t));
  list->count = 0;
  list->capacity = 0;
  list->elements = NULL;
  return list;
}

void list_free(list_t *list) {
  list->capacity = 0;
  list->count = 0;
  free(list->elements);
  free(list);
}

list_t *list_singleton(int element) {
  list_t *list = list_empty();
  list_insert(list, element);
  return list;
}

int list_element(list_t *list, int index) {
  return list->elements[index];
}

int list_count(list_t *list) {
  return list->count;
}

int list_capacity(list_t *list) {
  return list->capacity;
}

void increase_size(list_t *list) {
  printf("list->capacity %d\n", list->capacity);
  list->capacity = list->capacity < 10 ? 10 : list->capacity * 2;
  int *new_elements = realloc(list->elements, sizeof(int) * list->capacity);
  list->elements = new_elements;
}

void list_insert(list_t *list, int new_element) {
  if (list->count + 1 > list->capacity) {
    increase_size(list);
  }
  list->elements[list->count] = new_element;
  list->count++;
}

void list_print(list_t *l) {
  for (int i = 0; i < list_count(l); i++) {
    printf("%d ", list_element(l, i));
  }
  printf("\n");
}
