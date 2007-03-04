#include <libming.h>

static char *test = "test06";

main()
{
SWFMovie m;

m= newSWFMovieWithVersion(8);

SWFMovie_save(m,"test06.swf");

return 0;
}
