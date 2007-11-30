#include <libming.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFSound sound;
	FILE *file;
	int ret;

	file = fopen(MEDIADIR "/video02.flv", "rb");
	if(!file) {
		perror(MEDIADIR "/video02.flv");
		return 1;
	}
		
	sound = newSWFSound_fromSoundStream(newSWFSoundStream(file));
	if(!sound)
	{
		fprintf(stderr, "Could not create SWFSound\n");
		return 1;
	}
	SWFMovie_startSound(m, sound);

	ret = SWFMovie_save(m, "test02.swf");
	if ( ret == -1 )
	{
		fprintf(stderr, "Something went wrong during SWFMovie_save\n");
		return 1;
	}

	return 0;
}
