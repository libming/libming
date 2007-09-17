#include <libming.h>

int main() {
	SWFMovie m = newSWFMovieWithVersion(7);
	SWFVideoStream stream;
	int i;
	FILE *file;
	file = fopen(MEDIADIR "/video01.flv", "rb");
	if(!file) 
		return 1;
		
	stream = newSWFVideoStream_fromFile(file);
	if(!stream)
		return 1;

	SWFVideoStream_setDimension(stream, 200,200);
	int frames = SWFVideoStream_getNumFrames(stream);
        SWFMovie_add(m, (SWFBlock)stream);
	for(i = 0; i < frames; i++)
		SWFMovie_nextFrame(m);

	SWFMovie_save(m, "test01.swf");
	return 0;
}
