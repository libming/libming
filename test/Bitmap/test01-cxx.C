#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(8);
	SWFBitmap *bmp = new SWFBitmap(MEDIADIR "/image01.png");

	m->add(bmp);
	m->save("test01.swf");
	return 0;

}
