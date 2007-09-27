#include <libming.h>

static char *test = "test02";

main()
{
SWFMovie m;

m= newSWFMovie();
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_protect(m,"foobar");

SWFMovie_save(m,"test02.swf");

return 0;
}
