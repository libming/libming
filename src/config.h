#define USE_ZLIB 1
#define USE_GIF 1
#define USE_PNG 1

#if USE_GIF
#undef USE_ZLIB
#define USE_ZLIB 1
#endif

#if USE_PNG
#undef USE_ZLIB
#define USE_ZLIB 1
#endif
