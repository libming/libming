
from ming import *

Ming_setScale(1.0)

s = SWFShape()
l = s.setLine(4, 0x7f, 0, 0)
f = s.addSolidFill(0xff, 0, 0)
s.movePenTo(10, 10)
s.setRightFill(f)
s.drawLineTo(310, 10)
s.drawLineTo(310, 230)
s.drawCurveTo(10, 230, 10, 10)

m = SWFMovie()
m.setDimension(320, 240)
m.setRate(12.0)
m.add(s)
m.nextFrame()

m.save("test.swf")

