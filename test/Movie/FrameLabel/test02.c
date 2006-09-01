#include <libming.h>

static char *test = "test01";

main()
{
SWFMovie m;

m= newSWFMovie();

SWFMovie_namedAnchor(m,"frame0");

SWFMovie_save(m,"test02.swf");

return 0;
}
