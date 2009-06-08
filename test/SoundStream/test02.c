#include <libming.h>
#include <stdlib.h>


int main()
{
	SWFMovie m;
	SWFMovieClip mc;
	SWFSoundStream stream;
	FILE* file;
	int i;

	file = fopen(MEDIADIR "/audio01.mp3", "rb");
	if(!file)
	{
		perror(MEDIADIR "/audio01.mp3");
		return EXIT_FAILURE;
	}

	m = newSWFMovieWithVersion(7);
	stream = newSWFSoundStream(file);
	if(stream == NULL)
	{
		fprintf(stderr, "SoundStream failed\n");
		return EXIT_FAILURE;
	}

	mc = newSWFMovieClip();
	SWFMovieClip_setSoundStream(mc, stream, SWFMovie_getRate(m));
	for(i = 0; i < 200; i++)
		SWFMovieClip_nextFrame(mc);	
	
	SWFMovie_add(m, mc);
	SWFMovie_nextFrame(m);

	SWFMovie_save(m, "test02.swf");
	fclose(file);
	return EXIT_SUCCESS;
}
