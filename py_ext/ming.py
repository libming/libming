
import mingc

# should raise an exception if return is non-zero:
mingc.Ming_init()

def Ming_setCubicThreshold(t):
    mingc.Ming_setCubicThreshold(t)

def Ming_setScale(scale):
    mingc.Ming_setScale(scale)

def Ming_getScale():
    return mingc.Ming_getScale()

def Ming_useSWFVersion(num):
    mingc.Ming_useSWFVersion(num)

def Ming_setSWFCompression( level ):
    return mingc.Ming_setSWFCompression( level )

def Ming_useConstants(flag):
   mingc.Ming_useConstants(flag)

def Ming_collectGarbage():
   mingc.Ming_collectGarbage()

class SWFBase:

    def __init__(self, o):
        self.this = o

    # reportedly useful for zope/zwiff:
    def set(self, name, val):
        setattr(self, name, val)


class SWFCXform(SWFBase):
	
    def __init__(rAdd=0, gAdd=0, bAdd=0, aAdd=0, rMult=1.0, gMult=1.0, bMult=1.0, aMult=1.0):
        self.this = mingc.newSWFCXform(rAdd, gAdd, bAdd, aAdd, rMult, gMult, bMult, aMult)

    def __del__(self):
        mingc.destroySWFCXform(self.this)

    def setColorAdd(self, rAdd, gAdd, bAdd, aAdd=0xff):
        mingc.SWFCXform_setColorAdd(self.this, rAdd, gAdd, bAdd, aAdd)

    def setColorMult(self, rMult, gMult, bMult, aMult):
        mingc.SWFCXform_setColorMult(self.this, rMult, gMult, bMult, aMult)

class SWFRect(SWFBase):

    def __init__(self, minX, maxX, minY, maxY):
        self.this = mingc.newSWFRect(minX, maxX, minY, maxY)

    def __del__(self):
        mingc.destroySWFRect(self.this)

    def getWidth(self):
        return mingc.SWFRect_getWidth(self.this)

    def getHeight(self):
        return mingc.SWFRect_getHeight(self.this)

class SWFShadow(SWFBase):
    def __init__(self, angle, distance, strength):
        self.this = mingc.newSWFShadow(angle, distance, strength)

    def __del__(self):
        mingc.destroySWFShadow(self.this)

class SWFBlur(SWFBase):
    def __init__(self, blurX, blurY, passes):
        self.this = mingc.newSWFBlur(blurX, blurY, passes)

    def __del__(self):
        mingc.destroySWFBlur(self.this)

class SWFFilterMatrix(SWFBase):
    def __init__(self, cols, rows, values):
        vals = floatArray(cols * rows)
        for i in range(0, cols*rows):
            vals[i] = values[i]
        self.this = mingc.newSWFFilterMatrix(cols, rows, vals)

    def __del__(self):
        mingc.destroySWFFilterMatrix(self.this)

SWFFILTER_TYPE_DROPSHADOW = mingc.SWFFILTER_TYPE_DROPSHADOW
SWFFILTER_TYPE_BLUR = mingc.SWFFILTER_TYPE_BLUR
SWFFILTER_TYPE_GLOW = mingc.SWFFILTER_TYPE_GLOW
SWFFILTER_TYPE_BEVEL = mingc.SWFFILTER_TYPE_BEVEL
SWFFILTER_TYPE_GRADIENTGLOW = mingc.SWFFILTER_TYPE_GRADIENTGLOW
SWFFILTER_TYPE_CONVOLUTION = mingc.SWFFILTER_TYPE_CONVOLUTION
SWFFILTER_TYPE_COLORMATRIX = mingc.SWFFILTER_TYPE_COLORMATRIX
SWFFILTER_TYPE_GRADIENTBEVEL = mingc.SWFFILTER_TYPE_GRADIENTBEVEL

SWFFILTER_MODE_INNER = mingc.SWFFILTER_MODE_INNER
SWFFILTER_MODE_KO = mingc.SWFFILTER_MODE_KO
SWFFILTER_MODE_COMPOSITE = mingc.SWFFILTER_MODE_COMPOSITE
SWFFILTER_MODE_ONTOP = mingc.SWFFILTER_MODE_ONTOP

SWFFILTER_FLAG_CLAMP = mingc.SWFFILTER_FLAG_CLAMP
SWFFILTER_FLAG_PRESERVE_ALPHA = mingc.SWFFILTER_FLAG_PRESERVE_ALPHA

class SWFFilter(SWFBase):
    def __del__(self):
        mingc.destroySWFFilter(self.this) 	

    def __init__(self, type, arg1=None, arg2=None, arg3=None, arg4=None, arg5=None):
        if type == SWFFILTER_TYPE_DROPSHADOW:
            #color, blur, shadow, flags
            if not isinstance(arg2, SWFBlur):
                raise AttributeError, "3. parameter has to be SWFBlur"
            if not isinstance(arg3, SWFShadow):
                raise AttributeError, "4. parameter has to be SWFShadow"
            self.this = mingc.newDropShadowFilter(arg1, arg2.this, arg3.this, arg4)
        elif type == SWFFILTER_TYPE_BLUR:
            #blur
            if not isinstance(arg1, SWFBlur):
                raise AttributeError, "2. parameter has to be SWFBlur"
            self.this = mingc.newBlurFilter(arg1.this)
        elif type == SWFFILTER_TYPE_GLOW:
            #color, blur, strength, flags
            if not isinstance(arg2, SWFBlur):
                raise AttributeError, "3. parameter has to be SWFBlur"
            self.this = mingc.newGlowFilter(arg1, arg2.this, arg3, arg4)
	elif type == SWFFILTER_TYPE_BEVEL:
            #shadowColor, highlightColor, blur, shadow, flags
            if not isinstance(arg3, SWFBlur):
                raise AttributeError, "4. parameter has to be SWFBlur"
            if not isinstance(arg4, SWFShadow):
                raise AttributeError, "5. parameter has to be SWFShadow"
            self.this = mingc.newBevelFilte(arg1, arg2, arg3.this, arg4.this, arg5)
	elif type == SWFFILTER_TYPE_GRADIENTGLOW:
            #gradient, blur, shadow, flags
            if not isinstance(arg1, SWFGradient):
                raise AttributeError, "2. parameter has to be SWFGradient"
            if not isinstance(arg2, SWFBlur):
                raise AttributeError, "3. parameter has to be SWFBlur"
            if not isinstance(arg3, SWFShadow):
                raise AttributeError, "4. parameter has to be SWFShadow"
            self.this = mingc.newGradienGlowFilter(arg1.this, arg2.this, arg3.this, arg4)
        elif type == SWFFILTER_TYPE_COLORMATRIX:
            #colormatrix
            if not isinstance(arg1, SWFFilterMatrix):
                raise AttributeError, "2. parameter has to be SWFFilterMatrix"
            self.this = mingc.newColorMatrixFilter(arg1.this)
	elif type == SWFFILTER_TYPE_CONVOLUTION:
            #colormatrix, divisor, bias, color, flags
            if not isinstance(arg1, SWFFilterMatrix):
                raise AttributeError, "2. parameter has to be SWFFilterMatrix"
            self.this = mingc.newConvolutionFilter(arg1.this, arg2, arg3, arg4, arg5)
        elif type == SWFFILTER_TYPE_GRADIENTBEVEL:
            #gradient, blur, shadow, flags
            if not isinstance(arg1, SWFGradient):
                raise AttributeError, "2. parameter has to be SWFGradient"
            if not isinstance(arg2, SWFBlur):
                raise AttributeError, "3. parameter has to be SWFBlur"
            if not isinstance(arg3, SWFShadow):
                raise AttributeError, "4. parameter has to be SWFShadow"
            self.this = mingc.newGradientBevelFilter(arg1.this, arg2.this, arg3.this, arg4)
	else:
            raise AttributeError, "bad filter type to SWFFilter::new"

