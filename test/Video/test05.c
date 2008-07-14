#include <libming.h>
#include <stdlib.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFVideoStream stream;
	int i;
	FILE *file;
	file = fopen(MEDIADIR "/video01.flv", "rb");
	if(!file) 
	{
		perror(MEDIADIR "/video01.flv");
		return EXIT_FAILURE;
	}
		
	stream = newSWFVideoStream_fromFile(file);
	if(!stream)
	{
		printf(stderr, "Could not create SWFVideoStream from file\n");
		return EXIT_FAILURE;
	}

	SWFVideoStream_setDimension(stream, 200,200);
	int frames = SWFVideoStream_getNumFrames(stream);
	SWFMovieClip mc  = newSWFMovieClip();
        SWFMovieClip_add(mc, stream);
	for(i = 0; i < frames; i++)
		SWFMovieClip_nextFrame(mc);

	SWFMovie_add(m, mc);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test05.swf");
	return 0;
}
