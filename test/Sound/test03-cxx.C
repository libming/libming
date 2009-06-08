#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(7);
		SWFSound *sound, *sound2;
		int i;
		FILE *file;

		file = fopen(MEDIADIR "/sound1.mp3", "rb");
		if(!file) {
			return EXIT_FAILURE;
		}

		sound = new SWFSound(file, SWF_SOUND_MP3_COMPRESSED);
		m->addExport(sound, "sound1.mp3");
		m->startSound(sound);

		sound2 = new SWFSound(file, SWF_SOUND_MP3_COMPRESSED);
		sound2->delaySeek(2048);
		m->addExport(sound2, "delaySeeked");

		m->save("test03.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
