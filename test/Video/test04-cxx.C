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
		stream->setFrameMode(SWFVIDEOSTREAM_MODE_MANUAL);
		int frames = stream->getNumFrames();
		m->add(stream);
	
		int i;
		for(i = 0; i < frames; i++)
		{
			if(i & 1)
				stream->nextFrame();
			m->nextFrame();
		}
		m->save("test04.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
