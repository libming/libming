import mingc

# should raise an exception if return is non-zero:
mingc.Ming_init()

def Ming_setCubicThreshold(t):
    mingc.Ming_setCubicThreshold(t);

def Ming_setScale(scale):
    mingc.Ming_setScale(scale);


class SWFBase:

    def __init__(self, o):
        self.this = o

    # reportedly useful for zope/zwiff:
    def set(self, name, val):
        setattr(self, name, val)


class SWFRect(SWFBase):

    def __init__(self, minX, maxX, minY, maxY):
        self.this = mingc.newSWFRect(minX, maxX, minY, maxY)

    def __del__(self):
        mingc.destroySWFRect(self.this)

    def getWidth(self):
        return mingc.SWFRect_getWidth(self.this)

    def getHeight(self):
        return mingc.SWFRect_getHeight(self.this)


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
        return mingc.SWFShape_setLine(self.this, width, r, g, b, a)

    # I know there's probably a better way to do this..
    def addFill(self, arg1, arg2=0, arg3=None, arg4=0xff):
        if arg3 != None:
            return SWFFill(mingc.SWFShape_addSolidFill(self.this, arg1, arg2, arg3, arg4))
        if arg1.this[-11:] == 'SWFGradient':
            # XXX - have to keep reference to gradient so it's not disposed
            self.fills.append(arg1)
            return SWFFill(mingc.SWFShape_addGradientFill(self.this, arg1.this, arg2))

        if arg1.this[-9:] == 'SWFBitmap':
            # XXX - have to keep reference to bitmap so it's not disposed
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

    def drawRect(self, rect):
        mingc.SWFShape_drawRect(self.this, rect.this);

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

    def drawGlyph(self, font, char):
        mingc.SWFShape_drawGlyph(self.this, font.this, ord(char[0]))

    def drawCharacterBounds(self, char):
        mingc.SWFShape_drawCharacterBounds(self.this, char.this)

    def end(self):
        mingc.SWFShape_end(self.this)

    # deprecated:
    def moveTo(self, x, y):
        mingc.SWFShape_moveTo(self.this, x, y)

    def lineTo(self, x, y):
        mingc.SWFShape_lineTo(self.this, x, y)

    def lineToRelative(self, dx, dy):
        mingc.SWFShape_lineToRelative(self.this, dx, dy)

    def curveTo(self, controlx, controly, anchorx, anchory):
        mingc.SWFShape_curveTo(self.this, controlx, controly, anchorx, anchory)

    def curveToRelative(self, controldx, controldy, anchordx, anchordy):
        mingc.SWFShape_curveToRelative(self.this, controldx, controldy,
                                       anchordx, anchordy)

    def addSolidFill(self, r, g, b, a=0xff):
        return mingc.SWFShape_addSolidFill(self.this, r, g, b, a)

    def addGradientFill(self, gradient, flags):
        return mingc.SWFShape_addGradientFill(self.this, gradient.this, flags)

    def addBitmapFill(self, bitmap, flags):
        return mingc.SWFShape_addBitmapFill(self.this, bitmap.this, flags)

# addFill flags:
SWFFILL_LINEAR_GRADIENT = mingc.SWFFILL_LINEAR_GRADIENT
SWFFILL_RADIAL_GRADIENT = mingc.SWFFILL_RADIAL_GRADIENT
SWFFILL_TILED_BITMAP    = mingc.SWFFILL_TILED_BITMAP
SWFFILL_CLIPPED_BITMAP  = mingc.SWFFILL_CLIPPED_BITMAP


class SWFFill(SWFBase):

    def __init__(self, o):
        self.this = o

    def __del__(self):
        mingc.destroySWFFill(self.this)

    def rotateTo(self, degrees):
        mingc.SWFFill_rotateTo(self.this, degrees)

    def moveTo(self, x, y):
        mingc.SWFFill_moveTo(self.this, x, y)

    def scaleTo(self, xScale, yScale=None):
        if yScale is None:
            mingc.SWFFill_scaleXYTo(self.this, xScale, xScale)
        else:
            mingc.SWFFill_scaleXYTo(self.this, xScale, yScale)

    def scaleXTo(self, xScale):
        mingc.SWFFill_scaleXTo(self.this, xScale)

    def scaleYTo(self, yScale):
        mingc.SWFFill_scaleYTo(self.this, yScale)

    def skewXTo(self, x):
        mingc.SWFFill_skewXTo(self.this, x);

    def skewYTo(self, y):
        mingc.SWFFill_skewYTo(self.this, y)


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


class SWFMovie(SWFBase):

    def __init__(self):
        self.this = mingc.newSWFMovie()
        self.blocks = []

    def __del__(self):
        mingc.destroySWFMovie(self.this)

    def setRate(self, rate):
        mingc.SWFMovie_setRate(self.this, rate)

    def setDimension(self, x, y):
        mingc.SWFMovie_setDimension(self.this, x, y)

    def setBackground(self, r, g, b):
        mingc.SWFMovie_setBackground(self.this, r, g, b)

    def setFrames(self, totalFrames):
        mingc.SWFMovie_setNumberOfFrames(self.this, totalFrames)

    # or:
    def setNumberOfFrames(self, totalFrames):
        mingc.SWFMovie_setNumberOfFrames(self.this, totalFrames)

    def nextFrame(self):
        mingc.SWFMovie_nextFrame(self.this)

    def add(self, block):
        self.blocks.append(block)
        return SWFDisplayItem(mingc.SWFMovie_add(self.this, block.this))

    def remove(self, item):
        mingc.SWFMovie_remove(self.this, item.this)

    def streamMp3(self, mp3):
        sound = SWFSound(mp3)
        self.blocks.append(sound)
        mingc.SWFMovie_setSoundStream(self.this, sound)

    def setSoundStream(self, sound):
        self.blocks.append(sound)
        mingc.SWFMovie_setSoundStream(self.this, sound.this);

    def output(self):
        return mingc.SWFMovie_simpleOutput(self.this)

    def save(self, filename):
        mingc.SWFMovie_save(self.this, filename)

    def saveToFile(self, file):
        mingc.SWFMovie_saveToFileNo(self.this, file.fileno())

    def labelFrame(self, label):
        mingc.SWFMovie_labelFrame(self.this, label)

    # deprecated:
    def simpleOutput(self):
        return mingc.SWFMovie_simpleOutput(self.this)


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


