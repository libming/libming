#include <libming.h>

static char *test = "test01";

int main()
{
SWFMovie m;

m= newSWFMovie();

SWFMovie_setBackground(m, 255, 0, 0);

SWFMovie_save(m,"test01.swf");

return 0;
}