class SWFMatrix(SWFBase):
    
    def getScaleX(self):
        return mingc.SWFMatrix_getScaleX(self.this)

    def getScaleY(self):
        return mingc.SWFMatrix_getScaleY(self.this)

    def getRotate0(self):
        return mingc.SWFMatrix_getRotate0(self.this)

    def getRotate1(self):
        return mingc.SWFMatrix_getRotate1(self.this)

    def getTranslateX(self):
        return mingc.SWFMatrix_getTranslateX(self.this)

    def getTranslateY(self):
        return mingc.SWFMatrix_getTranslateY(self.this)

SWF_SHAPE3			= mingc.SWF_SHAPE3
SWF_SHAPE4			= mingc.SWF_SHAPE4
SWF_SHAPE_USESCALINGSTROKES	= mingc.SWF_SHAPE_USESCALINGSTROKES
SWF_SHAPE_USENONSCALINGSTROKES	= mingc.SWF_SHAPE_USENONSCALINGSTROKES

class SWFShape(SWFBase):

    def __init__(self, o=None):
        self.fills = []

        if o is None:
            self.this = mingc.newSWFShape()
        else:
            self.this = o

    def __del__(self):
        mingc.destroySWFShape(self.this)

    def setLine(self, width, r, g, b, a=0xff):
        mingc.SWFShape_setLine(self.this, width, r, g, b, a)

    def setLine2(self, width, r, g, b, a, flags, miter):
        mingc.SWFShape_setLine2(self.this, width, r, g, b, a, flags, miter)

    def setLine2Filled(self, width, fill, flags, miter):
        mingc.SWFShape_setLine2Filled(self.this, width, mingc.SWFFill_getFillStyle(fill.this), flags, miter)

    # I know there's probably a better way to do this..
    def addFill(self, arg1, arg2=0, arg3=None, arg4=0xff):
        if arg3 != None:
            return SWFFill(mingc.SWFShape_addSolidFill(self.this, arg1, arg2, arg3, arg4))
        if isinstance(arg1, SWFGradient):
            # XXX - have to keep reference to gradient so it's not disposed
            self.fills.append(arg1)
            return SWFFill(mingc.SWFShape_addGradientFill(self.this, arg1.this, arg2))

        if isinstance(arg1, SWFBitmap):
            # XXX - have to keep reference to bitmap so it's not disposed
            self.fills.append(arg1)
            return SWFFill(mingc.SWFShape_addBitmapFill(self.this, arg1.this, arg2))

        if isinstance(arg1, SWFJpegBitmap):
            self.fills.append(arg1)
            return SWFFill(mingc.SWFShape_addBitmapFill(self.this, arg1.this, arg2))

        else:
            raise AttributeError, "bad argument to SWFShape::addFill"

    def setLeftFill(self, fill):
        mingc.SWFShape_setLeftFill(self.this, fill)

    def setRightFill(self, fill):
        mingc.SWFShape_setRightFill(self.this, fill)

    def movePenTo(self, x, y):
        mingc.SWFShape_movePenTo(self.this, x, y)

    def movePen(self, x, y):
        mingc.SWFShape_movePen(self.this, x, y)

    def drawLineTo(self, x, y):
        mingc.SWFShape_drawLineTo(self.this, x, y)

    def drawLine(self, dx, dy):
        mingc.SWFShape_drawLine(self.this, dx, dy)

    def drawArc(self, x, y, r, startAngle, endAngle):
        mingc.SWFShape_drawArc(self.this, x, y, r, startAngle, endAngle)

    def drawCircle(self, r):
        mingc.SWFShape_drawCircle(self.this, r)

    def drawCurveTo(self, bx, by, cx, cy, dx=None, dy=None):
        if(dx != None):
            mingc.SWFShape_drawCubicTo(self.this, bx, by, cx, cy, dx, dy)
        else:
            mingc.SWFShape_drawCurveTo(self.this, bx, by, cx, cy)

    def drawCurve(self, bx, by, cx, cy, dx=None, dy=None):
        if(dx != None):
            mingc.SWFShape_drawCubic(self.this, bx, by, cx, cy, dx, dy)
        else:
            mingc.SWFShape_drawCurve(self.this, bx, by, cx, cy)

    def drawCubicTo(self, bx, by, cx, cy, dx, dy):
        mingc.SWFShape_drawCubicTo(self.this, bx, by, cx, cy, dx, dy)

    def drawCubic(self, bx, by, cx, cy, dx, dy):
        mingc.SWFShape_drawCubic(self.this, bx, by, cx, cy, dx, dy)

    def drawGlyph(self, font, char, size=0):
        mingc.SWFShape_drawSizedGlyph(self.this, font.this, ord(char[0]), size)

    def drawCharacterBounds(self, char):
        mingc.SWFShape_drawCharacterBounds(self.this, char.this)

    def end(self):
        mingc.SWFShape_end(self.this)

    def useVersion(self, version):
        mingc.SWFShape_useVersion(self.this, version)

    def getVersion(self):
        return mingc.SWFShape_getVersion(self.this)

    def addSolidFill(self, r, g, b, a=0xff):
        return SWFFill(mingc.SWFShape_addSolidFill(self.this, r, g, b, a))

    def addGradientFill(self, gradient, flags):
        return SWFFill(mingc.SWFShape_addGradientFill(self.this, gradient.this, flags))

    def addBitmapFill(self, bitmap, flags):
        return SWFFill(mingc.SWFShape_addBitmapFill(self.this, bitmap.this, flags))

    def setRenderHintingFlags(self, flags):
        mingc.SWFShape_setRenderHintingFlags(self.this, flags)

    def getPenX(self):
        mingc.SWFShape_getPenX(self.this)

    def getPenY(self):
        mingc.SWFShape_getPenY(self.this)

    def hideLine(self):
        mingc.SWFShape_hideLine(self.this)
 
