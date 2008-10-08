#include <libming.h>
#include <stdlib.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFSound sound;
	FILE *file;
	int ret;

	file = fopen(MEDIADIR "/sound1.mp3", "rb");
	if(!file) {
		perror(MEDIADIR "/sound1.mp3");
		return EXIT_FAILURE;
	}
		
	sound = newSWFSound(file, SWF_SOUND_MP3_COMPRESSED);
	if(!sound)
	{
		fprintf(stderr, "Could not create SWFSound\n");
		return EXIT_FAILURE;
	}
	SWFMovie_addExport(m, sound, "sound1.mp3");
	SWFMovie_startSound(m, sound);

	ret = SWFMovie_save(m, "test03.swf");
	if ( ret == -1 )
	{
		fprintf(stderr, "Something went wrong during SWFMovie_save\n");
		return EXIT_FAILURE;
	}

	//SWFSound destroy SWFInput created from file
	destroySWFSound(sound);
	destroySWFMovie(m);
	
	return 0;
}
