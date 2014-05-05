/****************************************************************************
 *
 *  makeswf - a command line actionscript compiler
 *
 *  Copyright (C) 2003-2009 "Sandro Santilli" <strk@keybit.net>
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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
 *  If you need another kind of preprocessing change the CPP define in
 *  makeswf_utils.c.
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
#include <strings.h>
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
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

// Cheating, but it works (not sure why the above ifdef for getopt isn't)
#ifdef _WIN32
#include <getopt.h>
#endif

#define DEFSWFVERSION 6
#define DEFSWFCOMPRESSION 9

#define MAXERRORMSG 1024

/* prototypes */
static void add_import_spec(char *spec);
static void add_init_action(char *file, int frameno);
static void add_init_action_spec(char *spec);
static void compile_init_actions(int frameno, int debug);
static int add_imports(void);
static void embed_image(SWFMovie movie, char *f);
static void embed_swf(SWFMovie movie, char *f);
static void print_init_actions(int frameno, FILE* stream);
// return pointer to allocated memory (free it)
static char* base_name(char* filename);
/* data */
static char **import_specs;
static int numimport_specs = 0;
static int swfversion = DEFSWFVERSION;
static const char *RCSID = "$Id$";
static SWFMovie mo;
static int useBgColor=0;
static long int bgcolor=0;
char *outputfile="out.swf";

typedef struct {
	char* file;
	int frameno;
} init_action;
static init_action* init_actions = NULL;
static int num_init_actions = 0;
static int max_init_action_frame = 0;

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
	fprintf(stderr, " -b <background_color>\n");
	fprintf(stderr, " -I <includedir>\n");
	fprintf(stderr, " -D <macro>[=<def>]>\n");
	fprintf(stderr, " -i <library.swf>:<sym>[,<sym>]>\n");
	fprintf(stderr, " -h  Print this help screen\n");
	fprintf(stderr, " -V  Print version and copyright info\n");
	fprintf(stderr, " -d  debug parser\n");
	fprintf(stderr, " -a  <AS_file>[:<frameno>] - add init action for frame <frameno> (0-based, 0 if omitted)\n");
	fprintf(stderr, " -n  network|file - restrict sandbox access from locally-loaded movies (automatically restricted to file for SWF>=8)\n");
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
typedef enum {

	// A prebuilt clip
	SWF,

	// Any bitmap that can be feed to embed_image
	BITMAP,

	// ActionScript code
	AS
} FileType;

FileType getFileType(char* filename)
{
	// TODO: use magic number

	char *ext = strrchr(filename, '.');
	if ( ! ext ) return AS;

	if ( ! strcasecmp(ext, ".swf") ) return SWF;
	if ( ! strcasecmp(ext, ".png") ) return BITMAP;
	if ( ! strcasecmp(ext, ".dbl") ) return BITMAP;
	if ( ! strcasecmp(ext, ".jpg") ) return BITMAP;
	if ( ! strcasecmp(ext, ".jpeg") ) return BITMAP;

	return AS;
}

