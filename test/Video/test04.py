#!/usr/bin/python
from ming import *
import sys

srcdir=sys.argv[1]

Ming_useSWFVersion(7);
m =  SWFMovie();

stream = SWFVideoStream(srcdir + "/../Media/video01.flv");
stream.setDimension(200, 200)
stream.setFrameMode(SWFVIDEOSTREAM_MODE_MANUAL);

m.add(stream);

numFrames = stream.getNumFrames()
for i in range(numFrames):
	if i % 2 : stream.nextFrame();
	m.nextFrame();

m.save("test04.swf");
