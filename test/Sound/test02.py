#!/usr/bin/python
from ming import *

import sys
mediadir = sys.argv[1] + '/../Media'

Ming_useSWFVersion(7)

m =  SWFMovie()

stream = SWFSoundStream(mediadir + '/video02.flv')
sound = SWFSound(stream)

m.addExport(sound, 'video02.flv')
m.startSound(sound)

m.save("test02.swf")