int
main (int argc, char **argv)
{
	int width=640, height=480;    /* default stage size */
	int i;
	int swfcompression = DEFSWFCOMPRESSION;
	float framerate = 12.0;
	/*
	 * Allow network access from locally-loaded movies.
	 *
	 *   0 = file access
	 *   1 = network access
	 *  -1 = unspecified (omit the tag if SWF < 8, file access otherwise)
	 *
	 *  By default is unspecified.
	 */
	int networkAccess = -1;
	int usedfiles = 0;
	struct stat statbuf;
	int debug_parser = 0;	
#ifdef HAVE_GETOPT_LONG
	struct option opts[] =
	{
		{"dont-preprocess", 0, 0, 'p'},
		{"frame-rate", 1, 0, 'r'},
		{"swfversion", 1, 0, 'v'},
		{"bgcolor", 1, 0, 'b'},
		{"compression", 1, 0, 'c'},
		{"includepath", 1, 0, 'I'},
		{"define", 1, 0, 'D'},
		{"size", 1, 0, 's'},
		{"network-access", 1, 0, 'n'},
		{"output", 1, 0, 'o'},
		{"import", 1, 0, 'i'},
		{"version", 0, 0, 'V'},
		{"help", 0, 0, 'h'},
		{"debug", 0, 0, 'd'},
		{"init-action", 1, 0, 'a'},
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

		const char *optstring = "Vhpds:r:D:I:v:c:b:i:o:a:n:";
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
			case 'n':
			{
				if ( ! strcasecmp(optarg, "network") )
				{
					networkAccess = 1;
				}
				else if ( ! strcasecmp(optarg, "file") )
				{
					networkAccess = 0;
				}
				else
				{
					fprintf(stderr, "Network access must be 'network' or 'file'\n");
					exit(1);
				}
				break;
			}
			case 'v':
				if ( sscanf(optarg, "%d", &swfversion) != 1 )
				{
					usage(argv[0], EXIT_FAILURE);
				}
				makeswf_set_swfversion(swfversion);
				break;
			case 'b':
				if ( sscanf(optarg, "%lx", &bgcolor) != 1 )
				{
					usage(argv[0], EXIT_FAILURE);
				}
				useBgColor=1;
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
				if ( sscanf(optarg, "%f", &framerate) != 1 )
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
			case 'd':
				debug_parser = 1;
				break;
			case 'a':
				add_init_action_spec(optarg);
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
	if ( networkAccess >= 0 ) SWFMovie_setNetworkAccess(mo, networkAccess);
	SWFMovie_setDimension(mo, (float)width, (float)height);
	SWFMovie_setRate(mo, framerate);

	if ( useBgColor )
	{
		SWFMovie_setBackground(mo,
			bgcolor >> 16,
			(bgcolor&0x00FF00) >> 8,
			(bgcolor&0x0000FF));
	}

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
		char ppfile[PATH_MAX];

		FileType type = getFileType(filename);

		compile_init_actions(i, debug_parser);

		if ( type == SWF )
		{
			printf("Adding prebuilt clip %s to frame %d... ",
					filename, i);
			embed_swf(mo, filename);
		}
		else if ( type == BITMAP )
		{
			printf("Adding bitmap %s to frame %d... ",
					filename, i);
			embed_image(mo, filename);
		}
		else
		{
			sprintf(ppfile, "%s.frame%d.pp", outputfile, i);
			ac = makeswf_compile_source(filename, ppfile, 
				debug_parser);
			printf("Adding %s to frame %d... ", filename, i);
			SWFMovie_add(mo, (SWFBlock)ac);
		}

		printf("done.\n"); 
		usedfiles++;
		SWFMovie_nextFrame(mo);

	}

	if ( i <= max_init_action_frame )
	{
		fprintf(stderr, "WARNING: following init actions for frames > %d have been discarded:\n", i-1);
		for (;i<=max_init_action_frame; ++i)
		{
			print_init_actions(i, stderr);
		}
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
		import_specs = (char **)malloc(sizeof(char*));
	}
	else
	{
		import_specs = (char **)realloc(import_specs,
				(numimport_specs+1)*sizeof(char*));
	}
	import_specs[numimport_specs] = spec;
	numimport_specs++;
}

static void
add_init_action(char* file, int frameno)
{
	printf("Init action for frame %d in file %s\n", frameno, file);

	++num_init_actions;
	init_actions = (init_action*)realloc(init_actions, num_init_actions*sizeof(init_action));
	init_action* ac = &(init_actions[num_init_actions-1]);

	ac->file = file;
	ac->frameno = frameno;
	
	if ( frameno > max_init_action_frame ) max_init_action_frame = frameno;
}

static void
print_init_actions(int frameno, FILE* stream)
{
	int i;

	for (i=0; i<num_init_actions; ++i)
	{
		init_action* ia = &(init_actions[i]);
		if ( ia->frameno != frameno ) continue;
		fprintf(stream, " %s:%d\n", ia->file, ia->frameno);
	}

}

static void
compile_init_actions(int frameno, int debug)
{
	int i;
	int found=0;
	SWFAction action;
	char ppfile[PATH_MAX];

	for (i=0; i<num_init_actions; ++i)
	{
		init_action* ia = &(init_actions[i]);
		char* file;

		if ( ia->frameno != frameno ) continue;
		file = ia->file;

		sprintf(ppfile, "%s.frame%d.init%d.pp", outputfile, frameno, found);
		action = makeswf_compile_source(file, ppfile, debug);


		printf("Adding %s to frame %d init actions... ",
					file, frameno);
	 	SWFMovie_add(mo, newSWFInitAction(action));	
		printf("done.\n"); 

		++found;
	}

}

static void
add_init_action_spec(char *spec)
{
	struct stat statbuf;
	int frameno = 0;
	char *file = strtok(spec, ":");
	char *framespec = strtok(NULL, ",");

	if ( framespec ) frameno = atoi(framespec);

	if ( -1 == stat(file, &statbuf) )
	{
		perror(file); 
		exit(EXIT_FAILURE);
	}

	if ( ! S_ISREG(statbuf.st_mode) )
	{
		fprintf(stderr, "ERROR: %s: is not a regular file\n", file);
		exit(EXIT_FAILURE);
	}

	/* check valid frame spec here */
	if ( frameno < 0 )
	{
		fprintf(stderr, "ERROR: invalid frame number %d for init actions\n", frameno);
		exit(EXIT_FAILURE);
	}

	add_init_action(file, frameno);
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
	ac = newSWFAction("__shared_assets._visible=false;");
	SWFMovie_add(mo, (SWFBlock)ac);

	return 1;
}

