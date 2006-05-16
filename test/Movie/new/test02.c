#include <libming.h>

static char *test = "test02";

main()
{
SWFMovie m;

m= newSWFMovieWithVersion(4);

SWFMovie_save(m,"test02.swf");

return 0;
}
