#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(7);
	SWFSoundStream *stream;
	int i;
	FILE *file;

	file = fopen(MEDIADIR "/video02.flv", "rb");
	if(!file) {
		return 0;
	}

	stream = new SWFSoundStream(file);
	if(!stream)
		return 0;

	m->setSoundStream(stream);
	for(i = 0; i < 200; i++)
		m->nextFrame();

	m->save("test02.swf");
	return 0;
}
