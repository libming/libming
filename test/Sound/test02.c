#include <libming.h>
#include <stdlib.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFSound sound;
	FILE *file;
	int ret;

	file = fopen(MEDIADIR "/video02.flv", "rb");
	if(!file) {
		perror(MEDIADIR "/video02.flv");
		return EXIT_FAILURE;
	}
		
	sound = newSWFSound_fromSoundStream(newSWFSoundStream(file));
	if(!sound)
	{
		fprintf(stderr, "Could not create SWFSound\n");
		return EXIT_FAILURE;
	}
	SWFMovie_startSound(m, sound);

	ret = SWFMovie_save(m, "test02.swf");
	if ( ret == -1 )
	{
		fprintf(stderr, "Something went wrong during SWFMovie_save\n");
		return EXIT_FAILURE;
	}

	return 0;
}