# addFill flags:
SWFFILL_SOLID		= mingc.SWFFILL_SOLID		
SWFFILL_GRADIENT	= mingc.SWFFILL_GRADIENT	
SWFFILL_LINEAR_GRADIENT = mingc.SWFFILL_LINEAR_GRADIENT 
SWFFILL_RADIAL_GRADIENT = mingc.SWFFILL_RADIAL_GRADIENT 
SWFFILL_BITMAP		= mingc.SWFFILL_BITMAP		
SWFFILL_TILED_BITMAP	= mingc.SWFFILL_TILED_BITMAP	
SWFFILL_CLIPPED_BITMAP	= mingc.SWFFILL_CLIPPED_BITMAP
#hinting flags
SWF_SHAPE_USESCALINGSTROKES    = mingc.SWF_SHAPE_USESCALINGSTROKES
SWF_SHAPE_USENONSCALINGSTROKES = mingc.SWF_SHAPE_USENONSCALINGSTROKES

class SWFFill(SWFBase):

    def __del__(self):
        mingc.destroySWFFill(self.this)

    def rotate(self, degrees):
        mingc.SWFFill_rotate(self.this, degrees)

    def rotateTo(self, degrees):
        mingc.SWFFill_rotateTo(self.this, degrees)

    def move(self, x, y):
        mingc.SWFFill_move(self.this, x, y)

    def moveTo(self, x, y):
        mingc.SWFFill_moveTo(self.this, x, y)

    def scale(self, xScale, yScale=None):
        if yScale is None:
            mingc.SWFFill_scaleXY(self.this, xScale, xScale)
        else:
            mingc.SWFFill_scaleXY(self.this, xScale, yScale)

    def scaleTo(self, xScale, yScale=None):
        if yScale is None:
            mingc.SWFFill_scaleXYTo(self.this, xScale, xScale)
        else:
            mingc.SWFFill_scaleXYTo(self.this, xScale, yScale)

    def scaleX(self, xScale):
        mingc.SWFFill_scaleX(self.this, xScale)

    def scaleXTo(self, xScale):
        mingc.SWFFill_scaleXTo(self.this, xScale)

    def scaleY(self, yScale):
        mingc.SWFFill_scaleY(self.this, yScale)

    def scaleYTo(self, yScale):
        mingc.SWFFill_scaleYTo(self.this, yScale)

    def skewX(self, x):
        mingc.SWFFill_skewX(self.this, x)

    def skewXTo(self, x):
        mingc.SWFFill_skewXTo(self.this, x)

    def skewY(self, y):
        mingc.SWFFill_skewY(self.this, y)

    def skewYTo(self, y):
        mingc.SWFFill_skewYTo(self.this, y)

    def setMatrix(self, a, b, c, d, x, y):
        mingc.SWFFill_setMatrix(self.this, a, b, c, d, x, y)

class SWFCharacter(SWFBase):

    def getWidth(self):
        mingc.SWFCharacter_getWidth(self.this)

    def getHeight(self):
        mingc.SWFCharacter_getHeight(self.this)

