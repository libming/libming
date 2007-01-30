#include <libming.h>

static char *test = "test02";

main()
{

SWFMovie m;

m = newSWFMovie();

SWFMovie_nextFrame(m);

SWFMovie_add(m, (SWFBlock)compileSWFActionCode("var a = 1;"));

SWFMovie_nextFrame(m);

SWFMovie_save(m, "test02.swf");

return 0;
}