class SWFFont(SWFBase):

    def __init__(self, name):
        if name[-4:] == '.fdb':
            self.browserfont = 0
            self.this = mingc.loadSWFFontFromFile(open(name, "rb"))
        else:
            self.browserfont = 1
            self.this = mingc.newSWFBrowserFont(name)

    def __del__(self):
        if self.browserfont:
            mingc.destroySWFBrowserFont(self.this)
        else:
            mingc.destroySWFFont(self.this)

    def getAscent(self):
        return mingc.SWFFont_getAscent(self.this)

    def getDescent(self):
        return mingc.SWFFont_getDescent(self.this)

    def getLeading(self):
        return mingc.SWFFont_getLeading(self.this)

    def getStringWidth(self, string):
        return mingc.SWFFont_getStringWidth(self.this, string)

    # or: (?)
    def getWidth(self, string):
        return mingc.SWFFont_getStringWidth(self.this, string)


class SWFBitmap(SWFBase):

    def __init__(self, fname, alpha=None):
        ext = fname[-4:]

        if ext == '.dbl' or ext == '.DBL':
            self.file = open(fname, "rb")
            self.this = mingc.newSWFDBLBitmap(self.file)

        elif ext == '.jpg' or ext == '.JPG':
            self.file = open(fname, "rb")
            if alpha is None:
                self.this = mingc.newSWFJpegBitmap(self.file)
            else:
                self.alpha = open(alpha, "rb")
                self.this = mingc.newSWFJpegWithAlpha(self.file, self.alpha)

    def __del__(self):
        mingc.destroySWFBitmap(self.this)

    def getWidth(self):
        return mingc.SWFBitmap_getWidth(self.this)

    def getHeight(self):
        return mingc.SWFBitmap_getHeight(self.this)


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

    def __init__(self):
        self.this = mingc.newSWFText2()

    def __del__(self):
        mingc.destroySWFText(self.this)

    def setFont(self, font):
        mingc.SWFText_setFont(self.this, font.this)

    def setHeight(self, height):
        mingc.SWFText_setHeight(self.this, height)

    def moveTo(self, x, y):
        mingc.SWFText_moveTo(self.this, x, y)

    def setColor(self, r, g, b, a=0xff):
        mingc.SWFText_setColor(self.this, r, g, b, a)

    def addString(self, s, advance=None):
        mingc.SWFText_addString(self.this, s, advance)

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

    # deprecated:
    def setXY(self, x, y):
        mingc.SWFText_setXY(self.this, x, y)


class SWFText2(SWFText):

    def __init__(self):
        self.this = mingc.newSWFText2()


class SWFTextField(SWFBase):

    def __init__(self, flags=None):
        self.this = mingc.newSWFTextField()

        if flags is not None:
            mingc.SWFTextField_setFlags(self.this, flags)

    def __del__(self):
        mingc.destroySWFTextField(self.this)

    def setFont(self, font):
	self.font = font
        mingc.SWFTextField_setFont(self.this, font)

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

# textfield alignment flags:
SWFTEXTFIELD_ALIGN_LEFT    = mingc.SWFTEXTFIELD_ALIGN_LEFT
SWFTEXTFIELD_ALIGN_RIGHT   = mingc.SWFTEXTFIELD_ALIGN_RIGHT
SWFTEXTFIELD_ALIGN_CENTER  = mingc.SWFTEXTFIELD_ALIGN_CENTER
SWFTEXTFIELD_ALIGN_JUSTIFY = mingc.SWFTEXTFIELD_ALIGN_JUSTIFY

# other flags:
SWFTEXTFIELD_HASLENGTH = mingc.SWFTEXTFIELD_HASLENGTH
SWFTEXTFIELD_NOEDIT    = mingc.SWFTEXTFIELD_NOEDIT
SWFTEXTFIELD_PASSWORD  = mingc.SWFTEXTFIELD_PASSWORD
SWFTEXTFIELD_MULTILINE = mingc.SWFTEXTFIELD_MULTILINE
SWFTEXTFIELD_WORDWRAP  = mingc.SWFTEXTFIELD_WORDWRAP
SWFTEXTFIELD_DRAWBOX   = mingc.SWFTEXTFIELD_DRAWBOX
SWFTEXTFIELD_NOSELECT  = mingc.SWFTEXTFIELD_NOSELECT


class SWFSound(SWFBase):

    def __init__(self, fname):
        self.file = open(fname, "rb")
        self.this = mingc.newSWFSound(self.file)

    # display list destroys this..


class SWFAction(SWFBase):

    def __init__(self, script):
        self.this = mingc.compileSWFActionCode(script)

    # assigned object will destroy this..


class SWFGradient(SWFBase):

    def __init__(self):
        self.this = mingc.newSWFGradient()

    def __del__(self):
        mingc.destroySWFGradient(self.this)

    def addEntry(self, ratio, r, g, b, a=0xff):
        mingc.SWFGradient_addEntry(self.this, ratio, r, g, b, a)


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
        mingc.SWFButton_addShape(self.this, character, flags)

    def addAction(self, action, flags):
        mingc.SWFButton_addAction(self.this, action, flags)


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
