#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(7);
	SWFSoundStream *stream;
	int i;
	FILE *file;

	file = fopen(MEDIADIR "/video02.flv", "rb");
	if(!file) {
		return EXIT_FAILURE;
	}

	stream = new SWFSoundStream(file);
	if(!stream)
		return EXIT_FAILURE;

	m->setSoundStream(stream);
	for(i = 0; i < 200; i++)
		m->nextFrame();

	m->save("test02.swf");
	return 0;
}
