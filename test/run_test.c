#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <ming.h>

int
run_test(SWFMovie m, const char *testname)
{
	char ref[PATH_MAX];
	char output[PATH_MAX];
#ifdef TOP_SOURCEDIR
	const char* refdir=TOP_SOURCEDIR;
#else
	const char* refdir=getenv("srcdir");
#endif
#ifdef TOP_BUILDDIR
	const char* top_builddir=TOP_BUILDDIR;
#else
	const char* top_builddir=getenv("top_builddir");
#endif
	int ret;
	char run[1024];

	if ( ! ref || ! top_builddir )
	{
		fprintf(stderr, "Please export srcdir and top_builddir vars\n");
		return 1;
	}

	sprintf(ref, "%s/%s.ref", refdir, testname);
	sprintf(output, "%s.swf", testname);

#if 0
	printf("ref: %s\n", ref);
	printf("output: %s\n", output);
	printf("top_builddir: %s\n", top_builddir);
#endif

	/* Save the movie in current directory */
	SWFMovie_save(m, output);

	sprintf(run, "%s/util/listswf %s | diff - %s",
		top_builddir, output, ref);

	printf("%s\n", run);

	if ( system(run) ) return 1;
	return 0;
}
