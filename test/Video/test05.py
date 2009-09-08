#!/usr/bin/python
from ming import *
import sys

srcdir=sys.argv[1]

Ming_useSWFVersion(7);
m =  SWFMovie();

stream = SWFVideoStream(srcdir + "/../Media/video01.flv");

mc = SWFMovieClip();
mc.add(stream);

numFrames = stream.getNumFrames()
for i in range(numFrames):
	mc.nextFrame();

m.add(mc);
m.nextFrame();


m.save("test05.swf");
