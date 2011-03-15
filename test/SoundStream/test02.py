#!/usr/bin/python
from ming import *

import sys
mediadir = sys.argv[1]+'/../Media'

Ming_useSWFVersion(7);
m =  SWFMovie();

mc =  SWFMovieClip(); 

stream = SWFSoundStream(mediadir + '/audio01.mp3')
mc.setSoundStream(stream, 12)
for i in range (0, 200):
	mc.nextFrame()

m.add(mc);
m.nextFrame(); 

m.save("test02.swf");
