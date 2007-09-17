#include <libming.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFSoundStream stream;
	int i;
	FILE *file;
	
	file = fopen(MEDIADIR "/video02.flv", "rb");
	if(!file) {
		perror(MEDIADIR "/video02.flv");
		return 1;
	}
		
	stream = newSWFSoundStream(file);
	if(!stream)
	{
		fprintf(stderr, "Could not create SWFSoundStream\n");
		return 1;
	}
	SWFMovie_setSoundStream(m, stream);
	for(i = 0; i < 200; i++)
		SWFMovie_nextFrame(m);

	SWFMovie_save(m, "test02.swf");
	return 0;
}
