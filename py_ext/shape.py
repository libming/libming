
from ming import *

Ming_setScale(1.0)

s = SWFShape()
s.setLine(40, 0x7f, 0, 0)
s.setRightFill(s.addFill(0xff, 0, 0))
s.movePenTo(200, 200)
s.drawLineTo(6200, 200)
s.drawLineTo(6200, 4600)
s.drawCurveTo(200, 4600, 200, 200)

m = SWFMovie()
m.setDimension(6400, 4800)
m.setRate(12.0)
m.add(s)
m.nextFrame()

m.save("test.swf")

