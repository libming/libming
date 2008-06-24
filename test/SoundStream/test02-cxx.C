#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(7);
		FILE *file;
		int i;
		
		file = fopen(MEDIADIR "/audio01.mp3", "rb");
                if(!file) {
                        return EXIT_FAILURE;
                }
		
		SWFMovieClip *mc = new SWFMovieClip();
		SWFSoundStream *stream = new SWFSoundStream(file);
		mc->setSoundStream(stream, m->getRate());
		for(i = 0; i < 200; i++)
			mc->nextFrame();		
		
		m->add(mc);
		m->nextFrame();
		m->save("test02.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
                return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
