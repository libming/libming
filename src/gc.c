/*
    Ming, an SWF output library

    Copyright (C) 2005-2009  Sandro Santilli <strk@keybit.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __C2MAN__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
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

