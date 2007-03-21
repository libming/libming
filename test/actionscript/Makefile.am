SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

TESTS = ActionScriptTest

AS_TESTS = \
	Function.as	\
	ASM0.as		\
	ASM1.as		\
	ASM_all.as	\
	ASM_extend.as	\
	ASM_push.as	\
	delete.as	\
	GotoFrame.as	\
	logor.as

EXTRA_DIST = 		\
	Function.ref	\
	ASM1.ref	\
	ASM0.ref	\
	ASM_all.ref	\
	ASM_extend.ref	\
	ASM_push.ref	\
	delete.ref	\
	logor.ref	\
	GotoFrame.ref	\
	$(AS_TESTS)

CLEANFILES = *.pp *.swf

check_PROGRAMS = \
	ActionScriptTest

ActionScriptTest_SOURCES = ActionScriptTest.c ../run_test.c ../../util/makeswf_utils.c
ActionScriptTest_LDADD = $(top_builddir)/src/libming.la
ActionScriptTest_CFLAGS = -DTOP_BUILDDIR='"$(top_builddir)"' -DTOP_SOURCEDIR='"$(srcdir)"' -I$(top_srcdir)/util/ -DAS_TESTS='"$(AS_TESTS)"'