static char*
base_name(char* filename)
{
	char *name, *ptr;

	ptr = strrchr(filename, '/');

	if ( ! ptr ) ptr = filename;
	else if ( ! *++ptr ) ptr = filename;
	name = strdup(ptr);
	ptr = strrchr(name, '.');
	if ( ptr ) *ptr = '\0';
	return name;
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
	char *name;

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
	SWFShape_drawLineTo(shape, width, 0);
	SWFShape_drawLineTo(shape, width, height);
	SWFShape_drawLineTo(shape, 0, height);
	SWFShape_drawLineTo(shape, 0, 0);

	clip = newSWFMovieClip();
	it2 = SWFMovieClip_add(clip, (SWFBlock)shape);
	SWFMovieClip_nextFrame(clip);

	it = SWFMovie_add(mo, (SWFBlock)clip);

	name = base_name(f);

	SWFDisplayItem_setName(it, name);

	free(name);

}

static void
embed_swf(SWFMovie movie, char* filename)
{
	SWFPrebuiltClip builtclip;
	SWFDisplayItem it;
	char *name;
	SWFInput in;
	FILE *swf;

        if (!(swf = fopen (filename, "rb")))
        {
                fprintf (stdout, "%s: %s\n", filename, strerror (errno));
		exit(1);
        }

        if (!(in = newSWFInput_file(swf)))
        {
                fprintf (stdout, "Can't create SWFInput from file\n");
		exit(1);
        }

	builtclip = newSWFPrebuiltClip_fromInput(in);
	if ( ! builtclip ) {
                fprintf (stdout, "Error creating prebuilt clip\n");
		exit(1);
	}

	it = SWFMovie_add(mo, (SWFBlock)builtclip);

	name = base_name(filename);
	SWFDisplayItem_setName(it, name);
	free(name);
}

/**************************************************************
 *
 * $Log$
 * Revision 1.50  2009/09/08 22:26:16  strk
 * Update copyright notice (and FSF address)
 *
 * Revision 1.49  2009/03/15 21:38:10  strk
 * Allow using .dbl files as frame content
 *
 * Revision 1.48  2009/03/14 09:45:10  strk
 * Add -b --bgcolor switch to makeswf
 *
 * Revision 1.47  2008/06/26 19:36:12  krechert
 * fix linker error and make enabling SWFAction's debug mode generic
 *
 * Revision 1.46  2008/02/08 09:19:57  strk
 * Add -n switch to force NetworkAccess flag in a file attribute tag.
 * See http://bugs.libming.org/show_bug.cgi?id=37
 *
 * Revision 1.45  2008/01/15 08:11:24  strk
 * Fix bitmap embedding code to respect size of image (rather then swap dimensions)
 *
 * Revision 1.44  2007/10/30 12:17:08  krechert
 * set framerate as float
 *
 * Revision 1.43  2007/10/25 17:26:12  krechert
 * use new InitAction api
 *
 * Revision 1.42  2007/10/24 08:38:47  strk
 * Use a separate character definition for each init action block
 *
 * Revision 1.41  2007/10/24 08:30:43  strk
 * Add support for multiple init actions in same and/or different frames.
 * Still unsupported adding init actions for frames for which we specify no content.
 *
 * Revision 1.40  2007/10/24 07:49:55  strk
 * Exit with an error if multiple init actions are attempted to be added (still unsupported)
 *
 * Revision 1.39  2007/10/24 07:46:30  strk
 * Change -C flag to -a (--init-action) and support specifying a target frame number.
 * Only frame0 is currently supported.
 *
 * Revision 1.38  2007/10/18 09:17:59  krechert
 * add class definition support.
 * Class definitions are initialized by InitAction tags. These tags need a dummy movieclip they depend on.
 *
 * Revision 1.37  2007/10/15 12:37:28  krechert
 * add bison debugging support in makeswf
 *
 * Revision 1.36  2007/07/06 17:53:14  krechert
 * fixed declaration after statement (C90)
 * makes ming compile on MSVC again
 *
 * Revision 1.35  2007/04/30 09:58:32  strk
 * Don't include getopt.h if it's not found. Patch by Nils Goroll <nils.goroll@mcs.de>.
 *
 * Revision 1.34  2007/04/14 16:58:46  strk
 * Removed unused CPP define from makeswf.c, fix the *used* one in makeswf_utils.
 * Thanks to Bastiaan Jacques for finding this out.
 *
 * Revision 1.33  2007/04/01 10:01:16  strk
 * Add -xc++ in CPP define  - patch by Deanna Phillips.
 *
 * Revision 1.32  2007/03/22 10:59:17  strk
 * Updated to use newSWFAction() instead of compileSWFActionCode()
 *
 * Revision 1.31  2006/12/12 23:36:10  strk
 * Implement a separate function for detecting the type of frame content.
 * Take files anding in .jpeg as bitmaps.
 * (to be improved to use magic number)
 *
 * Revision 1.30  2006/12/11 22:01:29  strk
 * Use a name for prebuilt clips as well. I can't belive how powerful this tool is getting ;)
 *
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
