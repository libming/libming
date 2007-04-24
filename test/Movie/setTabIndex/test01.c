#include <ming.h>


int main()
{
        SWFMovie m = newSWFMovieWithVersion(7);

	SWFMovie_setTabIndex(m, 1, 2);
	SWFMovie_setTabIndex(m, 2, 2);

        SWFMovie_save(m, "test01.swf");
        return 0;
}

