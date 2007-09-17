#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(7);
	SWFSoundStream *stream;
	int i;
	FILE *file;

	file = fopen(MEDIADIR "/audio01.mp3", "rb");
	if(!file) {
		return 0;
	}

	stream = new SWFSoundStream(file);
	if(!stream)
		return 0;

	m->setSoundStream(stream);
	for(i = 0; i < 200; i++)
		m->nextFrame();

	m->save("test01.swf");
	return 0;
}
