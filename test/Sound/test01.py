#!/usr/bin/python
from ming import *

import sys
mediadir = sys.argv[1] + '/../Media'

Ming_useSWFVersion(7)

m =  SWFMovie()

stream = SWFSoundStream(mediadir + '/audio01.mp3')
sound = SWFSound(stream)

m.addExport(sound, 'audio01.mp3')
m.startSound(sound)

m.save("test01.swf")
