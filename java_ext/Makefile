
JAVADIR=/usr/java
MINGDIR=..

COMPILE = $(JAVADIR)/bin/javac
JAR = $(JAVADIR)/bin/jar


SRCS = \
	SWFColor.java \
	SWFMatrix.java \
	SWFAction.java \
	SWFActionI.java \
	SWFBitmap.java \
	SWFBitmapI.java \
	SWFButton.java \
	SWFButtonI.java \
	SWFDisplayItem.java \
	SWFDisplayItemI.java \
	SWFException.java \
	SWFFill.java \
	SWFFillI.java \
	SWFFont.java \
	SWFFontI.java \
	SWFGradient.java \
	SWFGradientI.java \
	SWFMorph.java \
	SWFMorphI.java \
	SWFMovie.java \
	SWFMovieClip.java \
	SWFMovieClipI.java \
	SWFMovieI.java \
	SWFObject.java \
	SWFObjectI.java \
	SWFShape.java \
	SWFShapeI.java \
	SWFSound.java \
	SWFSoundI.java \
	SWFText.java \
	SWFTextField.java \
	SWFTextFieldI.java \
	SWFTextI.java


all: jswf.jar

%.class: %.java
	$(COMPILE) $<  

jswf.jar: $(patsubst %.java,%.class,${SRCS})
	$(JAR) cvf jswf.jar $+

clean:
	rm -f *.class *.jar

