#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_MEM 0
#define CHECK_NODES 0

void *ming_calloc(size_t nmemb, size_t size);
void *ming_malloc(size_t size);
void *ming_realloc(void *ptr, size_t size);
void ming_free(void *ptr);
char *ming_strdup(const char *s);
char *ming_strndup(const char *s, size_t size);
void ming_garbage_collect(void);

typedef struct NODE_T
{
#if CHECK_NODES
	struct NODE_T *self;
	size_t size;
#endif
	struct NODE_T *next;
	struct NODE_T *prev;
#if CHECK_NODES
	struct NODE_T *self2;
#endif
	void *data;
}
NODE;

#define NODHDRSZ (sizeof(NODE)-sizeof(void *))

static NODE *firstnode = NULL;
static NODE *lastnode = NULL;


/*******************************************************************/

void *
ming_calloc(size_t nmemb, size_t size)
{
	NODE *node;
#if DEBUG_MEM
	fprintf(stderr, "ming_calloc(%d, %d)\n", nmemb, size);
#endif

	if ( size == 0 )
		fprintf(stderr, "ming_calloc(%d, %d)\n", nmemb, size);

	node = (NODE *)calloc(nmemb, size+NODHDRSZ);
	if ( ! node )
	{
#if DEBUG_MEM
		fprintf(stderr, "ming_calloc(%d, %d) returns NULL\n", nmemb, size);
#endif
		return NULL;
	}

	node->prev = lastnode;
	if ( ! firstnode ) {
		firstnode = node;
	} else {
		lastnode->next = node;
	}
#if CHECK_NODES
	node->self = node;
	node->self2 = node;
	node->size = size;
#endif

	node->next = NULL;
	lastnode = node;

#if DEBUG_MEM
	fprintf(stderr, "ming_calloc(%d, %d) allocates %p, returns %p\n", nmemb, size, node, &(node->data));
#endif

	return &(node->data);
}

void *
ming_malloc(size_t size)
{
	NODE *node;

	node = malloc(size+NODHDRSZ);
	if ( ! node )
	{
#if DEBUG_MEM
	fprintf(stderr, "ming_malloc(%d) returns NULL\n", size);
#endif
		return NULL;
	}

	node->prev = lastnode;
	if ( ! firstnode ) {
		firstnode = node;
	} else {
		lastnode->next = node;
	}
	node->next = NULL;
	lastnode = node;

#if DEBUG_MEM
	fprintf(stderr, "ming_malloc(%d) allocated %p, returns %p\n", size,
		node, &(node->data));
#endif

#if CHECK_NODES
	node->self = node;
	node->self2 = node;
	node->size = size;
#endif

	return &(node->data);

}

void *
ming_realloc(void *oldptr, size_t size)
{
	NODE *node, *oldnode, *prev, *next;

#if DEBUG_MEM
	fprintf(stderr, "ming_realloc(%p, %d)\n", oldptr, size);
#endif

	if ( ! oldptr )
	{
#if DEBUG_MEM
		fprintf(stderr, " calls ming_malloc(%d)\n", size);
#endif
		return ming_malloc(size);
	}

	if ( ! size )
	{
#if DEBUG_MEM
		fprintf(stderr, " calls ming_free(%p)\n", oldptr);
#endif
		ming_free(oldptr);
		return NULL;
	}

	oldnode = oldptr-NODHDRSZ;

#if CHECK_NODES
	if ( oldnode->self != oldnode || oldnode->self2 != oldnode )
	{
		fprintf(stderr, "WARN! ming_realloc(%p,%d): corrupted node (node:%p,self:%p)\n", oldptr, size, oldnode, oldnode->self);
		return NULL;
		//return realloc(oldptr, size);
	}
#endif

	prev = oldnode->prev;
	next = oldnode->next;


	node = realloc(oldnode, size+NODHDRSZ);
	if ( ! node )
	{
#if DEBUG_MEM
		fprintf(stderr, " returns NULL\n");
#endif
		return NULL;
	}

	/* Link information of the node is correct, should just
	 * update linkers to this node
	 */
	if ( node != oldnode )
	{
#if DEBUG_MEM
		fprintf(stderr, " moved allocation from %p to %p, relinking adhiacent nodes\n", oldnode, node);
#endif
		if ( prev ) prev->next = node;
		else firstnode = node;
		if ( next ) next->prev = node;
		else lastnode = node;
	}

#if CHECK_NODES
	node->self = node;
	node->self2 = node;
	node->size = size;
#endif

#if DEBUG_MEM
	fprintf(stderr, "ming_realloc(%p, %d) allocates %p, returns %p\n", oldptr, size, node, &(node->data));
#endif

	return &(node->data);
}

