
#ifndef MING_LIBMING_INCLUDED
#define MING_LIBMING_INCLUDED

#include <string.h>

#ifndef TRUE
  #define TRUE 1
#endif

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef M_PI
  #define M_PI 3.14159265358979f
#endif

#ifndef rint
  #define rint(d) (floor((d)+0.5))
#endif

#ifndef WIN32
  #include <unistd.h>
#endif

#endif /* MING_LIBMING_INCLUDED */
