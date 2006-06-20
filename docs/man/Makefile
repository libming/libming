include ../../Makefile.config

MAN1 = makeswf.1

MAN3 =	\
	destroySWFMovie.3		\
	Ming_getScale.3			\
	Ming_init.3			\
	Ming_setCubicThreshold.3	\
	Ming_setErrorFunction.3		\
	Ming_setScale.3			\
	Ming_setSWFCompression.3	\
	Ming_setWarnFunction.3		\
	Ming_useSWFVersion.3		\
	newSWFMovie.3			\
	newSWFMovieWithVersion.3	\
	SWFMovie_setDimension.3		\
	SWFMovie_setNumberOfFrames.3	\
	SWFMovie_setRate.3

install: 
	$(INSTALL) -d $(mandir)/man1
	$(INSTALL_DATA) $(MAN1) $(mandir)/man1
	$(INSTALL) -d $(mandir)/man3
	$(INSTALL_DATA) $(MAN3) $(mandir)/man3
