#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(7);
	SWFSound *sound;
	int i;
	FILE *file;

	file = fopen(MEDIADIR "/video02.flv", "rb");
	if(!file) {
		return 0;
	}

	sound = new SWFSound(new SWFSoundStream(file));
	if(!sound)
		return 0;

	m->startSound(sound);

	m->save("test02.swf");
	return 0;
}
