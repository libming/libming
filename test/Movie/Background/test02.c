#include <libming.h>

static char *test = "test02";

int main()
{
SWFMovie m;

m= newSWFMovie();

SWFMovie_setBackground(m, 0, 255, 0);

SWFMovie_save(m,"test02.swf");

return 0;
}
