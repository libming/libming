#ifndef GC_H
#define GC_H 1

#include "ming_config.h"

typedef void *mem_node;

mem_node *ming_gc_add_node(void *ptr, void *dtor);
void ming_gc_remove_node(mem_node *node);

#endif
