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
	ASM_targetPath.as \
	delete.as	\
	GotoFrame.as	\
	logor.as 	\
	targetPath.as	\
	FunctionInLoop.as \
	newline_comment.as \
	newline_crlf.as	\
	TryCatch.as 

EXTRA_DIST = 		\
	Function.ref	\
	ASM1.ref	\
	ASM0.ref	\
	ASM_all.ref	\
	ASM_extend.ref	\
	ASM_push.ref	\
	ASM_targetPath.ref \
	delete.ref	\
	logor.ref	\
	GotoFrame.ref	\
	targetPath.ref  \
	FunctionInLoop.ref \
	newline_comment.ref \
	newline_crlf.ref \
	TryCatch.ref \
	$(AS_TESTS)

CLEANFILES = *.pp *.swf

check_PROGRAMS = \
	ActionScriptTest

ActionScriptTest_SOURCES = ActionScriptTest.c ../run_test.c ../../util/makeswf_utils.c
ActionScriptTest_LDADD = $(top_builddir)/src/libming.la
ActionScriptTest_CFLAGS = -DTOP_BUILDDIR='"$(top_builddir)"' -DTOP_SOURCEDIR='"$(srcdir)"' -I$(top_srcdir)/util/ -DAS_TESTS='"$(AS_TESTS)"'

