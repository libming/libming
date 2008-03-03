#include <mingpp.h>
#include <cstdlib>


int main()
{
	try {
		SWFMovie *m = new SWFMovie(7);
		FILE *file;
		file = fopen(MEDIADIR "/video01.flv", "rb");
		if(!file)
			return EXIT_FAILURE;
	
		SWFVideoStream *stream = new SWFVideoStream(file);
		stream->setDimension(200, 200);
		int frames = stream->getNumFrames();
		m->add(stream);
	
		int i;
		for(i = 0; i < frames; i++)
			m->nextFrame();

		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
