require 'mingc'
require 'final'        # for Ruby 1.4
include Mingc
include ObjectSpace

class SWFBase
	attr_accessor :this

	def initialize(this)
		@this = this
	end
end

class SWFRect <SWFBase
	def initialize(minX, maxX, minY, maxY)
		super newSWFRect(minX, maxX, minY, maxY)
		define_finalizer(self) {destroySWFRect(@this)}
	end

	def getWidth
		return SWFRect_getWidth(@this)
	end

	def getHeight
		return SWFRect_getHeight(@this)
	end
end

class SWFShape <SWFBase
	def initialize(o = nil)
		@fills = []
		if o.nil?
			super newSWFShape()
		else
			super o
		end
		define_finalizer(self) {destroySWFShape(@this)}
	end

	def setLine(width, r, g, b, a=0xff)
		return SWFShape_setLine(@this, width, r, g, b, a)
	end

	# I know there's probably a better way to do this..
	def addFill(arg1, arg2=0, arg3=nil, arg4=0xff)
		if arg3 != nil
			return SWFFill.new(SWFShape_addSolidFill(@this, arg1, arg2, arg3, arg4))
		end
		if arg1.is_a?(SWFGradient)
			# XXX - have to keep reference to gradient so it's not disposed
			@fills.push(arg1)
			return SWFFill.new(SWFShape_addGradientFill(@this, arg1.this, arg2))
		end

		if arg1.is_a?(SWFBitmap)
			# XXX - have to keep reference to bitmap so it's not disposed
			@fills.push(arg1)
			return SWFFill.new(SWFShape_addBitmapFill(@this, arg1.this, arg2))
		else
			raise "bad argument to SWFShape::addFill"
		end
	end

	def setLeftFill(fill)
		SWFShape_setLeftFill(@this, fill.this)
	end

	def setRightFill(fill)
		SWFShape_setRightFill(@this, fill.this)
	end

	def movePenTo(x, y)
		SWFShape_movePenTo(@this, x, y)
	end

	def drawLineTo(x, y)
		SWFShape_drawLineTo(@this, x, y)
	end

	def drawLine(dx, dy)
		SWFShape_drawLine(@this, dx, dy)
	end

	def drawRect(rect)
		SWFShape_drawRect(@this, rect.this)
	end

	def drawCurveTo(bx, by, cx, cy, dx=nil, dy=nul)
		if dx != nil
			SWFShape_drawCubicTo(@this, bx, by, cx, cy, dx, dy)
		else
			SWFShape_drawCurveTo(@this, bx, by, cx, cy)
		end
	end

	def drawCurve(bx, by, cx, cy, dx=nil, dy=nil)
		if dx != nil
			SWFShape_drawCubic(@this, bx, by, cx, cy, dx, dy)
		else
			SWFShape_drawCurve(@this, bx, by, cx, cy)
		end
	end

	def drawCubicTo(bx, by, cx, cy, dx, dy)
		SWFShape_drawCubicTo(@this, bx, by, cx, cy, dx, dy)
	end

	def drawCurve(bx, by, cx, cy, dx, dy)
		SWFShape_drawCubic(@this, bx, by, cx, cy, dx, dy)
	end

	def drawGlyph(font, char, size=0)
		SWFShape_drawSizedGlyph(@this, font.this, char[0], size) #ord(char[0]))
	end

	def addSolidFill(r, g, b, a=0xff)
		return SWFFill.new(SWFShape_addSolidFill(@this, r, g, b, a))
	end

	def addGradientFill(gradient, flags)
		return SWFShape_addGradientFill(@this, gradient.this, flags)
	end

	def addBitmapFill(bitmap, flags)
		return SWFShape_addBitmapFill(@this, bitmap.this, flags)
	end
end

class SWFFill <SWFBase
	def initialize(o)
		define_finalizer(self) {destroySWFFill(@this)}
		super o
	end

	def rotateTo(degrees)
		SWFFill_rotateTo(@this, degrees)
	end

	def moveTo(x, y)
		SWFFill_moveTo(@this, x, y)
	end

	def scaleTo(xScale, yScale=nil)
		if yScale.nil?
			SWFFill_scaleXYTo(@this, xScale, xScale)
		else
			SWFFill_scaleXYTo(@this, xScale, yScale)
		end
	end

	def scaleXTo(xScale)
		SWFFill_scaleXTo(@this, xScale)
	end

	def scaleYTo(yScale)
		SWFFill_scaleYTo(@this, yScale)
	end

	def skewXTo(x)
		SWFFill_skewXTo(@this, x);
	end

	def skewYTo(y)
		SWFFill_skewYTo(@this, y)
	end

	def setMatrix(a, b, c, d, x, y)
		SWFFill_setMatrix(@this, a, b, c, d, x, y)
	end
end

