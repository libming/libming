#include <mingpp.h>
#include <cstdlib>
#include <cstdio>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(7);
		SWFSoundStream *stream;
		int i;
		FILE *file;

		file = fopen(MEDIADIR "/mpeg1.mp3", "rb");
		if(!file) {
			return EXIT_FAILURE;
		}

		stream = new SWFSoundStream(file);
		m->setSoundStream(stream);
		for(i = 0; i < 12; i++)
			m->nextFrame();

		m->save("test04.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