class SWFDisplayItem(SWFBase):

    def rotate(self, degrees):
        mingc.SWFDisplayItem_rotate(self.this, degrees)

    def rotateTo(self, degrees):
        mingc.SWFDisplayItem_rotateTo(self.this, degrees)

    def move(self, x, y):
        mingc.SWFDisplayItem_move(self.this, x, y)

    def moveTo(self, x, y):
        mingc.SWFDisplayItem_moveTo(self.this, x, y)

    def scale(self, xScale, yScale):
        mingc.SWFDisplayItem_scale(self.this, xScale, yScale)

    def scaleTo(self, xScale, yScale):
        mingc.SWFDisplayItem_scaleTo(self.this, xScale, yScale)

    def skewX(self, x):
        mingc.SWFDisplayItem_skewX(self.this, x)

    def skewXTo(self, x):
        mingc.SWFDisplayItem_skewXTo(self.this, x);

    def skewY(self, y):
        mingc.SWFDisplayItem_skewY(self.this, y)

    def skewYTo(self, y):
        mingc.SWFDisplayItem_skewYTo(self.this, y)

    def setName(self, name):
        mingc.SWFDisplayItem_setName(self.this, name)

    def setRatio(self, ratio):
        mingc.SWFDisplayItem_setRatio(self.this, ratio)

    def getDepth(self):
        return mingc.SWFDisplayItem_getDepth(self.this)

    def setDepth(self, depth):
        mingc.SWFDisplayItem_setDepth(self.this, depth)

    def addColor(self, r, g, b, a=0):
        mingc.SWFDisplayItem_setColorAdd(self.this, r, g, b, a)

    def multColor(self, r, g, b, a=1.0):
        mingc.SWFDisplayItem_setColorMult(self.this, r, g, b, a)

    def remove(self):
        mingc.SWFDisplayItem_remove(self.this)

    def setMatrix(self, a, b, c, d, x, y):
        mingc.SWFDisplayItem_setMatrix(self.this, a, b, c, d, x, y)

    def cacheAsBitmap(self, flag):
        mingc.SWFDisplayItem_cacheAsBitmap(self.this, flag)

    def setBlendMode(self, mode):
        mingc.SWFDisplayItem_setBlendMode(self.this, flag)

    def addAction(self, actionScript, flags):
        mingc.SWFDisplayItem_addAction(self.this, actionScript, flags)

    def setMaskLevel(self, level):
        mingc.SWFDisplayItem_setMaskLevel(self.this, level)

    def getPositionX(self):
	x = mingc.new_floatp()
	y = mingc.new_floatp()
	mingc.SWFDisplayItem_getPosition(self.this, x, y)
	ret = mingc.floatp_value(x)
	mingc.delete_floatp(x)
	mingc.delete_floatp(y)
	return ret

    def getPositionY(self):
	x = mingc.new_floatp()
	y = mingc.new_floatp()
	mingc.SWFDisplayItem_getPosition(self.this, x, y)
	ret = mingc.floatp_value(y)
	mingc.delete_floatp(x)
	mingc.delete_floatp(y)
	return ret

    def getRotation(self):
        rot = mingc.new_floatp()
	mingc.SWFDisplayItem_getRotation(self.this, rot)
	ret = mingc.floatp_value(rot)
	mingc.delete_floatp(rot)
	return ret

    def getScaleX(self):
        sx = mingc.new_floatp()
        sy = mingc.new_floatp()
        mingc.SWFDisplayItem_getScale(self.this, sx, sy)
        ret = mingc.floatp_value(sx)
        mingc.delete_floatp(sx)
        mingc.delete_floatp(sy)
        return ret

    def getScaleY(self):
        sx = mingc.new_floatp()
        sy = mingc.new_floatp()
        mingc.SWFDisplayItem_getScale(self.this, sx, sy)
        ret = mingc.floatp_value(sy)
        mingc.delete_floatp(sx)
        mingc.delete_floatp(sy)
        return ret

    def getSkewX(self):
        sx = mingc.new_floatp()
        sy = mingc.new_floatp()
        mingc.SWFDisplayItem_getSkew(self.this, sx, sy)
        ret = mingc.floatp_value(sx)
        mingc.delete_floatp(sx)
        mingc.delete_floatp(sy)
        return ret

    def getSkewY(self):
        sx = mingc.new_floatp()
        sy = mingc.new_floatp()
        mingc.SWFDisplayItem_getSkew(self.this, sx, sy)
        ret = mingc.floatp_value(sy)
        mingc.delete_floatp(sx)
        mingc.delete_floatp(sy)
        return ret

    def flush(self):
        mingc.SWFDisplayItem_flush(self.this)

    def endMask(self):
        mingc.SWFDisplayItem_endMask(self.this)

    def getMatrix(self):
        return SWFMatrix(mingc.SWFDisplayItem_getMatrix(self.this))

    def setCXform(self, cx):
	mingc.SWFDisplayItem_setCXform(self.this, cx.this);
         
    def addFilter(self, filter):
        mingc.SWFDisplayItem_addFilter(self.this, filter.this)

    def getCharacter(self):
        return SWFCharacter(mingc.SWFDisplayItem_getCharacter(self.this))

SWFACTION_ONLOAD      = mingc.SWFACTION_ONLOAD      
SWFACTION_ENTERFRAME  = mingc.SWFACTION_ENTERFRAME  
SWFACTION_UNLOAD      = mingc.SWFACTION_UNLOAD      
SWFACTION_MOUSEMOVE   = mingc.SWFACTION_MOUSEMOVE   
SWFACTION_MOUSEDOWN   = mingc.SWFACTION_MOUSEDOWN   
SWFACTION_MOUSEUP     = mingc.SWFACTION_MOUSEUP     
SWFACTION_KEYDOWN     = mingc.SWFACTION_KEYDOWN     
SWFACTION_KEYUP       = mingc.SWFACTION_KEYUP       
SWFACTION_DATA        = mingc.SWFACTION_DATA       

SWFBLEND_MODE_NULL	= mingc.SWFBLEND_MODE_NULL
SWFBLEND_MODE_NORMAL	= mingc.SWFBLEND_MODE_NORMAL
SWFBLEND_MODE_LAYER 	= mingc.SWFBLEND_MODE_LAYER
SWFBLEND_MODE_MULT	= mingc.SWFBLEND_MODE_MULT
SWFBLEND_MODE_SCREEN	= mingc.SWFBLEND_MODE_SCREEN
SWFBLEND_MODE_LIGHTEN	= mingc.SWFBLEND_MODE_LIGHTEN
SWFBLEND_MODE_DARKEN	= mingc.SWFBLEND_MODE_DARKEN
SWFBLEND_MODE_DIFF	= mingc.SWFBLEND_MODE_DIFF
SWFBLEND_MODE_ADD	= mingc.SWFBLEND_MODE_ADD
SWFBLEND_MODE_SUB	= mingc.SWFBLEND_MODE_SUB
SWFBLEND_MODE_INV	= mingc.SWFBLEND_MODE_INV
SWFBLEND_MODE_ALPHA	= mingc.SWFBLEND_MODE_ALPHA
SWFBLEND_MODE_ERASE	= mingc.SWFBLEND_MODE_ERASE
SWFBLEND_MODE_OVERLAY	= mingc.SWFBLEND_MODE_OVERLAY
SWFBLEND_MODE_HARDLIGHT = mingc.SWFBLEND_MODE_HARDLIGHT


