#define USE_ZLIB 1
#undef USE_GIF 
#undef USE_PNG 

/*
 * Define this if you want Ming to track
 * all objects allocations. Ming will
 * mantain a linked doubly linked list 
 * of allocated objects, call ming_garbage_collect
 * to get rid of them all.
 */
#undef TRACK_ALLOCS

#if USE_GIF
#undef USE_ZLIB
#define USE_ZLIB 1
#endif

#if USE_PNG
#undef USE_ZLIB
#define USE_ZLIB 1
#endif
