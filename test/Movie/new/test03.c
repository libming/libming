#include <libming.h>

static char *test = "test03";

main()
{
SWFMovie m;

m= newSWFMovieWithVersion(5);
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_save(m,"test03.swf");

return 0;
}
