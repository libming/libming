#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFCharacter c = SWFMovie_importCharacter(m, "test.swf", "test");
	SWFMovie_add(m, c);
	SWFMovie_save(m, "test02.swf");
	return 0;
}	
