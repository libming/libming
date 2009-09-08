#!/usr/bin/python
from ming import *
import sys
import os

srcdir=sys.argv[1]

Ming_useSWFVersion(7);
m = SWFMovie();

stream = SWFVideoStream(srcdir + "/../Media/video01.flv");
stream.setDimension(200, 200);
stream.setFrameMode(SWFVIDEOSTREAM_MODE_MANUAL);

m.add(stream);

for i in range(50):
	if i == 30 : off = stream.seek(50, os.SEEK_SET);
	if i == 40 : stream.seek(off, os.SEEK_SET);
	stream.nextFrame();
	m.nextFrame();

m.save("test06.swf");
