#include <libming.h>

static char *test = "test02";

main()
{
SWFMovie m;

m= newSWFMovieWithVersion(4);
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_save(m,"test02.swf");

return 0;
}