class SWFDisplayItem <SWFBase
	def rotate(degrees)
		SWFDisplayItem_rotate(@this, degrees)
	end

	def rotateTo(degrees)
		SWFDisplayItem_rotateTo(@this, degrees)
	end

	def move(x, y)
		SWFDisplayItem_move(@this, x, y)
	end

	def moveTo(x, y)
		SWFDisplayItem_moveTo(@this, x, y)
	end

	def scale(xScale, yScale)
		SWFDisplayItem_scale(@this, xScale, yScale)
	end

	def scaleTo(xScale, yScale)
		SWFDisplayItem_scaleTo(@this, xScale, yScale)
	end

	def skewX(x)
		SWFDisplayItem_skewX(@this, x)
	end

	def skewXTo(x)
		SWFDisplayItem_skewXTo(@this, x);
	end

	def skewY(y)
		SWFDisplayItem_skewY(@this, y)
	end

	def skewYTo(y)
		SWFDisplayItem_skewYTo(@this, y)
	end

	def setMatrix(a, b, c, d, x, y)
		SWFDisplayItem_setMatrix(@this, a, b, c, d, x, y)
	end

	def setName(name)
		SWFDisplayItem_setName(@this, name)
	end

	def setRatio(ratio)
		SWFDisplayItem_setRatio(@this, ratio)
	end

	def getDepth
		return SWFDisplayItem_getDepth(@this)
	end

	def setDepth(depth)
		SWFDisplayItem_setDepth(@this, depth)
	end

	def addColor(r, g, b, a=0)
		SWFDisplayItem_addColor(@this, r, g, b, a)
	end

	def multColor(r, g, b, a=1.0)
		SWFDisplayItem_multColor(@this, r, g, b, a)
	end

	def remove
		SWFDisplayItem_remove(@this)
	end
end

class SWFMovie <SWFBase
	def initialize
		super newSWFMovie()
		@blocks = []
		define_finalizer(self) {destroySWFMovie(@this)}
	end

	def setRate(rate)
		SWFMovie_setRate(@this, rate)
	end

	def setDimension(x, y)
		SWFMovie_setDimension(@this, x, y)
	end

	def setBackground(r, g, b)
		SWFMovie_setBackground(@this, r, g, b)
	end

	def setFrames(totalFrames)
		SWFMovie_setNumberOfFrames(@this, totalFrames)
	# or:
	end

	def setNumberOfFrames(totalFrames)
		SWFMovie_setNumberOfFrames(@this, totalFrames)
	end

	def nextFrame
		SWFMovie_nextFrame(@this)
	end

	def add(block)
		@blocks.push(block)
		return SWFDisplayItem.new(SWFMovie_add(@this, block.this))
	end

	def remove(item)
		SWFMovie_remove(@this, item.this)
	end

	def streamMp3(sound)
		@blocks.push(sound)
		SWFMovie_setSoundStream(@this, sound.this);
	end

	# deprecated:
	def setSoundStream(sound)
		@blocks.push(sound)
		SWFMovie_setSoundStream(@this, sound.this);
	end

	def output
		return SWFMovie_simpleOutput(@this)
	end

	# deprecated: (?)
	def simpleOutput
		return SWFMovie_simpleOutput(@this)
	end

	def save(filename)
		SWFMovie_save(@this, filename)
	end

	def labelFrame(label)
		SWFMovie_labelFrame(@this, label)
	end
end

class SWFSprite <SWFBase
	def initialize
		super newSWFMovieClip()
		@blocks = []
		define_finalizer(self) {destroySWFMovieClip(@this)}
	end

	def setNumberOfFrames(frames)
		SWFMovieClip_setNumberOfFrames(@this, frames)
	end

	def add(block)
		@blocks.push(block)
		return SWFDisplayItem.new(SWFMovieClip_add(@this, block.this))
	end

	def remove(item)
		SWFMovieClip_remove(@this, item.this)
	end

	def nextFrame
		SWFMovieClip_nextFrame(@this)
	end

	def labelFrame(label)
		SWFMovieClip_labelFrame(@this, label)
	end
end

# deprecated
class SWFMovieClip <SWFSprite

class SWFGradient <SWFBase
	def initialize
		super newSWFGradient()
	end

	def addEntry(ratio, r, g, b, a)
		return SWFGradient_addEntry(@this, ratio, r, g, b, a)
	end
end

class SWFMorph <SWFBase
	def initialize
		super newSWFMorphShape()
		define_finalizer(self) {destroySWFMorph(@this)}
	end

	def getShape1
		# have to keep a reference so it doesn't scope out
		@shape1 = SWFShape.new(SWFMorph_getShape1(@this))
		return @shape1
	end

	def getShape2
		@shape2 = SWFShape.new(SWFMorph_getShape2(@this))
		return @shape2
	end
end

# deprecated
class SWFMorphShape <SWFMorph

class SWFFont <SWFBase

	def initialize(name)
		if name =~ /\.fdb$/
			@browserfont = 0
			super loadSWFFontFromFile(open(name))
		else
			@browserfont = 1
			super newSWFBrowserFont(name)
		end

		define_finalizer(self) {
			if @browserfont == 1
				destroySWFBrowserFont(@this)
			else
				destroySWFFont(@this)
			end
		}
	end

	def getAscent
		return SWFFont_getAscent(@this)
	end

	def getDescent
		return SWFFont_getDescent(@this)
	end

	def getLeading
		return SWFFont_getLeading(@this)
	end

	def getWidth(string)
		return SWFFont_getWidth(@this, string)
	end
