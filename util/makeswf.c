/*
 * makeswf - a command line actionscript compiler
 *
 * Copyleft (g) 2003 strk@keybit.net
 *
 * Intro
 * -----
 *
 * This is a simple Flash actionscript compiler that
 * uses libming to do its job. You'll find it useful
 * only when coding in pure actionscript, thus probably
 * only when using the Flash 6 drawing API.
 *
 * Output version is set to '6' and compression level to '9'.
 * To change this, you'll have to update this file; it's very
 * simple, don't be afraid ;)
 *
 * Oh... input files are preprocessed unless you provide
 * the -p flag (do not use preprocessor). -D can be used to set macros
 * and -I to add dirs to include search paths.
 *
 * If you need another kind of preprocessing change the CPP define on
 * top of this file.
 *
 * Build
 * -----
 *
 * This binary will link dinamically to libming, so you should
 * install ming somewhere, or let the linker know where to find
 * the ming library.
 *
 * TODO
 * ----
 * 
 * - Change command line to:
 *	makeswf [-o <outputfile>] [-p] <input> ...
 * 
 * - Accept output version as parameter.
 * - Accept compression level as parameter.
 * - Accept -v for versioning and credits.
 *
 */

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
#ifdef HAVE_GETOPT_H
#include <getopt.h>
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
	fprintf(stderr, "Usage: %s [OPTIONS] <output> <as> ...\n", me);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, " -s <width>x<height>\n");
	fprintf(stderr, " -r <frame_rate>\n");
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
	char *outputfile;
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
		{"includepath", 1, 0, 'I'},
		{"define", 1, 0, 'D'},
		{"size", 1, 0, 's'},
		{"import", 1, 0, 'i'},
		{0, 0, 0, 0}
	};
	int opts_idx;
#endif
	int c;
	char cppargs[256];
	char *me;

	cppargs[0] = '\0';


	me = argv[0];

	while (1)
	{
		char buf [256];

#ifdef HAVE_GETOPT_LONG
		c = getopt_long (argc, argv, "ps:r:D:I:v:i:", opts, &opts_idx);
#else
		c = getopt (argc, argv, "ps:r:D:I:v:i:");
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
			case 'r':
				if ( sscanf(optarg, "%d", &framerate) != 1 )
				{
					usage(argv[0], 1);
				}
				break;
			case 'I':
				// yes, you can smash the stack ... 
				sprintf(buf, "-I%s", optarg);
				strcat(cppargs, buf);
				break;
			case 'i':
				add_import_spec(optarg);
				break;
			case 'D':
				// yes, you can smash the stack ... 
				sprintf(buf, "-D%s", optarg);
				strcat(cppargs, buf);
				break;
			default:
				usage(argv[0], 1);
				break;
		}
	}
	argv+=optind;
	argc-=optind;

	if ( argc < 2 ) usage(me, 1);

	outputfile = argv[0];
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

	for ( i=1; i<argc; i++ )
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

		printf("Compiling code from %s... ", filename);
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
   bzero(ret, size+1);
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
