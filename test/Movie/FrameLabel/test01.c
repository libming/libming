#include <libming.h>

//static char *test = "test01";

int
main()
{
SWFMovie m;

m= newSWFMovie();

SWFMovie_labelFrame(m,"frame0");

SWFMovie_save(m,"test01.swf");

return 0;
}
