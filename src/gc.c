#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gc.h"

#define DEBUG 0


struct mem_node_t
{
	struct mem_node_t *next;
	struct mem_node_t *prev;
	void *ptr;
	dtorfunctype dtor; /* void (*dtor)(); */
};

static mem_node *firstnode = NULL;
static mem_node *lastnode = NULL;

#if DEBUG
static void dumpNodeList();
#endif
mem_node *ming_gc_add_node(void *ptr, dtorfunctype dtor)
{
	mem_node *node = (mem_node*) calloc(1, sizeof(mem_node));
	node->ptr = ptr;
	node->dtor = dtor;
	if ( ! firstnode ) firstnode = node;
	if ( ! lastnode ) lastnode = node;
	else {
		lastnode->next = node;
		node->prev = lastnode;
		lastnode = node;
	}
#if DEBUG
	printf("Node %p added\n", node);
	dumpNodeList();
#endif
	return node;
}

void
ming_gc_remove_node(mem_node *node)
{
	if ( node->prev ) {
		node->prev->next = node->next;
	} else {
		firstnode = node->next;
	}
	if ( node->next ) {
		node->next->prev = node->prev;
	} else {
		lastnode = node->prev;
	}

	free(node);
#if DEBUG
	printf("Node %p removed\n", node);
	dumpNodeList();
#endif
}

void
Ming_collectGarbage()
{
	mem_node *ptr;
#if DEBUG
	printf("GC called\n");
	dumpNodeList();
#endif
	ptr=firstnode;
	while(ptr)
	{
		ptr->dtor(ptr->ptr);
		/*
		 * the node destructor might destory other
		 * objects in the list, so we better always
		 * start from the firstnode 
		 */
		ptr=firstnode;
			
	}
}

#if DEBUG
static void
dumpNodeList()
{
	mem_node *ptr;
	printf("First node %p \n", firstnode);
	printf("Last node %p \n", lastnode);
	for (ptr=firstnode; ptr; ptr=ptr->next)
	{
		printf("Node %p (ptr %p / dtor %p)\n",
			ptr, ptr->ptr, ptr->dtor);
	}
}
#endif

