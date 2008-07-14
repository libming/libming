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
		
		SWFMovieClip *mc = new SWFMovieClip();
		mc->add(stream);
	
		int i;
		for(i = 0; i < frames; i++)
			mc->nextFrame();
		m->add(mc);
		m->nextFrame();
		m->save("test05.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
