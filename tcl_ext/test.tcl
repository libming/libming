load ./.libs/mingc.so mingc
set shape [ newSWFShape ]
SWFShape_setLine $shape 4 100 0 0 255
SWFShape_setRightFill $shape [ SWFShape_addSolidFill $shape 255 0 0 255 ]
SWFShape_movePenTo $shape 10 10
SWFShape_drawLineTo $shape 310 10
SWFShape_drawLineTo $shape 310 230
SWFShape_drawCurveTo $shape 10 230 10 10

set movie [newSWFMovie  ]
SWFMovie_setDimension $movie  320  240 
SWFMovie_add $movie $shape 
SWFMovie_nextFrame $movie
SWFMovie_save $movie "test.swf"