class SWFMovie(SWFBase):

    def __init__(self):
        self.this = mingc.newSWFMovie()
        self.blocks = []

    def __del__(self):
        mingc.destroySWFMovie(self.this)

    def setRate(self, rate):
        mingc.SWFMovie_setRate(self.this, rate)

    def getRate(self):
        return mingc.SWFMovie_getRate(self.this)

    def setDimension(self, x, y):
        mingc.SWFMovie_setDimension(self.this, x, y)

    def setBackground(self, r, g, b):
        mingc.SWFMovie_setBackground(self.this, r, g, b)

    def setFrames(self, totalFrames):
        mingc.SWFMovie_setNumberOfFrames(self.this, totalFrames)

    # or:
    def setNumberOfFrames(self, totalFrames):
        mingc.SWFMovie_setNumberOfFrames(self.this, totalFrames)

    def protect(self, passwd=None):
        if passwd is None:
            mingc.SWFMovie_protect(self.this, None)
        else:
            mingc.SWFMovie_protect(self.this, passwd)


    def nextFrame(self):
        mingc.SWFMovie_nextFrame(self.this)

    def add(self, block):
        self.blocks.append(block)
        return SWFDisplayItem(mingc.SWFMovie_add(self.this, block.this))

    def remove(self, item):
        mingc.SWFMovie_remove(self.this, item.this)

    def replace(self, item, block):
        self.blocks.append(block)
        mingc.SWFMovie_replace(self.this, item.this, block.this)

    def streamMp3(self, mp3, skip=0.0):
        sound = SWFSoundStream(mp3)
        self.blocks.append(sound)
        mingc.SWFMovie_setSoundStreamAt(self.this, sound, skip)

    def setSoundStream(self, sound, skip=0.0):
        self.blocks.append(sound)
        mingc.SWFMovie_setSoundStreamAt(self.this, sound.this, skip);

    def output(self):
        return mingc.SWFMovie_simpleOutput(self.this)

    def save(self, filename):
        mingc.SWFMovie_save(self.this, filename)

    def saveToFile(self, file):
        mingc.SWFMovie_saveToFileNo(self.this, file.fileno())

    def labelFrame(self, label):
        mingc.SWFMovie_labelFrame(self.this, label)

    def namedAnchor(self, label):
        mingc.SWFMovie_namedAnchor(self.this, label)

    # deprecated:
    def simpleOutput(self):
        return mingc.SWFMovie_simpleOutput(self.this)

    def addExport(self, clipObj, libName):
        return mingc.SWFMovie_addExport(self.this, clipObj, libName)

    def writeExports(self):
        mingc.SWFMovie_writeExports(self.this)

    def assignSymbol(self, character, name):
	mingc.SWFMovie_assignSymbol(self.this, character.this, name)
	
    def setNetworkAccess(self, flag):
        mingc.SWFMovie_setNetworkAccess(self.this, flag)

    def addMetadata(self, xmldata):
        mingc.SWFMovie_addMetadata(self.this, xmldata)

    def setScriptLimits(self, maxRecursion, timeout):
        mingc.SWFMovie_setScriptLimits(self.this, maxRecursion, timeout)

    def setTabIndex(self, depth, index):
        mingc.SWFMovie_setTabIndex(self.this, depth, index)

    def defineScene(self, offset, name):
        mingc.SWFMovie_defineScene(self.this, offset, index)

    def addFont(self, font):
        return SWFFontCharacter(mingc.SWFMovie_addFont(self.this, font.this))

    def importFont(self, filename, name):
        return SWFFontCharacter(mingc.SWFMovie_importFont(self.this, filename, name))

    def startSound(self, sound):
        return SWFSoundInstance(mingc.SWFMovie_startSound(self.this, sound.this))

    def stopSound(self, sound):
        mingc.SWFMovie_stopSound(self.this, sound.this)

    def importCharacter(self):
        SWFCharacter(mingc.SWFMovie_importCharacter(self.this))


class SWFSprite(SWFBase):

    def __init__(self):
        self.this = mingc.newSWFMovieClip()
        self.blocks = []

    def __del__(self):
        mingc.destroySWFMovieClip(self.this)

    def setNumberOfFrames(self, frames):
        mingc.SWFMovieClip_setNumberOfFrames(self.this, frames)

    def add(self, block):
        self.blocks.append(block)
        return SWFDisplayItem(mingc.SWFMovieClip_add(self.this, block.this))

    def remove(self, item):
        mingc.SWFMovieClip_remove(self.this, item.this)

    def nextFrame(self):
        mingc.SWFMovieClip_nextFrame(self.this)

    def labelFrame(self, label):
        mingc.SWFMovieClip_labelFrame(self.this, label)

    def setScalingGrid(self, x, y, w, h):
        mingc.SWFMovieClip_setScalingGrid(self.this, x, y, w, h)

    def removeScalingGrid(self):
        mingc.SWFMovieClip_removeScalingGrid(self.this)

    def addInitAction(self, action):
        self.blocks.append(block)
        mingc.SWFMovieClip_addInitAction(self.this, action.this)

    def startSound(self, sound):
        return SWFSoundInstance(mingc.SWFMovieClip_startSound(self.this, sound.this))

    def stopSound(self, sound):
        mingc.SWFMovieClip_stopSound(self.this, sound.this)

    def setSoundStream(self, sound, rate, skip=0.0):
        self.blocks.append(sound)
        mingc.SWFMovieClip_setSoundStreamAt(self.this, sound.this, rate, skip);

# deprecated:
class SWFMovieClip(SWFSprite):
    pass


class SWFMorph(SWFBase):

    def __init__(self):
        self.this = mingc.newSWFMorphShape()

    def __del__(self):
        mingc.destroySWFMorph(self.this)

    def getShape1(self):
        # have to keep a reference so that it doesn't scope out
        self.shape1 = SWFShape(mingc.SWFMorph_getShape1(self.this))
        return self.shape1

    def getShape2(self):
        self.shape2 = SWFShape(mingc.SWFMorph_getShape2(self.this))
        return self.shape2

# deprecated:
class SWFMorphShape(SWFMorph):
    pass


class SWFBrowserFont(SWFBase):

    def __init__(self, name):
       self.this = mingc.newSWFBrowserFont(name)
       self.name = name

    def __del__(self):
       	mingc.destroySWFBrowserFont(self.this)		


class SWFFont(SWFBase):

    def __init__(self, name):
            self.this = mingc.newSWFFont_fromFile(name)
            self.name = name
        
    def __del__(self):
            mingc.destroySWFFont(self.this)

    def getAscent(self):
        return mingc.SWFFont_getAscent(self.this)

    def getDescent(self):
        return mingc.SWFFont_getDescent(self.this)

    def getLeading(self):
        return mingc.SWFFont_getLeading(self.this)

    def getUTF8StringWidth(self):
        return mingc.SWFFont_getUTF8StringWidth(self.this, string.encode("utf-8"))

    def getStringWidth(self, string):
        return mingc.SWFFont_getStringWidth(self.this, string)

    # or: (?)
    def getWidth(self, string):
        return mingc.SWFFont_getStringWidth(self.this, string)

    def getName(self):
        return mingc.SWFFont_getName(self.this)

    def getGlyphCount(self):
        return mingc.SWFFont_getGlyphCount(self.this)

