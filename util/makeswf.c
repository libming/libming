/****************************************************************************
 *
 *  makeswf - a command line actionscript compiler
 *
 *  Copyright (C) 2003 2004 2005 "Sandro Santilli" <strk@keybit.net>
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
 *  Input files are preprocessed unless you provide
 *  the -p flag (do not use preprocessor). -D can be used to set macros
 *  and -I to add dirs to include search paths.
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
 *  - Accept -V for versioning and credits.
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
static char * readfile (char *file);
static int preprocess (char *file, char *out, char *cppargs);
static void compileError(const char *fmt, ...);
static void add_import_spec(char *spec);
static int add_imports(void);

/* data */
static char lastcompilemessage[MAXERRORMSG];
static int lastcompilefailed = 0;
static char **import_specs;
static int numimport_specs = 0;
SWFMovie mo;

void
usage (char *me, int ex)
{
	fprintf(stderr, "Usage: %s [OPTIONS] <frame.as> ...\n",
		me);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, " -o <output>\n");
	fprintf(stderr, " -s <width>x<height>\n");
	fprintf(stderr, " -r <frame_rate>\n");
	fprintf(stderr, " -v <output_version>\n");
	fprintf(stderr, " -c <compression_level>\n");
	fprintf(stderr, " -I <includedir>\n");
	fprintf(stderr, " -D <macro>[=<def>]>\n");
	fprintf(stderr, " -i <library.swf>:<sym>[,<sym>]>\n");
	exit(ex);
}

/*
 * Should translate preprocessed file's line number to
 * original file's line number (seems not so easy)
 */
static void
printCompileMessage()
{
   char *ptr1;

   printf("  %s\n", strtok(lastcompilemessage, "\n"));
   while  ( (ptr1=strtok(NULL, "\n")) )
   {
      printf("  %s\n", ptr1);
   }
}

/* 
 * This is here to handle line number reporting
 * due to preprocessor scrambling of it
 */
static void 
compileError(const char *fmt, ...)
{
   char *msg;
   va_list ap;

   va_start (ap, fmt);

   /*
    * This is a GNU extension.
    * Dunno how to handle errors here.
    */
   if ( ! vasprintf(&msg, fmt, ap) )
   {
      fprintf(stderr, "vasnprintf allocated 0 bytes\n");
      va_end(ap);
      return;
   }
   va_end(ap);

   memcpy(lastcompilemessage, msg, MAXERRORMSG-1);
   free(msg);
   lastcompilemessage[MAXERRORMSG-1] = '\0';
   lastcompilefailed = 1;

   return;
}


