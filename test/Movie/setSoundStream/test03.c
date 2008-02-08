#include <libming.h>
#include <stdlib.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFSoundStream stream;
	int i;
	FILE *file;
	
	file = fopen(MEDIADIR "/nellymoser.flv", "rb");
	if(!file) {
		perror(MEDIADIR "/nellymoser.flv");
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

	SWFMovie_save(m, "test03.swf");
	return 0;
}
