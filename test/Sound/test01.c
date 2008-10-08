#include <libming.h>
#include <stdlib.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFSound sound;
	SWFSoundStream stream;
	FILE *file;
	int ret;

	file = fopen(MEDIADIR "/audio01.mp3", "rb");
	if(!file) {
		perror(MEDIADIR "/audio01.mp3");
		return EXIT_FAILURE;
	}
		
	stream = newSWFSoundStream(file);
	if (!stream)
	{
		fprintf(stderr, "Could not create SWFSoundStream\n");
		return EXIT_FAILURE;
	}

	sound = newSWFSound_fromSoundStream(stream);
	if(!sound)
	{
		fprintf(stderr, "Could not create SWFSound\n");
		return EXIT_FAILURE;
	}
	SWFMovie_addExport(m, sound, "audio01.mp3");
	SWFMovie_startSound(m, sound);

	ret = SWFMovie_save(m, "test01.swf");
	if ( ret == -1 )
	{
		fprintf(stderr, "Something went wrong during SWFMovie_save\n");
		return EXIT_FAILURE;
	}

	//SWFSoundStream destroy SWFInput created from file
	destroySWFSoundStream(stream);
	destroySWFSound(sound);
	destroySWFMovie(m);

	return 0;
}
