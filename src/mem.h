#include "ming_config.h"

#if WRAP_MEMORY_MANAGEMENT
void *ming_calloc(size_t nmemb, size_t size);
void *ming_malloc(size_t size);
void *ming_realloc(void *ptr, size_t size);
void ming_free(void *ptr);
char *ming_strdup(const char *s);
char *ming_strndup(const char *s, size_t size);
#else
#define ming_calloc calloc
#define ming_malloc malloc
#define ming_realloc realloc
#define ming_free free
#define ming_strdup strdup
#define ming_strndup ming_strndup
#endif
void ming_garbage_collect(void);


