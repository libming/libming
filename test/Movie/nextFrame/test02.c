#include <libming.h>

int main()
{

SWFMovie m;

m = newSWFMovie();

SWFMovie_nextFrame(m);
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_add(m, (SWFBlock)newSWFAction("var a = 1;"));

SWFMovie_nextFrame(m);

SWFMovie_save(m, "test02.swf");

return 0;
}
