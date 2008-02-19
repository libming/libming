#include <libming.h>

int main()
{
SWFMovie m;

m= newSWFMovie();
SWFMovie_setBackground(m, 0xff, 0xff, 0xff);
SWFMovie_setRate(m, 24.00);

SWFMovie_save(m,"test01.swf");

return 0;
}
