#ifndef MEM_H
#define MEM_H 1

#include "ming_config.h"

#if WRAP_MEMORY_MANAGEMENT
#define calloc(x,y) ming_calloc((x),(y))
#define malloc(x) ming_malloc((x))
#define realloc(x,y) ming_realloc((x), (y))
#define free(x) ming_free((x))
#define strdup(x) ming_strdup((x))
#define strndup(x,y) ming_strndup((x),(y))
#endif

void *ming_calloc(size_t nmemb, size_t size);
void *ming_malloc(size_t size);
void *ming_realloc(void *ptr, size_t size);
void ming_free(void *ptr);
char *ming_strdup(const char *s);
char *ming_strndup(const char *s, size_t size);

void ming_garbage_collect(void);


#endif // ndef MEM_H
