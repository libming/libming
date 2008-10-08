#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(7);
		SWFSound *sound;
		int i;
		FILE *file;

		file = fopen(MEDIADIR "/audio01.mp3", "rb");
		if(!file) {
			return EXIT_FAILURE;
		}

		sound = new SWFSound(new SWFSoundStream(file));
		m->addExport(sound, "audio01.mp3");
		m->startSound(sound);

		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
