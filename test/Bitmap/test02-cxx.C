#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(8);
		SWFBitmap *bmp = new SWFBitmap(MEDIADIR "/image01.jpeg");

		m->addExport(bmp, "BitmapExport");
		m->writeExports();

		m->save("test02.swf");
	}
	catch (SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;

}
