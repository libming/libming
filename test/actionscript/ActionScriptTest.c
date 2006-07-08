/* This is needed to get vasprintf definition */
#define _GNU_SOURCE 1

#include <libming.h>
#include <run_test.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <makeswf.h>

int swfversion=6;

static SWFMovie
compile(const char* filename)
{
	SWFAction ac;
	SWFMovie mo = newSWFMovie();

	ac = makeswf_compile_source(filename);

	SWFMovie_add(mo, (SWFBlock)ac);
	SWFMovie_nextFrame(mo);

	return mo;
}

static int
do_test(const char* name)
{
	char source[PATH_MAX];

	snprintf(source, PATH_MAX, "%s/%s.as", TOP_SOURCEDIR, name);
	source[PATH_MAX-1] = '\0';

	SWFMovie m = compile(source);

	return run_test(m, name);
}

static int
do_tests()
{
	int failures=0;
	const char* all_tests = AS_TESTS;

	printf("Should test all tests (%s), but this is not implemented yet, so I'll test the hard-coded \"Function.as\" one\n", all_tests);

	failures += do_test("Function");

	return failures;
}

int
main()
{
	int swfcompression=-1; // be fast, we don't care
	                       // about compression, do we ?
	if ( Ming_init() )
	{
		fprintf(stderr, "Ming initialization error\n");
		exit(EXIT_FAILURE);
	}
	Ming_useSWFVersion(swfversion);
	makeswf_set_swfversion(swfversion);
	Ming_setSWFCompression(swfcompression);

	return do_tests();

}
