#include <libming.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFSoundStream stream;
	int i;
	FILE *file;
	file = fopen("../../Media/audio01.mp3", "rb");
	if(!file) {
		return 0;
	}
		
	stream = newSWFSoundStream(file);
	if(!stream)
		return 0;
	SWFMovie_setSoundStream(m, stream);
	for(i = 0; i < 200; i++)
		SWFMovie_nextFrame(m);

	SWFMovie_save(m, "test01.swf");
	return 0;
}
