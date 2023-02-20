#ifndef _TSP_LIST_H
#define _TSP_LIST_H

#include <stdlib.h>
#include <stdio.h>


typedef struct list list_t;

list_t *list_empty();
list_t *list_singleton(int element);
void list_free(list_t *list);

int list_element(list_t *list, int index);
int list_count(list_t *list);
void list_insert(list_t *list, int new_element);

void list_print(list_t *l);

#endif