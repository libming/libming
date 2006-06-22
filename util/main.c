#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

//open()
#include <fcntl.h>

//fstat()
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ming_config.h"

//decompression
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#include "blocks/blocktypes.h"
#include "action.h"
#include "read.h"
#include "parser.h"
#include "outputdecl.h"

SWF_Parserstruct *blockParse (FILE *f, int length, SWFBlocktype header);

char *filename;
char tmp_name[PATH_MAX];
FILE *tempfile;
char *swftargetfile=NULL;

int verbose = 0;

void silentSkipBytes(FILE *f, int length)
{
  for(; length>0; --length)
    readUInt8(f);
}

void readRect(FILE *f, struct Rect *s)
{
  int nBits;

  byteAlign();

  nBits = readBits(f, 5);
  s->xMin = readSBits(f, nBits);
  s->xMax = readSBits(f, nBits);
  s->yMin = readSBits(f, nBits);
  s->yMax = readSBits(f, nBits);
}

struct Movie m;

/*
 * Compressed swf-files have a 8 Byte uncompressed header and a
 * zlib-compressed body. 
 */
int
cws2fws(FILE *f, uLong outsize)
{

	struct stat statbuffer;
	int insize;
	int err,tmp_fd;
	Byte *inbuffer,*outbuffer;

	sprintf(tmp_name, "/tmp/swftoscriptXXXXXX");

#ifdef HAVE_MKSTEMP
	tmp_fd = mkstemp(tmp_name);
#endif
#ifndef HAVE_MKSTEMP
	tmp_fd = open(tmp_name, O_RDWR | O_CREAT | O_TRUNC , 0600);
#endif

	if ( tmp_fd == -1 )
	{
		error("Couldn't create tempfile.\n");
	}

	tempfile = fdopen(tmp_fd, "w+");
	if ( ! tempfile )
	{
		error("fdopen: %s", strerror(errno));
	}


	if( stat(filename, &statbuffer) == -1 )
	{
		error("stat() failed on input file");
	}
	
	insize = statbuffer.st_size-8;
	inbuffer = malloc(insize);
	if(!inbuffer){ error("malloc() failed"); }
	if ( ! fread(inbuffer, insize, 1, f) )
	{
		error("Error reading input file");
	}
	
	/* We don't trust the value in the swfheader. */
	outbuffer=NULL;
	do{
		outbuffer = realloc(outbuffer, outsize);	
		if (!outbuffer) { error("malloc(%lu) failed",outsize); }

#ifdef USE_ZLIB
		/* uncompress the data */
		err=uncompress(outbuffer,&outsize,inbuffer,insize);
#else // ndef USE_ZLIB
		/* No zlib, so we can't uncompress the data */
		error("No zlib support compiled in, "
			"cannot read compressed SWF");
#endif
		switch(err){
			case Z_MEM_ERROR:
				error("Not enough memory.\n");
				break;
			case Z_BUF_ERROR:
				fprintf(stderr,"resizing outbuffer..\n");
				outsize*=2;
				continue;
			case Z_DATA_ERROR:
				error("Data corrupted. Couldn't uncompress.\n");
				break;
			case Z_OK:
				break;
			default:
				error("Unknown returnvalue of uncompress:%i\n",
					err);
				break;
		}
	} while(err == Z_BUF_ERROR);
 
	/* Rebuild the header so the file offsets will be right */
	fputc('F',tempfile);
	fputc('W',tempfile);
	fputc('S',tempfile);
	fputc(m.version,tempfile);
	fwrite(&m.size,sizeof(int),1,tempfile);

	if ( outsize != fwrite(outbuffer, 1, outsize, tempfile) )
	{
		error("Error writing uncompressed");
	}

	rewind(tempfile);
	return (int)outsize;
}

static void usage(char *prog)
{
	fprintf(stderr,"%s: [-v] inputfile [swftargetfile]\n", prog);
}

int
main (int argc, char *argv[])
{
  SWF_Parserstruct *blockp;
  FILE *f;
  char first;
  int block, type, blockstart, blockoffset, length, nextFrame=0;
  int compressed = 0;

  setbuf(stdout,NULL);
  switch( argc ) {
	  case 2:
		filename = argv[1];
		break;
	  case 3:
  		if (strcmp (argv[1], "-v") == 0) {
			verbose = 1;
			filename = argv[2];
		} else {
			filename = argv[1];
			swftargetfile = argv[2];
		}
		break;
	  case 4:
  		if (strcmp (argv[1], "-v") != 0) {
			usage(argv[0]);
			exit(1);
		}
		verbose = 1;
		filename = argv[2];
		swftargetfile = argv[3];
		break;
	  case 0:
	  case 1:
	 default:
		usage(argv[0]);
		exit(1);
  }
  if (!(f = fopen (filename, "rb")))
    {
      fprintf(stderr,"Sorry, can't seem to read the file '%s'\n",filename);
      usage(argv[0]);
      exit(1);
    }

  first = readUInt8 (f);
  compressed = (first == ('C')) ? 1 : 0;
  if (!
      ((first == 'C' || first == 'F') && readUInt8 (f) == 'W'
       && readUInt8 (f) == 'S'))
    {
      error ("Doesn't look like a swf file to me..\n");
    }

	m.version = readUInt8 (f);
	m.size = readUInt32 (f);

	if (compressed)
	{
		int unzipped = cws2fws (f, m.size);
		if (m.size != (unzipped + 8))
		{
			warning ("m.size: %i != %i+8  Maybe wrong value in swfheader.\n", m.size, unzipped + 8);
		}
		fclose (f);
		f = tempfile;
	        fseek(f,8,SEEK_SET);
	}

	readRect (f, &(m.frame));

	m.rate = readUInt8 (f) / 256.0 + readUInt8 (f);
	m.nFrames = readUInt16 (f);

	outputHeader(&m);

  for (;;)
    {
      blockoffset = fileOffset;
      /*
      printf ("Block offset: %d\n", fileOffset);
      */
      block = readUInt16 (f);
      type = block >> 6;

      length = block & ((1 << 6) - 1);

      if (length == 63)		/* it's a long block. */
	{
		length = readUInt32 (f);
	}

      if (type == 0 || fileOffset >= m.size)
	break;

      /*
       printf ("Found Block: %s, %i bytes\n", blockName (type), length);
       */
       blockstart = fileOffset;
       nextFrame = fileOffset+length;

       blockp=blockParse(f,length,type);

       if( ftell(f) != nextFrame ) {
	       fprintf(stderr, " Stream out of sync...\n");
	       fprintf(stderr, " %ld but expecting %d\n", ftell(f),nextFrame);
       }

       if( blockp ) {
	       outputBlock( type, blockp, f, blockoffset, length );
	       free(blockp);
       } else {
	       printf("Error parsing block\n");
       }

	fseek(f, nextFrame, SEEK_SET);

    }

  putchar ('\n');

  if (fileOffset < m.size)
    {
      printf ("extra garbage (i.e., we messed up in main): \n");
      dumpBytes (f, m.size - fileOffset);
      printf ("\n\n");
    }

  outputTrailer(&m);

  fclose (f);
  if (compressed)
    {
      unlink (tmp_name);
    }
  exit (0);
}
