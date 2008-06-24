#include <libming.h>
#include <stdlib.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFSoundStream stream;
	int i;
	FILE *file;
	file = fopen(MEDIADIR "/audio01.mp3", "rb");
	if(!file) {
		perror(MEDIADIR "/audio01.mp3");
		return EXIT_FAILURE;
	}
		
	stream = newSWFSoundStream(file);
	if(!stream)
	{
		fprintf(stderr, "Could not create SWFSoundStream\n");
		return EXIT_FAILURE;
	}
	SWFMovie_setSoundStream(m, stream);
	for(i = 0; i < 200; i++)
		SWFMovie_nextFrame(m);

	int ret = SWFMovie_save(m, "test01.swf");
	if ( ret == -1 )
	{
		fprintf(stderr, "Something went wrong during SWFMovie_save\n");
		return 1;
	}
	destroySWFSoundStream(stream);
	destroySWFMovie(m);
	return 0;
}
