#!/usr/bin/python
from ming import *

import sys
mediadir = sys.argv[1]+'/../Media'

Ming_useSWFVersion(7);
m =  SWFMovie()
m.setRate(1)

stream = SWFSoundStream(mediadir + '/sound1.mp3')

mc =  SWFMovieClip()
mc.setSoundStream(stream, 1)
mc.nextFrame()
mc.nextFrame()

m.add(mc);
m.nextFrame(); 
m.save("test01.swf");
