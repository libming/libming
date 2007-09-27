#include <libming.h>

static char *test = "test01";

main()
{
SWFMovie m;

m= newSWFMovie();
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_save(m,"test01.swf");

return 0;
}
