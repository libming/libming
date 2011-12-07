/****************************************************************************
 *
 *  ActionScriptTest.c - Ming testing framework tool
 *
 *  Copyright (C) 2003 2004 2005 2006 2007 Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ***************************************************************************
 *
 *  Authors:
 * 	- "Sandro Santilli" <strk@keybit.net>
 *
 ***************************************************************************/

/* This is needed to get vasprintf definition */
#define _GNU_SOURCE 1

#include <libming.h>
#include <run_test.h>
#include <ctype.h>
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
#include <vasprintf.h>

static SWFMovie
compile(const char* filename, const char* ppfile, int version)
{
	SWFAction ac;
	SWFMovie mo = newSWFMovieWithVersion(version);
	makeswf_set_swfversion(version);

	ac = makeswf_compile_source(filename, ppfile, 0);

	SWFMovie_add(mo, (SWFBlock)ac);
	SWFMovie_nextFrame(mo);

	return mo;
}

static int
do_test(const char* name, int version)
{
	char source[PATH_MAX];
	char ppfile[PATH_MAX];

	sprintf(ppfile, "%s.as.pp", name);

	snprintf(source, PATH_MAX, "%s/%s.as", TOP_SOURCEDIR, name);
	source[PATH_MAX-1] = '\0';

	SWFMovie m = compile(source, ppfile, version);

	return run_test(m, name);
}

static int
do_tests()
{
	int failures=0;
	const char *all_tests = AS_TESTS;
	char testfile[PATH_MAX];
	char vstr[2];
	const char *from, *to, *end;
	char *ptr;
	size_t len = strlen(all_tests);

	from = all_tests;
	end = from+len;
	do
	{
		while (*from && isspace(*from)) ++from;
		if ( ! *from ) break;

		to = from + strcspn(from, " \t");

		len = to-from;
		if ( len+1 >= PATH_MAX )
		{
			fprintf(stderr,
				"Name of test exceeds PATH_MAX, skipping"
				" [from: %s]\n", from);
			from = to;
			continue;
		}
		vstr[0] = from[len - 1];
		vstr[1] = '\0';
		strncpy(testfile, from, len-2);
		testfile[len-2]='\0';
		/* strip the .as part (if any) */
		if ( (ptr=strstr(testfile+len-5, ".as")) )
		{
			*ptr='\0';
		}

		printf("Testing %s with swfversion %i\n", testfile, atoi(vstr));

		failures += do_test(testfile, atoi(vstr));

		from=to;

	} while (from < end);

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

	// TODO: use multiple SWF target versions !

	Ming_setSWFCompression(swfcompression);

	return do_tests();

}
