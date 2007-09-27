#include <libming.h>

//static char *test = "test01";

int
main()
{
SWFMovie m;

m= newSWFMovie();
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_labelFrame(m,"frame0");

SWFMovie_save(m,"test01.swf");

return 0;
}
