#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(7);
	SWFSound *sound;
	int i;
	FILE *file;

	file = fopen(MEDIADIR "/audio01.mp3", "rb");
	if(!file) {
		return 0;
	}

	sound = new SWFSound(new SWFSoundStream(file));
	if(!sound)
		return 0;

	m->startSound(sound);

	m->save("test01.swf");
	return 0;
}
