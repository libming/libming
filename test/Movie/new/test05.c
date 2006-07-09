#include <libming.h>

static char *test = "test05";

main()
{
SWFMovie m;

m= newSWFMovieWithVersion(7);

SWFMovie_save(m,"test05.swf");

return 0;
}
