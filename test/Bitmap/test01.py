#!/usr/bin/python
from ming import *

import sys
srcdir=sys.argv[1]
mediadir=srcdir+'/../Media'

Ming_useSWFVersion(8);

m =  SWFMovie();

Ming_setScale(1.0);
#Unknown block type 69

# SWF_DEFINELOSSLESS 

# Bitmap 1 (lossless). To extract:
# swfextract -p 1 -o character1.png $swf
character1 =  SWFBitmap(mediadir + '/image01.png');

# SWF_EXPORTASSETS 
m.addExport(character1,'BitmapExport');
m.writeExports();

# SWF_SHOWFRAME 
m.nextFrame(); # end of frame 1 

# SWF_END 

m.save("test01.swf");
