require 'ming'

s = SWFShape.new
s.moveTo(200, 200)
s.setLine(40, 0x7f, 0, 0, 0xff)
s.setRightFill(s.addSolidFill(0xff, 0, 0, 0xff))
s.lineTo(6200, 200)
s.lineTo(6200, 4600)
s.curveTo(200, 4600, 200, 200)

m = SWFMovie.new
m.setDimension(6400, 4800)
m.setRate(12.0)
m.add(s)
m.nextFrame

m.save('shape.swf')
