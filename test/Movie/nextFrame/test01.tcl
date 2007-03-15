load mingc.so mingc
set m [newSWFMovie]
setFrames $m (2);

# SWF_SETBACKGROUNDCOLOR 
setBackground $m 0xff  0xff  0xff

# SWF_SHOWFRAME 
nextFrame $m  # end of frame 1 

# SWF_DOACTION 
add $m (new SWFAction("var a = 1;\
") );

# SWF_SHOWFRAME 
nextFrame $m  # end of frame 2 

# SWF_END 

save $m "test01.swf"
