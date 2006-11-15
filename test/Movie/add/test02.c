#include <libming.h>

static char *test = "test02";

int
main()
{
SWFMovie m;
SWFMovieClip mc;

m = newSWFMovie();
mc = newSWFMovieClip();

SWFMovie_add(m, (SWFBlock)mc);

SWFMovie_save(m, "test02.swf");

return 0;
}
