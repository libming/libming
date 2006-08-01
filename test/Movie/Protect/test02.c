#include <libming.h>

static char *test = "test02";

main()
{
SWFMovie m;

m= newSWFMovie();

SWFMovie_protect(m,"foobar");

SWFMovie_save(m,"test02.swf");

return 0;
}
