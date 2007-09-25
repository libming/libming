#include <libming.h>


int main()
{
	SWFMovie em = newSWFMovie();
	SWFMovie_save(em, "empty.swf");
	
	SWFMovie m = newSWFMovie();
	SWFPrebuiltClip pbc = newSWFPrebuiltClip_fromFile("empty.swf");
	SWFMovie_add(m, pbc);
	pbc = newSWFPrebuiltClip_fromFile("empty.swf");
	SWFMovie_add(m, pbc);

	SWFMovie_save(m, "test01.swf");

	return 0;
}
