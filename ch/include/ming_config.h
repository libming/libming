#ifndef MING_CONFIG_H
#define MING_CONFIG_H

#define USE_ZLIB 1
#define USE_PNG  1
#undef USE_GIF 

/*
 * Define this if you want Ming to track
 * all objects allocations. Ming will
 * mantain a linked doubly linked list 
 * of allocated objects, call ming_garbage_collect
 * to get rid of them all.
 */
#define TRACK_ALLOCS 1

#if USE_GIF
#undef USE_ZLIB
#define USE_ZLIB 1
#endif

#if USE_PNG
#undef USE_ZLIB
#define USE_ZLIB 1
#endif

#endif
