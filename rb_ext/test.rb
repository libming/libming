require 'mingc'
include Mingc

s = newSWFShape()
l = SWFShape_setLine(s, 40, 0x7f, 0, 0, 0xff)
f = SWFShape_addSolidFill(s, 0xff, 0, 0, 0xff)
SWFShape_setRightFill(s, f)
SWFShape_movePenTo(s, 1000, 1000)
SWFShape_drawLineTo(s, 6000, 4000)
SWFShape_end(s)

m = newSWFMovie()
SWFMovie_setDimension(m, 6400, 4800)
SWFMovie_setBackground(m, 0xff, 0, 0)
SWFMovie_setRate(m, 12.0)
SWFMovie_add(m, s)
SWFMovie_nextFrame(m)

SWFMovie_save(m, "pippo.swf")
