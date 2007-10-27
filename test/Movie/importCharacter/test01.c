#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();
	SWFCharacter c = SWFMovie_importCharacter(m, "test.swf", "test");
	SWFMovie_add(m, c);
	SWFMovie_save(m, "test01.swf");
	return 0;
}	
