#include <libming.h>

int
main()
{
SWFMovie m;
SWFMovieClip mc;

m = newSWFMovie();
mc = newSWFMovieClip();
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_add(m, (SWFBlock)mc);

SWFMovie_save(m, "test02.swf");

return 0;
}
