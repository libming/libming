#include <libming.h>
#include <stdlib.h>


int main()
{
	SWFMovie m;
	SWFMovieClip mc;
	SWFSoundStream stream;
	FILE* file;

	file = fopen(MEDIADIR "/sound1.mp3", "rb");
	if(!file)
	{
		perror(MEDIADIR "/sound1.mp3");
		return EXIT_FAILURE;
	}

	m = newSWFMovieWithVersion(7);
	SWFMovie_setRate(m, 1);

	stream = newSWFSoundStream(file);
	if(stream == NULL)
	{
		fprintf(stderr, "SoundStream failed\n");
		return EXIT_FAILURE;
	}

	mc = newSWFMovieClip();
	SWFMovieClip_setSoundStream(mc, stream, 1);
	SWFMovieClip_nextFrame(mc);	
	SWFMovieClip_nextFrame(mc);	
	
	SWFMovie_add(m, mc);
	SWFMovie_nextFrame(m);

	SWFMovie_save(m, "test01.swf");
	fclose(file);
	return EXIT_SUCCESS;
}
