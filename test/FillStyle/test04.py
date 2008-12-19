#!/usr/bin/python
from ming import *
import sys

srcdir=sys.argv[1]

Ming_useSWFVersion(8);
m =  SWFMovie();

bitmap = SWFBitmap(srcdir + "/../Media/image01.dbl")
shape = SWFShape()
fill = shape.addFill(bitmap, SWFFILL_CLIPPED_BITMAP)
shape.setRightFill(fill)
shape.setLine(1, 0, 0, 0, 255)
shape.drawLine(100, 0)
shape.drawLine(0, 100)
shape.drawLine(-100, 0)
shape.drawLine(0, -100)
m.add(shape)
m.save("test04.swf")
