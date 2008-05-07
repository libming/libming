#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(7);
		FILE *file;
		
		file = fopen(MEDIADIR "/sound1.mp3", "rb");
                if(!file) {
                        return EXIT_FAILURE;
                }

		m->setRate(1);
		
		SWFMovieClip *mc = new SWFMovieClip();
		SWFSoundStream *stream = new SWFSoundStream(file);
		mc->setSoundStream(stream, 1);
		mc->nextFrame();	
		mc->nextFrame();	
		
		m->add(mc);
		m->nextFrame();
		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
                return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