end

class SWFBitmap <SWFBase
	def initialize(fname, alpha=nil)
		if fname =~ /\.dbl$/i
			@file = open(fname)
			super newSWFDBLBitmap(@file)
		elsif fname =~ /\.jpg$/i
			@file = open(fname)
			if alpha.nil?
				super newSWFJpegBitmap(@file)
			else
				@alpha = open(alpha)
				super newSWFJpegWithAlpha(@file, @alpha)
			end
		end
		define_finalizer(self) {destroySWFBitmap(@this)}
	end

	def getWidth
		return SWFBitmap_getWidth(@this)
	end

	def getHeight
		return SWFBitmap_getHeight(@this)
	end
end

# deprecated
class SWFDBLBitmap <SWFBitmap
	def initialize(fname)
		super newSWFDBLBitmap(open(fname))
	end
end

# deprecated
class JpegBitmap <SWFBitmap
	def initialize(fname, alpha=nil)
		if alpha.nil?
			super newSWFJpegBitmap(open(fname))
		else
			super newSWFJpegWithAlpha(open(fname), open(alpha))
 		end
 	end
end

class SWFText <SWFBase
	def initialize
		super newSWFText2()
		define_finalizer(self) {destroySWFText(@this)}
	end

	def setFont(font)
		SWFText_setFont(@this, font.this)
	end

	def setHeight(height)
		SWFText_setHeight(@this, height)
	end

	def moveTo(x, y)
		SWFText_moveTo(@this, x, y)
	end

	def setColor(r, g, b, a=0xff)
		SWFText_setColor(@this, r, g, b, a)
	end

	def addString(s, advance=nil)
		SWFText_addString(@this, s, advance)
	end

	def setSpacing(spacing)
		SWFText_setSpacing(@this, spacing)
	end

	def getAscent
		return SWFText_getAscent(@this)
	end

	def getDescent
		return SWFText_getDescent(@this)
	end

	def getLeading
		return SWFText_getLeading(@this)
	end

	def getWidth(string)
		return SWFText_getWidth(@this, string)
	end
end

class SWFTextField <SWFBase
 	def initialize
		super newSWFTextField()
		define_finalizer(self) {destroySWFTextField(@this)}
	end

	def setFont(font)
		SWFTextField_setFont(@this, font.this)
	end

	def setBounds(width, height)
		SWFTextField_setBounds(@this, width, height)
 	end

	def setFlags(flags)
		SWFTextField_setFlags(@this, flags)
 	end

	def setColor(r, g, b, a=0xff)
		SWFTextField_setColor(@this, r, g, b, a)
	end

	def setVariableName(name)
		SWFTextField_setVariableName(@this, name)
	end

	def addString(string)
		SWFTextField_addString(@this, string)
	end

	def setHeight( height)
		SWFTextField_setHeight(@this,  height)
	end

	def setLeftMargin(leftMargin)
		SWFTextField_setLeftMargin(@this, leftMargin)
	end

	def setRightMargin(rightMargin)
		SWFTextField_setRightMargin(@this, rightMargin)
	end

	def setIndentation(indentation)
		SWFTextField_setIndentation(@this, indentation)
	end

	def setLineSpacing(lineSpacing)
		SWFTextField_setLineSpacing(@this, lineSpacing)
	end

	def setAlignment(alignment)
		SWFTextField_setAlignment(@this,  alignment)
	# or just
	end

	def align(alignment)
		SWFTextField_setAlignment(@this,  alignment)
	end

	def setLength(length)
		SWFTextField_setLength(@this, length)
	end
end

class SWFSound <SWFBase
	def initialize(fname)
		@file = open(fname)
		super newSWFSound(@file)
	end
	# display list destroys this..
end

class SWFAction <SWFBase
	def initialize(script)
		super compileSWFActionCode(script)
	end
	# assigned object will destroy this..
end

class SWFButton <SWFBase
 	def initialize
		super newSWFButton()
		@shapes = []
		define_finalizer(self) {destroySWFButton(@this)}
	end

	def setUp(character)
		addShape(character, SWFBUTTON_UP)
	end

	def setDown(character)
		addShape(character, SWFBUTTON_DOWN)
	end

	def setOver(character)
		addShape(character, SWFBUTTON_OVER)
	end

	def setHit(character)
		addShape(character, SWFBUTTON_HIT)
	end

	def addShape(character, flags)
		@shapes.push(character)
		SWFButton_addShape(@this, character.this, flags)
	end

	def addAction(action, flags)
		SWFButton_addAction(@this, action.this, flags)
	end

	def SWFButton.SWFBUTTON_KEYPRESS(c)
		return swfButton_keypress(c)
	end
end
