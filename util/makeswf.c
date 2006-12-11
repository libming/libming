/****************************************************************************
 *
 *  makeswf - a command line actionscript compiler
 *
 *  Copyright (C) 2003 2004 2005 2006 "Sandro Santilli" <strk@keybit.net>
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
 ****************************************************************************
 * 
 *  Intro
 *  -----
 *
 *  This is a simple Flash actionscript compiler that
 *  uses libming to do its job. You'll find it useful
 *  only when coding in pure actionscript, thus probably
 *  only when using the Flash 6 drawing API.
 *
 *  Input files are preprocessed 
 *  -D can be used to set macros and -I to add dirs to include search paths.
 *  You can skip the preprocessing step using the -p switch, but this
 *  is undocumented/deprecated.
 *
 *  If you need another kind of preprocessing change the CPP define on
 *  top of this file.
 *
 *  Build
 *  -----
 *
 *  This binary will link dinamically to libming, so you should
 *  install ming somewhere, or let the linker know where to find
 *  the ming library.
 *
 *  TODO
 *  ----
 *
 *  - Write preprocessor output to <output>.as.pp rather then <source>.as.pp
 *    to have more chances at succeeding (source dir can be unwriteable)
 *
 ***************************************************************************/

/* This is needed to get vasprintf definition */
#define _GNU_SOURCE 1

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <ming.h>
#include <ming_config.h>
#include "makeswf.h"
#ifdef HAVE_GETOPT
#include <getopt.h>
#endif

// Cheating, but it works (not sure why the above ifdef for getopt isn't)
#ifdef _WIN32
#include <getopt.h>
#endif

#ifndef HAVE_VASPRINTF
/* Workaround for the lack of vasprintf()
 * As found on: http://unixpapa.com/incnote/stdio.html
 * Seems to be Public Domain
 */
int
vasprintf(char **ret, const char *format, va_list ap)
{
	va_list ap2;
	int len = 100;        /* First guess at the size */
	if ((*ret = (char *) malloc(len)) == NULL)
	{
		return -1;
	}
	while (1)
	{
		int nchar;
		va_copy(ap2, ap);
		nchar= vsnprintf(*ret, len, format, ap2);
		if (nchar > -1 && nchar < len)
		{
			return nchar;
		}
		if (nchar > len)
		{
			len= nchar+1;
		} else
		{
			len*= 2;
		}
		if ((*ret = (char *) realloc(*ret, len)) == NULL)
		{
			free(*ret);
			return -1;
		}
	}
}
#endif

#define DEFSWFVERSION 6
#define DEFSWFCOMPRESSION 9

#define CPP "cpp -P -C -Wall"
#define MAXERRORMSG 1024

/* prototypes */
static void add_import_spec(char *spec);
static int add_imports(void);
static void embed_image(SWFMovie movie, char *f);

/* data */
static char **import_specs;
static int numimport_specs = 0;
static int swfversion = DEFSWFVERSION;
static const char *RCSID = "$Id$";
SWFMovie mo;

