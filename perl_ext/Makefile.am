EXTRA_DIST = SWF/Action.pm SWF/BinaryData.pm SWF/Bitmap.pm SWF/Blur.pm \
		SWF/Button.pm SWF/ButtonRecord.pm SWF/Constants.pm \
		SWF/CXform.pm SWF/DisplayItem.pm SWF/Fill.pm \
		SWF/FilterMatrix.pm SWF/Filter.pm SWF/Font.pm \
		SWF/FontCharacter.pm SWF/Gradient.pm SWF/InitAction.pm \
		SWF/Matrix.pm SWF/Morph.pm SWF/Movie.pm SWF/PrebuiltClip.pm \
		SWF/Shape.pm SWF/Shadow.pm SWF/Sound.pm \
		SWF/SoundInstance.pm SWF/SoundStream.pm SWF/MovieClip.pm \
		SWF/Text.pm SWF/TextField.pm SWF/VideoStream.pm SWF/Sprite.pm \
		SWF/BrowserFont.pm SWF/FontCollection.pm SWF/Character.pm\
		t/00_basic.t t/01_shape.t t/03_movie.t t/button.t t/drag.t \
		t/fill.t t/filljpeg.t t/gradient.t t/movieclip.t t/png.t \
		t/sound.t \
		Action.xs BinaryData.xs Bitmap.xs Blur.xs Button.xs CREDITS \
		Changes Constants.xs CXform.xs BrowserFont.xs  FontCollection.xs\
		DisplayItem.xs Exports.c Fill.xs FilterMatrix.xs Filter.xs \
		Font.xs Gradient.xs InitAction.xs MANIFEST Makefile.PL \
		Matrix.xs Morph.xs Movie.xs MovieClip.xs PrebuiltClip.xs \
		README SUPPORT SWF.pm.in SWF.xs Shadow.xs Shape.xs Sound.xs \
		SoundInstance.xs SoundStream.xs TODO Text.xs TextField.xs \
		VideoStream.xs FontCharacter.xs ButtonRecord.xs Character.xs\
		perl_swf.h swf_util.c typemap examples/action.cgi \
		examples/alphafill.cgi  examples/animation.cgi \
		examples/glyph.cgi examples/jpegfill.cgi \
		examples/keypress.cgi examples/morph.cgi examples/shape.cgi \
		examples/sound.cgi examples/sprite.cgi examples/text.cgi \
		examples/textfield.cgi examples/video.cgi \
		common/_sans.fdb common/README common/beep.wav \
		mkdoc

#TMP_DIR=$(top_builddir)/perl_ext
#abs_builddir=`cd $(TMP_DIR); pwd`

all: Makefile.perl
	$(MAKE) -f Makefile.perl

install: Makefile.perl
	$(MAKE) -f Makefile.perl install PREFIX=$(prefix)

Makefile.perl: Makefile.PL
	srcdir=$(srcdir) builddir=$(abs_builddir) $(PERL) $< PREFIX=$(prefix) FIRST_MAKEFILE=Makefile.perl

clean-local: Makefile.perl
	$(MAKE) -f Makefile.perl realclean
	rm -f Makefile.perl

distclean-local:
	rm -f t/config.pl

check: all
