#include <libming.h>

static char *test = "test01";

int
main()
{
SWFMovie m;

m = newSWFMovie();

SWFMovie_add(m, (SWFBlock)compileSWFActionCode("var a = 1;"));

SWFMovie_save(m, "test01.swf");

return 0;
}