void
usage (char *me, int ex)
{
	fprintf(stderr, "Usage: %s [OPTIONS] <frame_content> ...\n",
		me);
	fprintf(stderr, "<frame_content> can be: swf, png, jpg, actionscript code.\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, " -o <output>\n");
	fprintf(stderr, " -s <width>x<height>\n");
	fprintf(stderr, " -r <frame_rate>\n");
	fprintf(stderr, " -v <output_version>\n");
	fprintf(stderr, " -c <compression_level>\n");
	fprintf(stderr, " -I <includedir>\n");
	fprintf(stderr, " -D <macro>[=<def>]>\n");
	fprintf(stderr, " -i <library.swf>:<sym>[,<sym>]>\n");
	fprintf(stderr, " -h  Print this help screen\n");
	fprintf(stderr, " -V  Print version and copyright info\n");
	exit(ex);
}

static void 
warningHandler(const char *fmt, ...)
{
	va_list ap;

	va_start (ap, fmt);

	vfprintf(stderr, fmt, ap);
	putc('\n', stderr);

	va_end(ap);
}


char *cppargs;
size_t cppargsize = 256;

int
main (int argc, char **argv)
{
	char *outputfile="out.swf";
	int width=640, height=480;    /* default stage size */
	int i;
	int swfcompression = DEFSWFCOMPRESSION;
	int framerate = 12;
	int usedfiles = 0;
	struct stat statbuf;
#ifdef HAVE_GETOPT_LONG
	struct option opts[] =
	{
		{"dont-preprocess", 0, 0, 'p'},
		{"frame-rate", 1, 0, 'r'},
		{"swfversion", 1, 0, 'v'},
		{"compression", 1, 0, 'c'},
		{"includepath", 1, 0, 'I'},
		{"define", 1, 0, 'D'},
		{"size", 1, 0, 's'},
		{"output", 1, 0, 'o'},
		{"import", 1, 0, 'i'},
		{"version", 0, 0, 'V'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	int opts_idx;
#endif
	int c;
	char *me;

	cppargs = malloc(cppargsize);
	sprintf(cppargs, "%s", DEFAULT_FLAGS);
	//cppargs[0] = '\0';


	me = argv[0];

	while (1)
	{
#define BUFSIZE 1024
		char buf [BUFSIZE];

		const char *optstring = "Vhps:r:D:I:v:c:i:o:";
#ifdef HAVE_GETOPT_LONG
		c = getopt_long (argc, argv, optstring, opts, &opts_idx);
#else
		c = getopt (argc, argv, optstring);
#endif
		if (c == -1) break;

		switch (c)
		{
			case 'p':
				makeswf_set_dopreprocess(0);
				break;
			case 's':
				if ( sscanf(optarg, "%dx%d", &width, &height) != 2 )
				{
					usage(argv[0], EXIT_FAILURE);
				}
				break;
			case 'v':
				if ( sscanf(optarg, "%d", &swfversion) != 1 )
				{
					usage(argv[0], EXIT_FAILURE);
				}
				makeswf_set_swfversion(swfversion);
				break;
			case 'c':
				if ( sscanf(optarg, "%d", &swfcompression) != 1 )
				{
					usage(argv[0], EXIT_FAILURE);
				}
				if ( swfcompression < -1 || swfcompression > 9 )
				{
					fprintf(stderr, "Compression level must be in the range -1..9\n");
					exit(1);
				}
				break;
			case 'r':
				if ( sscanf(optarg, "%d", &framerate) != 1 )
				{
					usage(argv[0], EXIT_FAILURE);
				}
				break;
			case 'I':
				snprintf(buf, BUFSIZE-1, " -I%s", optarg);
				buf[BUFSIZE-1]='\0';
				makeswf_append_cpparg(buf);
				break;
			case 'i':
				add_import_spec(optarg);
				break;
			case 'o':
				outputfile = optarg;
				break;
			case 'D':
				// yes, you can smash the stack ... 
				snprintf(buf, BUFSIZE-1, " -D%s", optarg);
				buf[BUFSIZE-1]='\0';
				makeswf_append_cpparg(buf);
				break;
			case 'V':
				printf("%s\n", RCSID);
				printf("Copyright (C) 2001-2006 \"Sandro Santilli\" <strk@keybit.net>.\n");
				printf("Released under the GNU General Public License.\n");
				exit(EXIT_SUCCESS);
			case 'h':
				usage(argv[0], EXIT_SUCCESS);
			default:
				usage(argv[0], EXIT_FAILURE);
				break;
		}
	}
	argv+=optind;
	argc-=optind;

	if ( argc < 1 ) usage(me, EXIT_FAILURE);

	if ( stat(outputfile, &statbuf) )
	{
		// should warn about overriding (-f ?)
	}

	if ( Ming_init() )
	{
		fprintf(stderr, "Ming initialization error\n");
		exit(EXIT_FAILURE);
	}
	Ming_setWarnFunction(warningHandler);
	//Ming_setErrorFunction(compileError);
	Ming_useSWFVersion(swfversion);
	Ming_setSWFCompression(swfcompression);

	mo = newSWFMovie();
	SWFMovie_setDimension(mo, (float)width, (float)height);
	SWFMovie_setRate(mo, framerate);

	printf("Output file name: %s\n", outputfile);
	printf("Output compression level: %d\n", swfcompression);
	printf("Output SWF version: %d\n", swfversion);

   	/* 
	 * Add imports
	 */
	if ( numimport_specs ) add_imports();

	for ( i=0; i<argc; i++ )
	{
		SWFAction ac;
		char *filename = argv[i];
		char *ext = strrchr(filename, '.');
		char ppfile[PATH_MAX];
		SWFPrebuiltClip builtclip;

		if ( ext && ! strcasecmp(ext, ".swf") )
		{
			printf("Adding prebuilt clip %s to frame %d... ",
					filename, i);
			builtclip = newSWFPrebuiltClip_fromFile(filename);
			if ( ! builtclip ) exit (1);
			SWFMovie_add(mo, (SWFBlock)builtclip);
		}
		else if ( ext && ( ! strcasecmp(ext, ".png") || ! strcasecmp(ext, ".jpg") ) )
		{
			printf("Adding bitmap %s to frame %d... ",
					filename, i);
			embed_image(mo, filename);
		}
		else
		{
			sprintf(ppfile, "%s.frame%d.pp", outputfile, i);
			ac = makeswf_compile_source(filename, ppfile);
			printf("Adding %s to frame %d... ", filename, i);
			SWFMovie_add(mo, (SWFBlock)ac);

		}

		printf("done.\n"); 
		usedfiles++;
		SWFMovie_nextFrame(mo);

	}

	if ( ! usedfiles )
	{
		printf("No valid input files\n");
		return 1;
	}

	printf("Saving output to %s... ", outputfile);
	fflush(stdout);
	SWFMovie_save(mo, outputfile);
	printf("done.\n"); 

	return 0;
}


static void
add_import_spec(char *spec)
{
	if ( numimport_specs == 0 )
	{
		import_specs = (char **)malloc(1);
	}
	else
	{
		import_specs = (char **)realloc(import_specs,
				numimport_specs+1);
	}
	import_specs[numimport_specs] = spec;
	numimport_specs++;
}

static int
add_imports()
{
	int i;
	SWFMovieClip mc;
	SWFDisplayItem di;
	SWFAction ac;
	struct stat statbuf;

	mc = newSWFMovieClip();

	for (i=0; i<numimport_specs; i++)
	{
		char *spec = import_specs[i];
		char *file = strtok(spec, ":");
		char *sym;
		char unchecked = 0;

   		if ( -1 == stat(file, &statbuf) )
		{
			unchecked = 1;
		}

		printf("Importing symbols from ");
		if ( unchecked ) printf("[UNAVAILABLE] ");
		printf("%s:", file);
		fflush(stdout);
		while ((sym=strtok(NULL, ",")))
		{
			SWFCharacter ch;

			printf(" %s", sym);
			fflush(stdout);
			ch = SWFMovie_importCharacter(mo, file, sym);
			SWFMovieClip_add(mc, (SWFBlock)ch);
		}
		printf("\n");
	}

	SWFMovieClip_nextFrame(mc);
	di = SWFMovie_add(mo, (SWFBlock)mc);
	SWFDisplayItem_setName(di, "__shared_assets");
	ac = compileSWFActionCode("__shared_assets._visible=false;");
	SWFMovie_add(mo, (SWFBlock)ac);

	return 1;
}

static void
embed_image(SWFMovie movie, char *f)
{
	SWFFill fill;
	SWFBitmap bm;
	SWFShape shape;
	SWFMovieClip clip;
	SWFDisplayItem it, it2;
	FILE *raster;
	SWFInput in;
	int height, width;
	char *name, *ptr;
	static int depth=10;

        if (!(raster = fopen (f, "rb")))
        {
                fprintf (stdout, "%s: %s\n", f, strerror (errno));
		exit(1);
        }

        if (!(in = newSWFInput_file(raster)))
        {
                fprintf (stdout, "Can't create SWFInput from file\n");
		exit(1);
        }

        if (!(bm = newSWFBitmap_fromInput (in)))
        {
                fprintf (stdout, "Error creating bitmap");
		exit(1);
        }

	height = SWFBitmap_getHeight(bm);
	width = SWFBitmap_getWidth(bm);


	shape = newSWFShape();
  
	SWFShape_movePenTo(shape, 0, 0);

	fill = SWFShape_addBitmapFill(shape, bm, SWFFILL_CLIPPED_BITMAP);
	SWFShape_setRightFill(shape, fill);
	SWFShape_drawLineTo(shape, 0, width);
	SWFShape_drawLineTo(shape, height, width);
	SWFShape_drawLineTo(shape, height, 0);
	SWFShape_drawLineTo(shape, 0, 0);

	clip = newSWFMovieClip();
	it2 = SWFMovieClip_add(clip, (SWFBlock)shape);
	SWFMovieClip_nextFrame(clip);

	it = SWFMovie_add(mo, (SWFBlock)clip);

	// Use file basename (w/out extension)
	// as the name for the new character

	ptr = strrchr(f, '/');
	if ( ! ptr ) ptr = f;
	else if ( ! *++ptr ) ptr = f;
	name = strdup(ptr);
	ptr = strrchr(name, '.');
	if ( ptr ) *ptr = '\0';

	SWFDisplayItem_setName(it, name);
	SWFDisplayItem_setDepth(it, ++depth);

	printf("%s at depth %d\n", name, depth);

}

/**************************************************************
 *
 * $Log$
 * Revision 1.29  2006/12/11 20:01:44  strk
 * Allow png or jpg frame contents
 *
 * Revision 1.28  2006/11/22 23:24:26  strk
 * Experimental support for use of prebuilt clips.
 * Example: makeswf frame1.as frame2.swf frame3.as ...
 *
 * Revision 1.27  2006/11/15 16:34:48  strk
 * Fixed import feature to properly set __shared_assets sprite frame count
 *
 * Revision 1.26  2006/09/19 08:50:16  strk
 * Fixed handling of -D flag
 *
 * Revision 1.25  2006/07/10 16:11:26  strk
 * Changed makeswf_compile_source signature to accept path to preprocessor output file. Changed preprocessed file paths to (<output>.frame#.pp) to reduce probability of filesystem permission problems.
 *
 * Revision 1.24  2006/07/10 15:30:41  strk
 * Fixed --help and --version to not require an argument, added TODO item
 * for non-writeable source dir case.
 *
 * Revision 1.23  2006/07/08 13:47:18  strk
 * Split makeswf general functionalities in a separate file, for use by unit testers
 *
 * Revision 1.22  2006/06/20 22:16:13  strk
 * makeswf:
 *   - Added __SWF_VERSION__ macro definition for use in source files
 *
 * Revision 1.21  2006/05/06 10:38:37  strk
 * Fixed support for builds w/out zlib in SWF_output and listswf.
 * Added handler for Ming's warning in command line compiler
 *
 * Revision 1.20  2006/05/04 22:28:40  strk
 * fixed read past end of allocated memory in error handler
 *
 * Revision 1.19  2006/04/21 13:55:42  vapour
 * Added vasprintf() function from http://unixpapa.com/incnote/stdio.html, for those platforms missing it.
 *
 * Revision 1.18  2006/04/19 13:16:56  vapour
 * + Forced use of getopt on Windows.
 * + Changed to use memset rather than bzero.  bzero not on MinGW.
 *
 * Revision 1.17  2006/04/19 11:38:01  vapour
 * Added liberal use of ifdefs to avoid vasprintf on the platforms that don't have it.  I make no guarantees of the functional integrity on them now. ;(
 *
 * Revision 1.16  2005/03/28 08:08:52  strk
 * Added frame layout printing.
 *
 * Revision 1.15  2005/03/22 13:41:30  strk
 * Changed calling interface:
 * 	- all arguments are source files
 * 	- each source file will be stored in a separate
 * 	  frame, in the order they appear
 * 	- output will be "out.swf" unless -o <output> is used
 *
 * Revision 1.14  2004/11/10 14:00:46  strk
 * Added support for specifying output compression level
 *
 * Revision 1.13  2004/11/09 12:48:12  strk
 * Added -v flag in usage string (already supported but not documented)
 *
 * Revision 1.12  2004/11/03 07:52:08  strk
 * Introduced a default include path to easy code sharing.
 *
 * Revision 1.11  2004/11/02 17:53:05  strk
 * Fixed a bug in -I and -D handling.
 *
 * Revision 1.10  2004/09/29 10:07:29  strk
 * ImportAssets executed even if import file is not found on filesystem.
 * A warning is issued in that case.
 *
 * Revision 1.9  2004/09/28 14:39:50  strk
 * Forced imported assets inclusion by mean of instantiation.
 * Symbols are instantiated inside a __shared_assets clip, which in turn
 * is made invisible.
 *
 * Revision 1.8  2004/09/28 14:09:07  strk
 * Added assets import support.
 *
 * Revision 1.7  2004/09/28 06:59:29  strk
 * Added -v switch to set output version.
 * Added notice about output configuration.
 *
 * Revision 1.6  2004/09/25 08:23:05  strk
 * Added -C to cpp call
 *
 * Revision 1.5  2004/09/25 08:17:11  strk
 * Post-processed files are kept and their name is shown to the user to
 * allow for error Line finding.
 *
 * Revision 1.4  2004/07/19 08:22:56  strk
 * GNU_SOURCE define in makeswf.c, makeswf.h dependency in Makefile
 *
 * Revision 1.3  2004/07/15 12:45:54  strk
 * Added -D and -I switched for preprocessor control
 *
 *
 **************************************************************/