class SWFFontCharacter(SWFBase):
     
     def addChars(self, string):
         mingc.SWFFontCharacter_addChars(self.this, string)

     def addUTF8Chars(self, string):
         mingc.SWFFontCharacter_addUTF8Chars(self.this, string)

     def addAllChars(self, string):
         mingc.SWFFontCharacter_addAllChars(self.this)

class SWFBitmap(SWFBase):

    def __init__(self, fname, alpha=None):
        ext = fname[-4:]

        if alpha is None:
            self.file = open(fname, "rb")
            self.this = mingc.newSWFBitmap_fromInput(mingc.newSWFInput_file(self.file))

        elif ext == '.jpg' or ext == '.JPG':
            self.file = open(fname, "rb")
            self.this = mingc.newSWFBitmap_fromInput(mingc.newSWFInput_file(self.file))

    def __del__(self):
        mingc.destroySWFBitmap(self.this)

    def getWidth(self):
        return mingc.SWFBitmap_getWidth(self.this)

    def getHeight(self):
        return mingc.SWFBitmap_getHeight(self.this)


class SWFFontCollection(SWFBase):

    def __init__(self, filename):
        self.this = mingc.newSWFFontCollection_fromFile(filename)

    def __del__(self):
        mingc.destroySWFFontCollection(self.this)

    def getFontCount(self):
        return mingc.SWFFontCollection_getFontCount(self.this)

    def getFont(self, index):
        return SWFFont(mingc.SWFFontCollection_getFont(self.this, index)) 


# deprecated:
class SWFDBLBitmap(SWFBitmap):

    def __init__(self, fname):
        self.this = mingc.newSWFDBLBitmap(open(fname, "rb"))


# deprecated:
class JpegBitmap(SWFBitmap):

    def __init__(self, fname, alpha=None):
        if alpha is None:
            self.this = mingc.newSWFJpegBitmap(open(fname, "rb"))
        else:
            self.this = mingc.newSWFJpegWithAlpha(open(fname, "rb"), open(alpha, "rb"))


class SWFText(SWFBase):

    def __init__(self, version=2):
        self.__fonts={} # keep reference
        if version == 2:
            self.this = mingc.newSWFText2()
        else:
            self.this = mingc.newSWFText()

    def __del__(self):
        mingc.destroySWFText(self.this)

    def setFont(self, font):
        self.__fonts[font.name] = font
        mingc.SWFText_setFont(self.this, font.this)

    def setHeight(self, height):
        mingc.SWFText_setHeight(self.this, height)

    def moveTo(self, x, y):
        mingc.SWFText_moveTo(self.this, x, y)

    def setColor(self, r, g, b, a=0xff):
        mingc.SWFText_setColor(self.this, r, g, b, a)

    def addString(self, s, advance=None):
        mingc.SWFText_addString(self.this, s, advance)

    def addUTF8String(self, s, advance=None):
        mingc.SWFText_addUTF8String(self.this, s.encode("utf-8"), advance)

    def setSpacing(self, spacing):
        mingc.SWFText_setSpacing(self.this, spacing)

    def getAscent(self):
        return mingc.SWFText_getAscent(self.this)

    def getDescent(self):
        return mingc.SWFText_getDescent(self.this)

    def getLeading(self):
        return mingc.SWFText_getLeading(self.this)

    def getWidth(self, string):
        return mingc.SWFText_getStringWidth(self.this, string)

    def getUTF8Width(self, s):
        return mingc.SWFText_getUTF8StringWidth(self.this, s.encode("utf-8")) 

    # deprecated:
    def setXY(self, x, y):
        mingc.SWFText_setXY(self.this, x, y)


class SWFText2(SWFText):

    def __init__(self):
        self.this = mingc.newSWFText2()


class SWFTextField(SWFBase):

    def __init__(self, flags=None):
        self.__fonts={} # keep reference
        self.this = mingc.newSWFTextField()

        if flags is not None:
            mingc.SWFTextField_setFlags(self.this, flags)

    def __del__(self):
        mingc.destroySWFTextField(self.this)

    def setFont(self, font):
        self.__fonts[font.name] = font
        mingc.SWFTextField_setFont(self.this, font.this)

    def setBounds(self, width, height):
        mingc.SWFTextField_setBounds(self.this, width, height)

    def setFlags(self, flags):
        mingc.SWFTextField_setFlags(self.this, flags)

    def setColor(self, r, g, b, a=0xff):
        mingc.SWFTextField_setColor(self.this, r, g, b, a)

    def setVariableName(self, name):
        mingc.SWFTextField_setVariableName(self.this, name)

    def addString(self, string):
        mingc.SWFTextField_addString(self.this, string)

    def addUTF8String(self, string):
        mingc.SWFTextField_addUTF8String(self.this, string.encode("utf-8"))

    def setHeight(self,  height):
        mingc.SWFTextField_setHeight(self.this,  height)

    def setLeftMargin(self, leftMargin):
        mingc.SWFTextField_setLeftMargin(self.this, leftMargin)

    def setRightMargin(self, rightMargin):
        mingc.SWFTextField_setRightMargin(self.this, rightMargin)

    def setIndentation(self, indentation):
        mingc.SWFTextField_setIndentation(self.this, indentation)

    def setLineSpacing(self, lineSpacing):
        mingc.SWFTextField_setLineSpacing(self.this, lineSpacing)

    def setAlignment(self, alignment):
        mingc.SWFTextField_setAlignment(self.this,  alignment)

    # or just:
    def align(self, alignment):
        mingc.SWFTextField_setAlignment(self.this,  alignment)

    def setLength(self, length):
        mingc.SWFTextField_setLength(self.this, length)

    def addChars(self, string):
        mingc.SWFTextField_addChars(self.this, string)

    def setPadding(self, pad):
        mingc.SWFTextField_setPadding(self.this, pad)

    def setFieldHeight(self, height):
        mingc.SWFTextField_setFieldHeight(self.this, height)

