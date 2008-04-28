EXTRA_DIST = SWF/Action.pm SWF/Bitmap.pm SWF/Button.pm SWF/ButtonRecord.pm \
		SWF/Constants.pm \
		SWF/DisplayItem.pm SWF/Fill.pm SWF/Font.pm SWF/Gradient.pm \
		SWF/Morph.pm SWF/Movie.pm SWF/Shape.pm SWF/Sound.pm \
		SWF/SoundInstance.pm SWF/SoundStream.pm SWF/MovieClip.pm \
		SWF/Text.pm SWF/TextField.pm SWF/VideoStream.pm SWF/Sprite.pm \
		t/00_basic.t t/01_shape.t t/03_movie.t t/button.t t/drag.t \
		t/fill.t t/filljpeg.t t/gradient.t t/png.t t/sound.t \
		Action.xs Bitmap.xs Button.xs CREDITS Changes Constants.xs \
		DisplayItem.xs Exports.c Fill.xs Font.xs Gradient.xs MANIFEST \
		Makefile.PL Morph.xs Movie.xs MovieClip.xs README SUPPORT \
		SWF.pm SWF.xs Shape.xs Sound.xs SoundInstance.xs SoundStream.xs \
		TODO Text.xs TextField.xs VideoStream.xs FontCharacter.xs ButtonRecord.xs\
		perl_swf.h swf_util.c \
		typemap examples/action.cgi examples/alphafill.cgi \
		examples/animation.cgi examples/glyph.cgi examples/jpegfill.cgi \
		examples/keypress.cgi examples/morph.cgi examples/shape.cgi \
		examples/sound.cgi examples/sprite.cgi examples/text.cgi 
		examples/textfield.cgi examples/video.cgi \
		common/_sans.fdb common/README common/beep.wav \
		mkdoc

abs_builddir=$(shell cd $(top_builddir)/perl_ext; pwd)

all: Makefile.perl
	$(MAKE) -f Makefile.perl

install: Makefile.perl
	$(MAKE) -f Makefile.perl install

Makefile.perl: Makefile.PL
	srcdir=$(srcdir) builddir=$(abs_builddir) $(PERL) $< PREFIX=$(prefix) FIRST_MAKEFILE=Makefile.perl

clean-local: Makefile.perl
	$(MAKE) -f Makefile.perl realclean
	rm -f Makefile.perl

distclean-local:
	rm -f t/config.pl
