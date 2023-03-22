#ifndef _TSP_LINKED_H
#define _TSP_LINKED_H

typedef struct linked linked_t;

linked_t *linked_empty();
void linked_init(linked_t *linked);
void linked_push(linked_t *linked, int elem);
int linked_pop(linked_t *linked);

#endif