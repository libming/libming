#!/usr/bin/python
from ming import *
import sys

srcdir=sys.argv[1]

Ming_useSWFVersion(7);
m =  SWFMovie();

stream = SWFVideoStream(srcdir + "/../Media/video01.flv");

m.add(stream);

numFrames = stream.getNumFrames();
for i in range(numFrames):
	m.nextFrame();

m.save("test01.swf");