# textfield alignment flags:
SWFTEXTFIELD_ALIGN_LEFT    = mingc.SWFTEXTFIELD_ALIGN_LEFT
SWFTEXTFIELD_ALIGN_RIGHT   = mingc.SWFTEXTFIELD_ALIGN_RIGHT
SWFTEXTFIELD_ALIGN_CENTER  = mingc.SWFTEXTFIELD_ALIGN_CENTER
SWFTEXTFIELD_ALIGN_JUSTIFY = mingc.SWFTEXTFIELD_ALIGN_JUSTIFY

# other flags:

SWFTEXTFIELD_ONMASK    = mingc.SWFTEXTFIELD_ONMASK    
SWFTEXTFIELD_OFFMASK   = mingc.SWFTEXTFIELD_OFFMASK   
SWFTEXTFIELD_HASFONT   = mingc.SWFTEXTFIELD_HASFONT   
SWFTEXTFIELD_HASLENGTH = mingc.SWFTEXTFIELD_HASLENGTH 
SWFTEXTFIELD_HASCOLOR  = mingc.SWFTEXTFIELD_HASCOLOR  
SWFTEXTFIELD_NOEDIT    = mingc.SWFTEXTFIELD_NOEDIT    
SWFTEXTFIELD_PASSWORD  = mingc.SWFTEXTFIELD_PASSWORD  
SWFTEXTFIELD_MULTILINE = mingc.SWFTEXTFIELD_MULTILINE 
SWFTEXTFIELD_WORDWRAP  = mingc.SWFTEXTFIELD_WORDWRAP  
SWFTEXTFIELD_HASTEXT   = mingc.SWFTEXTFIELD_HASTEXT   
SWFTEXTFIELD_USEFONT   = mingc.SWFTEXTFIELD_USEFONT   
SWFTEXTFIELD_HTML      = mingc.SWFTEXTFIELD_HTML      
SWFTEXTFIELD_DRAWBOX   = mingc.SWFTEXTFIELD_DRAWBOX   
SWFTEXTFIELD_NOSELECT  = mingc.SWFTEXTFIELD_NOSELECT  
SWFTEXTFIELD_HASLAYOUT = mingc.SWFTEXTFIELD_HASLAYOUT 
SWFTEXTFIELD_AUTOSIZE  = mingc.SWFTEXTFIELD_AUTOSIZE

class SWFSoundStream(SWFBase):
	
    def __init__(self, fname):
        self.file = open(fname, "rb")
        self.this = mingc.newSWFSoundStream(self.file)

    def getDuration(self):
        return mingc.SWFSoundStream_getDuration(self.this)

    def setInitialMp3Delay(self, delay):
        mingc.SWFSoundStream_setInitialMp3Delay(self.this, delay)

class SWFSound(SWFBase):

    def __init__(self, arg, flags=0):
        self.arg = arg;
        if isinstance(arg, SWFSoundStream):
            self.this = mingc.newSWFSound_fromSoundStream(arg)
        else:
            self.file = open(arg, "rb")
            self.this = mingc.newSWFSound(self.file, flags)

    def setInitialMp3Delay(self, delay):
        mingc.SWFSound_setInitialMp3Delay(self.this, delay)

    # display list destroys this..

SWF_SOUND_COMPRESSION       = mingc.SWF_SOUND_COMPRESSION       
SWF_SOUND_NOT_COMPRESSED    = mingc.SWF_SOUND_NOT_COMPRESSED    
SWF_SOUND_ADPCM_COMPRESSED  = mingc.SWF_SOUND_ADPCM_COMPRESSED  
SWF_SOUND_MP3_COMPRESSED    = mingc.SWF_SOUND_MP3_COMPRESSED    
SWF_SOUND_NOT_COMPRESSED_LE = mingc.SWF_SOUND_NOT_COMPRESSED_LE 
SWF_SOUND_NELLY_COMPRESSED  = mingc.SWF_SOUND_NELLY_COMPRESSED  
SWF_SOUND_RATE              = mingc.SWF_SOUND_RATE              
SWF_SOUND_5KHZ              = mingc.SWF_SOUND_5KHZ              
SWF_SOUND_11KHZ             = mingc.SWF_SOUND_11KHZ             
SWF_SOUND_22KHZ             = mingc.SWF_SOUND_22KHZ             
SWF_SOUND_44KHZ             = mingc.SWF_SOUND_44KHZ             
SWF_SOUND_BITS              = mingc.SWF_SOUND_BITS              
SWF_SOUND_8BITS             = mingc.SWF_SOUND_8BITS             
SWF_SOUND_16BITS            = mingc.SWF_SOUND_16BITS            
SWF_SOUND_CHANNELS          = mingc.SWF_SOUND_CHANNELS          
SWF_SOUND_MONO              = mingc.SWF_SOUND_MONO              
SWF_SOUND_STEREO            = mingc.SWF_SOUND_STEREO            

class SWFSoundInstance(SWFBase):

    def setNoMultiple(self):
        mingc.SWFSoundInstance_setNoMultiple(self.this)

    def setLoopInPoint(self, point):
        mingc.SWFSoundInstance_setLoopInPoint(self.this, point)

    def setLoopOutPoint(self, point):
        mingc.SWFSoundInstance_setLoopOutPoint(self.this, point)

    def setLoopCount(self, count):
        mingc.SWFSoundInstance_setLoopCount(self.this, count)

    def addEnvelope(self, mark44, left, right):
        mingc.SWFSoundInstance_addEnvelope(self.this, mark44, left, right)

class SWFAction(SWFBase):

    def __init__(self, script):
        self.this = mingc.newSWFAction(script)

    def compile(swfVersion):
        return mingc.SWFAction_compile(self.this, swfVersion, NULL)

class SWFInitAction(SWFBase):

   def __init__(self, action):
       self.this = mingc.newSWFInitAction(action.this)

    # assigned object will destroy this..

SWF_GRADIENT_PAD	= mingc.SWF_GRADIENT_PAD
SWF_GRADIENT_REFLECT	= mingc.SWF_GRADIENT_REFLECT
SWF_GRADIENT_REPEAT	= mingc.SWF_GRADIENT_REPEAT
SWF_GRADIENT_NORMAL	= mingc.SWF_GRADIENT_NORMAL
SWF_GRADIENT_LINEAR	= mingc.SWF_GRADIENT_LINEAR

