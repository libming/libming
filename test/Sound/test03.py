#!/usr/bin/python
from ming import *

import sys
mediadir = sys.argv[1] + '/../Media'

Ming_useSWFVersion(7)

m =  SWFMovie()

sound = SWFSound(mediadir + '/sound1.mp3', SWF_SOUND_MP3_COMPRESSED)

m.addExport(sound, 'sound1.mp3')
m.startSound(sound)

sound2 = SWFSound(mediadir + '/sound1.mp3', SWF_SOUND_MP3_COMPRESSED)
sound2.setInitialMp3Delay(2048)

m.addExport(sound2, 'delaySeeked')

m.save("test03.swf")
