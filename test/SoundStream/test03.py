#!/usr/bin/python
from ming import *

import sys
mediadir = sys.argv[1]+'/../Media'

Ming_useSWFVersion(7);
m =  SWFMovie();

mc = SWFMovieClip(); 

stream = SWFSoundStream(mediadir + '/audio01.mp3')
stream.setInitialMp3Delay(16384);

mc.setSoundStream(stream, m.getRate())
for i in range (0, 200):
	mc.nextFrame()



m.add(mc);

m.save("test03.swf");