void
ming_free(void *ptr)
{
	NODE *node;
	
	node = (NODE *)(ptr-NODHDRSZ);

#if DEBUG_MEM
	fprintf(stderr, "ming_free(%p) brings to node %p\n", ptr, node);
#endif


#if CHECK_NODES
	if ( node->self != node || node->self2 != node )
	{
		fprintf(stderr, "WARN! ming_free: corrupted node (node:%p,self:%p)\n", node, node->self);
		return;
	}
#endif

	if ( node->next )
	{
#if DEBUG_MEM
		fprintf(stderr, " next node %p had prev=%p, now %p\n", node->next, node->next->prev, node->prev);
#endif
		node->next->prev = node->prev;
	}
	else
	{
#if DEBUG_MEM
		fprintf(stderr, " no next node, prev node (%p) set as lastnode\n", node->prev);
#endif
		lastnode = node->prev;
	}

	if ( node->prev )
	{
#if DEBUG_MEM
		fprintf(stderr, " prev node %p had next=%p, now %p\n", node->prev, node->prev->next, node->next);
#endif
		node->prev->next = node->next;
	}
	else
	{
#if DEBUG_MEM
		fprintf(stderr, " no prev node, next node (%p) set as firstnode\n", node->next);
#endif
		firstnode = node->next;
	}

	free(node);
}

char *
ming_strdup(const char *s)
{
	NODE *node;
	char *ret;
	size_t size;

#if DEBUG_MEM
	fprintf(stderr, "ming_strdup(%p)\n", s);
#endif

	size = strlen(s)+1;

	node = malloc(size+NODHDRSZ);
	if ( ! node )
	{
#if DEBUG_MEM
		fprintf(stderr, "ming_strdup(%p) returns NULL\n", s);
#endif
		return NULL;
	}

	node->prev = lastnode;
	if ( ! firstnode ) {
		firstnode = node;
	} else {
		lastnode->next = node;
	}
	node->next = NULL;
	lastnode = node;

	ret = (char *)&(node->data);
	strcpy(ret, s);

#if DEBUG_MEM
	fprintf(stderr, "ming_strdup(%p) allocates %p, returns %p\n", s, node, ret);
#endif

#if CHECK_NODES
	node->self = node;
	node->self2 = node;
	node->size = size;
#endif

	return ret;
}

char *
ming_strndup(const char *s, size_t size)
{
	NODE *node;
	char *ret;

#if DEBUG_MEM
	fprintf(stderr, "ming_strndup(%p, %d)\n", s, size);
#endif

	node = malloc(size+1+NODHDRSZ);
	if ( ! node )
	{
#if DEBUG_MEM
		fprintf(stderr, "ming_strndup(%p, %d) returns NULL\n", s, size);
#endif
		return NULL;
	}

	node->prev = lastnode;
	if ( ! firstnode ) {
		firstnode = node;
	} else {
		lastnode->next = node;
	}
	node->next = NULL;
	lastnode = node;

	ret = (char *)&(node->data);
	strncpy(ret, s, size);
	ret[size] = '\0';

#if DEBUG_MEM
	fprintf(stderr, "ming_strndup(%p, %d) allocates %p, returns %p\n", s, size, node, ret);
#endif

#if CHECK_NODES
	node->self = node;
	node->self2 = node;
	node->size = size;
#endif

	return ret;
}

void
ming_garbage_collect()
{
	NODE *it = firstnode;
	void *ptr;

#if DEBUG_MEM
	fprintf(stderr, "ming_garbage_collect() called\n");
#endif

	while (it)
	{
#if DEBUG_MEM
		fprintf(stderr, "ming_garbage_collect(): cleaning node %p\n",
			it);
#endif
		ptr = it->next;
		free(it);
		it = ptr;
	}
	firstnode = NULL;
	lastnode = NULL;
}

