#ifndef GC_H
#define GC_H 1

#include "ming_config.h"

/* typedef void *mem_node; */
typedef struct mem_node_t mem_node;

typedef void (*dtorfunctype)(void*);

mem_node *ming_gc_add_node(void *ptr, dtorfunctype dtor);
void ming_gc_remove_node(mem_node *node);

#endif
