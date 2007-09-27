#include <libming.h>

static char *test = "test01";

main()
{

SWFMovie m;

m = newSWFMovie();
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_nextFrame(m);

SWFMovie_add(m, (SWFBlock)compileSWFActionCode("var a = 1;"));

SWFMovie_save(m,"test01.swf");

return 0;
}
