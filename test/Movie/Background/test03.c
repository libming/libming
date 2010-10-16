#include <libming.h>

static char *test = "test03";

int main()
{
SWFMovie m;

m= newSWFMovie();

SWFMovie_setBackground(m, 0, 0, 255);

SWFMovie_save(m,"test03.swf");

return 0;
}