class SWFGradient(SWFBase):

    def __init__(self):
        self.this = mingc.newSWFGradient()

    def __del__(self):
        mingc.destroySWFGradient(self.this)

    def addEntry(self, ratio, r, g, b, a=0xff):
        mingc.SWFGradient_addEntry(self.this, ratio, r, g, b, a)

    def setSpreadMode(self, mode):
        mingc.SWFGradient_setSpreadMode(self.this, mode)

    def setInterpolationMode(self, mode):
        mingc.SWFGradient_setInterpolationMode(self.this, mode)

    def SWFGradient_setFocalPoint(self, fp):
        mingc.SWFGradient_setFocalPoint(self.this, fp)

class SWFButton(SWFBase):

    def __init__(self):
        self.this = mingc.newSWFButton()
        self.shapes = []

    def __del__(self):
        mingc.destroySWFButton(self.this)

    def setUp(self, character):
        self.addShape(character, SWFBUTTON_UP)

    def setDown(self, character):
        self.addShape(character, SWFBUTTON_DOWN)

    def setOver(self, character):
        self.addShape(character, SWFBUTTON_OVER)

    def setHit(self, character):
        self.addShape(character, SWFBUTTON_HIT)

    def addShape(self, character, flags):
        self.shapes.append(character)
        mingc.SWFButton_addCharacter(self.this, character, flags)

    def addAction(self, action, flags):
        mingc.SWFButton_addAction(self.this, action.this, flags)

    def addCharacter(self, character, flags):
        self.shapes.append(character)
        return SWFButtonRecord(mingc.SWFButton_addCharacter(self.this, character, flags))

    def addSound(self, sound, flags):
        return SWFSoundInstance(mingc.SWFButton_addSound(self.this, sound.this, flags))

    def setMenu(self, flag):
        mingc.SWFButton_setMenu(self.this, flag)

    def setScalingGrid(self, x, y, w, h):
        mingc.SWFButton_setScalingGrid(self.this, x, y, w, h)

    def removeScalingGrid(self):
        mingc.SWFButton_removeScalingGrid(self.this)

class SWFButtonRecord(SWFBase):

    def setDepth(self, depth):
        mingc.SWFButtonRecord_setDepth(self.this, depth)

    def setBlendMode(self, mode):
        mingc.SWFButtonRecord_setBlendMode(self.this, mode)

    def move(self, x, y):
        mingc.SWFButtonRecord_move(self.this, x, y)

    def moveTo(self, x, y):
        mingc.SWFButtonRecord_moveTo(self.this, x, y)

    def rotate(self, deg):
        mingc.SWFButtonRecord_rotate(self.this, deg)

    def rotateTo(self, deg):
        mingc.SWFButtonRecord_rotateTo(self.this, deg)

    def scale(self, sx, sy):
        mingc.SWFButtonRecord_scale(self.this, sx, sy)

    def scaleTo(self, sx, sy):
        mingc.SWFButtonRecord_scaleTo(self.this, sx, sy)

    def skewX(self, sx):
        mingc.SWFButtonRecord_skewX(self.this, sx)

    def skewXTo(self, sx):
        mingc.SWFButtonRecord_skewXTo(self.this, sx)

    def skewY(self, sy):
        mingc.SWFButtonRecord_skewY(self.this, sy)

    def skewYTo(self, sy):
        mingc.SWFButtonRecord_skewXTo(self.this, sy)

    def addFilter(self, filter):
        mingc.SWFButtonRecord_addFilter(self.this, filter.this)

SWFVIDEOSTREAM_MODE_AUTO = mingc.SWFVIDEOSTREAM_MODE_AUTO
SWFVIDEOSTREAM_MODE_MANUAL = mingc.SWFVIDEOSTREAM_MODE_MANUAL

class SWFVideoStream(SWFBase):
    def __init__(self, filename=None):
	if filename is None:
		self.file = 0;
        	self.this = mingc.newSWFVideoStream()
	else:
		self.file = open(filename, "rb");
        	self.this = mingc.newSWFVideoStream_fromFile(self.file)
    def setDimension(self, w, h):
        return mingc.SWFVideoStream_setDimension(self.this, w, h)
    def getNumFrames(self):
        return mingc.SWFVideoStream_getNumFrames(self.this)
    def hasAudio(self):
        return mingc.SWFVideoStream_hasAudio(self.this)
    def setFrameMode(self, mode):
        return mingc.SWFVideoStream_setFrameMode(self.this, mode)
    def nextFrame(self):
        return mingc.SWFVideoStream_nextFrame(self.this)
    def seek(self, frame, whence):
        return mingc.SWFVideoStream_seek(self.this, frame, whence)
    def __del__(self):
        mingc.destroySWFVideoStream(self.this)

def SWFBUTTON_KEYPRESS(c):
    return mingc.swfButton_keypress(c)

# addShape flags:
SWFBUTTON_UP             = mingc.SWFBUTTON_UP
SWFBUTTON_HIT            = mingc.SWFBUTTON_HIT
SWFBUTTON_DOWN           = mingc.SWFBUTTON_DOWN
SWFBUTTON_OVER           = mingc.SWFBUTTON_OVER

# addAction flags:
SWFBUTTON_MOUSEUPOUTSIDE = mingc.SWFBUTTON_MOUSEUPOUTSIDE
SWFBUTTON_DRAGOVER       = mingc.SWFBUTTON_DRAGOVER
SWFBUTTON_DRAGOUT        = mingc.SWFBUTTON_DRAGOUT
SWFBUTTON_MOUSEUP        = mingc.SWFBUTTON_MOUSEUP
SWFBUTTON_MOUSEDOWN      = mingc.SWFBUTTON_MOUSEDOWN
SWFBUTTON_MOUSEOUT       = mingc.SWFBUTTON_MOUSEOUT
SWFBUTTON_MOUSEOVER      = mingc.SWFBUTTON_MOUSEOVER

class SWFPrebuiltClip(SWFBase):
    def __init__(self, filename):
        self.this = mingc.newSWFPrebuiltClip_fromFile(filename)
    def __del__(self):
        mingc.destroySWFPrebuiltClip(self.this)

class SWFBinaryData(SWFBase):
    def __init__(self, string, len):
        self.this = mingc.newSWFBinaryData(string, len)
    def __del__(self):
        mingc.destroySWFBinaryData(self.this)
