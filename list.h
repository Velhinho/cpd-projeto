#ifndef _TSP_LIST_H
#define _TSP_LIST_H

#include <stdlib.h>
#include <stdio.h>


typedef struct list list_t;

list_t *list_empty();
list_t *list_singleton(int element);
list_t *list_copy(list_t *list);
list_t *list_append(list_t *list, int elem);
void list_free(list_t *list);

int list_element(list_t *list, int index);
void list_change_element(list_t *list, int index, int new_elem);
int list_count(list_t *list);
void list_insert(list_t *list, int new_element);
int list_contains(list_t *list, int elem);

void list_print(list_t *l);

#endif