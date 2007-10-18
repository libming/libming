#include <libming.h>

main()
{
SWFMovie m;

m= newSWFMovieWithVersion(8);
SWFMovie_setBackground(m, 0, 0, 0);
SWFMovie_save(m,"test07.swf");

return 0;
}
