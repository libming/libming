#define USE_ZLIB 1
#undef USE_GIF 
#undef USE_PNG 

/*
 * Define this if you want Ming to release
 * all memory it allocates at Ming_cleanup() time.
 * Beware that the running speed will be much lower.
 */
#undef WRAP_MEMORY_MANAGEMENT

#if USE_GIF
#undef USE_ZLIB
#define USE_ZLIB 1
#endif

#if USE_PNG
#undef USE_ZLIB
#define USE_ZLIB 1
#endif
