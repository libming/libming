
Zwiff is a quick hack to let folks play with ming (http://ming.sf.net) using Zope.

Beware... due to the way ming handles some errors, Zwiff can crash Zope ;-(. 

Yeah.. we're thinking about fixing that. ;-)

SJS

In a Python Script try something like::

	## Script (Python) "testMing"
	##bind container=container
	##bind context=context
	##bind namespace=
	##bind script=script
	##bind subpath=traverse_subpath
	##parameters=
	##title=
	##

	from ming import SWFMovie, SWFMorph
	from Products.Zwiff import dumbBuffer, MingLocker
	
	b = dumbBuffer(ext='swf')
	
	lock = MingLocker() # you want this... keeps different ming users from stomping on globals
	
	p = SWFMorph()
	                                                                                                                             
	s = p.getShape1()
	s.setLine(0,0,0,0)
	s.setLeftFill(s.addFill(0xff, 0, 0))
	s.movePenTo(-100,-100)
	s.drawLine(200,0)
	s.drawLine(0,200)
	s.drawLine(-200,0)
	s.drawLine(0,-200)
	                                                                                                                             
	s = p.getShape2()
	s.setLine(6,0,0,0)
	s.setLeftFill(s.addFill(0, 0, 0xff))
	s.movePenTo(0,-100)
	s.drawLine(100,100)
	s.drawLine(-100,100)
	s.drawLine(-100,-100)
	s.drawLine(100,-100)
	                                                                                                                             
	m = SWFMovie()
	m.setDimension(320, 240)
	m.setBackground(0xff, 0xff, 0xff)
	                                                                                                                             
	i = m.add(p)
	i.moveTo(160,120)
	                                                                                                                             
	for j in range(10):
	    r = (j*1.0)/10.0
	    i.setRatio(r)
	    m.nextFrame()
	                                                                                                                             
	m.save(b.filename())
	
	context.REQUEST.RESPONSE.setHeader('content-type','application/x-shockwave-flash')
	context.REQUEST.RESPONSE.setHeader('expires','Wed, 31-Dec-97 23:59:59 GMT')
	context.REQUEST.RESPONSE.setHeader('Cache-Control','no-store, no-cache, must-revalidate')
	context.REQUEST.RESPONSE.setHeader('Cache-Control','post-check=0, pre-check=0')
	context.REQUEST.RESPONSE.setHeader('Pragma','no-cache')
	
	return b.read()
	