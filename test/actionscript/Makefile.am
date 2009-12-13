SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

TESTS = ActionScriptTest

#filename:swfversion
AS_TESTS = \
	Function.as:6		\
	ASM0.as:6		\
	ASM1.as:6		\
	ASM_all.as:6		\
	ASM_extend.as:6		\
	ASM_push.as:6		\
	ASM_targetPath.as:6 	\
	delete.as:6		\
	GotoFrame.as:6		\
	logor.as:6 		\
	targetPath.as:6		\
	FunctionInLoop.as:6	\
	newline_comment.as:6 	\
	newline_crlf.as:6	\
	TryCatch.as:6		\
	forin.as:6		\
	legacy_functions.as:6 	\
	drag.as:6		\
	class_empty.as:6	\
	class_method.as:6 	\
	class_member.as:6	\
	ASM_V4.as:4		\
	condition.as:6		\
	With.as:6		\
	switch.as:6		\
	getURL2.as:6		\
	substr.as:6		\
	for.as:6

AS_TEST_FILES = \
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
	TryCatch.as	\
	forin.as	\
	legacy_functions.as \
	drag.as		\
	class_empty.as	\
	class_method.as \
	class_member.as \
	ASM_V4.as	\
	condition.as	\
	With.as		\
	switch.as	\
	getURL2.as	\
	for.as	
	

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
	forin.ref \
	legacy_functions.ref \
	drag.ref	\
	class_empty.ref	\
	class_method.ref \
	class_member.ref \
	ASM_V4.ref	\
	condition.ref	\
	With.ref	\
	switch.ref	\
	getURL2.ref	\
	for.ref		\
	substr.ref	\
	$(AS_TEST_FILES)

CLEANFILES = *.pp *.swf

check_PROGRAMS = \
	ActionScriptTest

ActionScriptTest_SOURCES = ActionScriptTest.c ../run_test.c ../../util/makeswf_utils.c
ActionScriptTest_LDADD = $(top_builddir)/src/libming.la
ActionScriptTest_CFLAGS = -DTOP_BUILDDIR='"$(top_builddir)"' -DTOP_SOURCEDIR='"$(srcdir)"' -I$(top_srcdir)/util/ -DAS_TESTS='"$(AS_TESTS)"'

