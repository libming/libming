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
 * Oh... input files are preprocessed using "cpp -P" unless you provide
 * the -p flag (do not use preprocessor).
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
 * - Try to keep lines number after preprocessing
 *   ( keep comments ? / what about includes ? / keep preprocessed files ? )
 *   ( postprocess error messages substituting Line no ? )
 *
 * - Change command line to:
 *	makeswf [-o <outputfile>] [-p] <input> ...
 * 
 * - Allow for preprocesed files keeping
 * - Accept output version as parameter.
 * - Accept compression level as parameter.
 * - Accept -v for versioning and credits.
 *
 */

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

#define CPP "cpp -P"
#define MAXERRORMSG 1024

/* prototypes */
static char * readfile (char *file);
static int preprocess (char *file, char *out);
static void compileError(const char *fmt, ...);

/* data */
static char lastcompilemessage[MAXERRORMSG];
static int lastcompilefailed = 0;

void
usage (char *me, int ex)
{
   fprintf(stderr, "Usage: %s [-p] [-s <width>x<height>] [-r <framerate>] <output> <as> ...\n", me);
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
   SWFMovie mo;
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
#ifdef HAVE_GETOPT_LONG
   struct option opts[] =
   {
    {"dont-preprocess", 0, 0, 'p'},
    {"frame-rate", 1, 0, 'r'},
    {"size", 1, 0, 's'},
    {0, 0, 0, 0}
   };
   int opts_idx;
#endif
   int c;

   if ( argc < 3 ) usage(argv[0], 1);

   while (1)
   {
#ifdef HAVE_GETOPT_LONG
      c = getopt_long (argc, argv, "ps:r:", opts, &opts_idx);
#else
      c = getopt (argc, argv, "ps:r:");
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
         case 'r':
            if ( sscanf(optarg, "%d", &framerate) != 1 )
            {
               usage(argv[0], 1);
            }
            break;
         default:
            usage(argv[0], 1);
            break;
      }
   }
   argv+=optind;
   argc-=optind;

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

   for ( i=1; i<argc; i++ )
   {

      if ( dopreprocess )
      {
         sprintf(ppfile, "%s.cpp", argv[i]);
         if ( ! preprocess(argv[i], ppfile) ) continue;
         if ( ! (code=readfile(ppfile)) ) continue;
         unlink(ppfile);
      }
      else
      {
         if ( ! (code=readfile(argv[i])) ) continue;
      }




      printf("Compiling code from %s... ", argv[i]);
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
preprocess (char *file, char *out)
{
   char buf[1024];

   sprintf(buf, "%s %s > %s", CPP, file, out);
   system(buf);

   return 1;
}

/*************************************************************8
 *
 * $Log$
 * Revision 1.2  2004/04/29 08:06:48  strk
 * moved copyright notice on top
 *
 * Revision 1.1  2004/04/26 09:12:13  strk
 * Added simple commandline actionscript compiler.
 *
 * Revision 1.7  2004/04/01 09:00:16  strk
 * Fixed typo in -r handling
 *
 * Revision 1.6  2004/03/31 22:08:21  strk
 * Added framerate switch
 *
 * Revision 1.5  2004/03/31 22:02:30  strk
 * SWFMovie_save() call corrected to latest API
 * 
 * Revision 1.4  2003/09/29 10:01:53  strk
 * 
 * making a mess ;)
 * 
 * Revision 1.3  2003/09/29 10:00:01  strk
 * 
 * Added a new introductory comment before the log list
 * (also found that when entering log with cvs it's better not include
 *  a newline after CVS: lines)
 * 
 * Revision 1.2  2003/09/29 09:58:09  strk
 * 
 * Added |Log| in a comment at top file to see if it will work
 * (modified)
 */
