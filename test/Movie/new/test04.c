#include <libming.h>

static char *test = "test04";

main()
{
SWFMovie m;

m= newSWFMovieWithVersion(6);
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_save(m,"test04.swf");

return 0;
}