int
main (int argc, char **argv)
{
	SWFAction ac;
	char *code;
	char *outputfile="out.swf";
	char ppfile[PATH_MAX];        /* preprocessed file */
	struct stat statbuf;
	int width=640, height=480;    /* default stage size */
	int i;
	int swfversion = DEFSWFVERSION;
	int swfcompression = DEFSWFCOMPRESSION;
	int dopreprocess = 1; /* use preprocessor by default */
	int framerate = 12;
	int compiledfiles = 0;
#ifdef HAVE_GETOPT_LONG
	struct option opts[] =
	{
		{"dont-preprocess", 0, 0, 'p'},
		{"frame-rate", 1, 0, 'r'},
		{"version", 1, 0, 'v'},
		{"compression", 1, 0, 'c'},
		{"includepath", 1, 0, 'I'},
		{"define", 1, 0, 'D'},
		{"size", 1, 0, 's'},
		{"output", 1, 0, 'o'},
		{"import", 1, 0, 'i'},
		{0, 0, 0, 0}
	};
	int opts_idx;
#endif
	int c;
	char *cppargs;
	size_t cppargsize = 256;
	char *me;

	cppargs = malloc(cppargsize);
	sprintf(cppargs, "%s", DEFAULT_FLAGS);
	//cppargs[0] = '\0';


	me = argv[0];

	while (1)
	{
		char buf [1024];

#ifdef HAVE_GETOPT_LONG
		c = getopt_long (argc, argv, "ps:r:D:I:v:c:i:o:", opts, &opts_idx);
#else
		c = getopt (argc, argv, "ps:r:D:I:v:i:c:o:");
#endif
		if (c == -1) break;

		switch (c)
		{
			case 'p':
				dopreprocess=0;
				break;
			case 's':
				if ( sscanf(optarg, "%dx%d", &width, &height) != 2 )
				{
					usage(argv[0], 1);
				}
				break;
			case 'v':
				if ( sscanf(optarg, "%d", &swfversion) != 1 )
				{
					usage(argv[0], 1);
				}
				break;
			case 'c':
				if ( sscanf(optarg, "%d", &swfcompression) != 1 )
				{
					usage(argv[0], 1);
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
					usage(argv[0], 1);
				}
				break;
			case 'I':
				// yes, you can smash the stack ... 
				sprintf(buf, " -I%s", optarg);
				if (strlen(cppargs)+strlen(buf) > cppargsize)
				{
					cppargsize *= 2;
					cppargs = realloc(cppargs, cppargsize);
				}
				strcat(cppargs, buf);
				break;
			case 'i':
				add_import_spec(optarg);
				break;
			case 'o':
				outputfile = optarg;
				break;
			case 'D':
				// yes, you can smash the stack ... 
				sprintf(buf, " -D%s", optarg);
				if (strlen(cppargs)+strlen(buf) > cppargsize)
				{
					cppargsize *= 2;
					cppargs = realloc(cppargs, cppargsize);
				}
				strcat(cppargs, buf);
				break;
			default:
				usage(argv[0], 1);
				break;
		}
	}
	argv+=optind;
	argc-=optind;

	if ( argc < 1 ) usage(me, 1);

	if ( ! stat(outputfile, &statbuf) )
	{
	}

	if ( Ming_init() )
	{
		fprintf(stderr, "Ming initialization error\n");
		exit(1);
	}
	Ming_setWarnFunction(compileError);
	Ming_setErrorFunction(compileError);
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
		struct stat statbuf;
		char *filename = argv[i];

		if ( -1 == stat(argv[i], &statbuf) )
		{
			fprintf(stderr, "Skipping source '%s': %s\n",
				argv[i], strerror(errno));
			continue;
		}

		if ( dopreprocess )
		{
			printf("Preprocessing %s... ", argv[i]);
			fflush(stdout);
			sprintf(ppfile, "%s.pp", argv[i]);
			if ( ! preprocess(argv[i], ppfile, cppargs) ) continue;
			//unlink(ppfile);
			filename = ppfile;
			printf("done.\n");
		}
		if ( ! (code=readfile(filename)) )
		{
			continue;
		}

		printf("Compiling `%s' into frame %d... ",
			filename, compiledfiles+1);
		fflush(stdout);
		lastcompilefailed=0;
		ac = compileSWFActionCode(code);
		free(code);
		if ( lastcompilefailed )
		{
			printf("failed:\n"); 
			printCompileMessage();
			exit(1);
		}
		else
		{
			printf("done.\n"); 
		}
		SWFMovie_add(mo, (SWFBlock)ac);
		compiledfiles++;
		SWFMovie_nextFrame(mo);

	}

	if ( ! compiledfiles )
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

static char *
readfile (char *file)
{
   FILE *fd;
   struct stat buf;
   int size;
   char *ret;

   fd = fopen(file, "r");
   if ( ! fd )
   {
      perror(file);
      return NULL;
   }
   fstat(fileno(fd), &buf);
   size = buf.st_size;
   ret = (char *) malloc( size+1 );
   if ( ! ret ) 
   {
      perror("readfile");
      return NULL;
   }
   memset(ret, '\0', size+1);
   fread(ret, 1, size, fd);
   fclose(fd);

   return ret;
}

static int
preprocess (char *file, char *out, char *cppargs)
{
	char buf[1024];
	int ret;

	sprintf(buf, "%s %s %s > %s", CPP, cppargs, file, out);
	//printf("%s\n", buf);

	ret = system(buf);
	if ( ret ) return 0;

	return 1;
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

	di = SWFMovie_add(mo, (SWFBlock)mc);
	SWFDisplayItem_setName(di, "__shared_assets");
	ac = compileSWFActionCode("__shared_assets._visible=false;");
	SWFMovie_add(mo, (SWFBlock)ac);

	return 1;
}

/*************************************************************8
 *
 * $Log$
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
 */
