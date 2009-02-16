/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* $Id$ */


#include "ming_config.h"

#include "libming.h"
#include "fromswf.h"
#include "method.h"
#include "input.h"
#include "output.h"
#include "error.h"

#ifndef __C2MAN__
#include <stdlib.h>
#include <string.h>
#endif

#if USE_ZLIB
#include <zlib.h>
#endif

static void
writeSWFPrebuiltClipToMethod(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	SWFPrebuiltClip clip = (SWFPrebuiltClip) block;
	methodWriteUInt16(CHARACTERID(clip), method, data);
	methodWriteUInt16(clip->frames, method, data);
	
	SWFOutput_writeToMethod(clip->display, method, data);
}

static int
completeSWFPrebuiltClip(SWFBlock block)
{
	return 4 + SWFPrebuiltClipLength(block);
}

int
SWFPrebuiltClipLength(SWFBlock block)
{	SWFPrebuiltClip clip = (SWFPrebuiltClip)block;
	return SWFOutput_getLength(clip->display);
}

/*
 * destroys a SWFPrebuiltClip instance
 */
void
destroySWFPrebuiltClip(SWFPrebuiltClip clip)
{	destroySWFOutput(clip->display);
	destroySWFCharacter((SWFCharacter) clip);
}

SWFPrebuiltClip
newSWFPrebuiltClip()
{
	SWFPrebuiltClip clip = (SWFPrebuiltClip)malloc(sizeof(struct SWFPrebuiltClip_s));

	SWFCharacterInit((SWFCharacter)clip);

	BLOCK(clip)->type = SWF_PREBUILTCLIP;
	BLOCK(clip)->writeBlock = writeSWFPrebuiltClipToMethod;
	BLOCK(clip)->complete = completeSWFPrebuiltClip;
	BLOCK(clip)->dtor = (destroySWFBlockMethod) destroySWFPrebuiltClip;
	
	clip->frames = 0;
	clip->display = newSWFOutput();
	return clip;
}

static void
writeSWFPrebuiltToMethod(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	SWFPrebuilt defines = (SWFPrebuilt) block;
	
	SWFOutput_writeToMethod(defines->defines, method, data);
}

static int
completeSWFPrebuilt(SWFBlock block)
{
	SWFPrebuilt defines = (SWFPrebuilt) block;
	return SWFOutput_getLength(defines->defines);
}

void
destroySWFPrebuilt(SWFPrebuilt defines)
{	destroySWFOutput(defines->defines);
	free((SWFBlock) defines);
}

SWFPrebuilt
newSWFPrebuilt()
{
	SWFPrebuilt data = (SWFPrebuilt)malloc(sizeof(struct SWFPrebuilt_s));
	
	SWFBlockInit((SWFBlock)data);
	BLOCK(data)->type = SWF_PREBUILT;
	BLOCK(data)->writeBlock = writeSWFPrebuiltToMethod;
	BLOCK(data)->complete = completeSWFPrebuilt;
	BLOCK(data)->dtor = (destroySWFBlockMethod) destroySWFPrebuilt;

	data->defines = newSWFOutput();
	return data;
}

// functions to read swf

static int verbose = {0};

struct bitstream
{	char lastch;
	char bitoff;
	unsigned char (*readc)(void *);
};
typedef struct bitstream *BITS;

#define alignbits(x) ((BITS)x)->bitoff = 0

static int
getbits(BITS bp, int nbits)
{	int res = 0, nb, db;
	for(nb = 0 ; nb < nbits ; nb += db)
	{	if(bp->bitoff == 0)
		{	bp->lastch = bp->readc(bp);
			bp->bitoff = 8;
		}
		db = nbits - nb;
		if(db > bp->bitoff)
			db = bp->bitoff;
		/* db bits von vorn wegnehmen */
		res <<= db;
		res |= (bp->lastch >> (bp->bitoff -= db)) & ((1 << db) - 1);
	}
	return res;
}
static int
getsbits(BITS bp, int nbits)
{	int res = getbits(bp, nbits);
	if(res & (1 << (nbits-1)))
		res |= (-1) << nbits;
	return(res);
}

/* rectangle */
static void rect(BITS bp)
{	int nbits, xmin, xmax, ymin, ymax;
	nbits = getbits(bp, 5);
	xmin = getbits(bp, nbits);
	xmax = getbits(bp, nbits);
	ymin = getbits(bp, nbits);
	ymax = getbits(bp, nbits);
	if(verbose)
		printf("rect %.2f,%.2f %.2f,%.2f\n", xmin/Ming_scale, ymin/Ming_scale, xmax/Ming_scale, ymax/Ming_scale);
}
/* matrix */
static void matrix(BITS bp)
{	int hasscale, nscalebits, scalex, scaley;
	int hasrotate, nrotatebits, rotateskew0, rotateskew1;
	int ntranslatebits, translatex, translatey;
	if((hasscale = getbits(bp, 1)))
	{	nscalebits = getbits(bp, 5);
		scalex = getbits(bp, nscalebits);
		scaley = getbits(bp, nscalebits);
		if(verbose)
			printf("scale %d %d\n", scalex, scaley);
	}
	if((hasrotate = getbits(bp, 1)))
	{	nrotatebits = getbits(bp, 5);
		rotateskew0 = getsbits(bp, nrotatebits);
		rotateskew1 = getsbits(bp, nrotatebits);
		if(verbose)
			printf("skew %d %d\n", rotateskew0, rotateskew1);
	}
	ntranslatebits = getbits(bp, 5);
	translatex = getsbits(bp, ntranslatebits);
	translatey = getsbits(bp, ntranslatebits);
	if(verbose)
		printf("translate %d %d\n", translatex, translatey);
}
/* rgb */
static void rgb(BITS bp)
{	int r, g, b;
	alignbits(bp);
	r = bp->readc(bp); g = bp->readc(bp); b = bp->readc(bp);
	if(verbose)
		printf("rgb %x %x %x\n", r, g, b);
}
static void rgba(BITS bp)
{	int r, g, b, a;
	alignbits(bp);
	r = bp->readc(bp); g = bp->readc(bp); b = bp->readc(bp);
	a = bp->readc(bp);
	if(verbose)
		printf("rgba %x %x %x %x\n", r, g, b, a);
}
static int
readint2(BITS bp)
{	int res;
	res = bp->readc(bp);
	res |= bp->readc(bp) << 8;
	return res;
}
static int
readint4(BITS bp)
{	int res;
	res = bp->readc(bp);
	res |= bp->readc(bp) << 8;
	res |= bp->readc(bp) << 16;
	res |= bp->readc(bp) << 24;
	return res;
}
static void putint2(unsigned char *p, int val)
{	*p++ = val;
	*p++ = val >> 8;
}
static void putint4(unsigned char *p, int val)
{	*p++ = val;
	*p++ = val >> 8;
	*p++ = val >> 16;
	*p++ = val >> 24;
}

/* open a swf file as a stream */
struct swfile
{	char lastch;
	char bitoff;
	unsigned char (*readc)(struct swfile *);

	char *name;	
	unsigned char vers[4];
	int fsize;
	
	char rect[9];
	unsigned short rectlen;
	SWFInput input;
	short frames, rate;

	short compressed;
	unsigned char *zbuf, *zptr, *zend;
};
static unsigned char freadc(struct swfile *sp)
{	return SWFInput_getChar(sp->input);
}
static unsigned char r_readc(struct swfile *sp)
{	return sp->rect[sp->rectlen++] = freadc(sp);
}
static void swfseek(struct swfile *sp, int delta)
{	SWFInput_seek(sp->input, delta, 1);
}

static struct swfile *openswf(SWFInput input)
{	struct swfile *res = (struct swfile *)malloc(sizeof(struct swfile));
	SWFInput_read(input, res->vers, 4);
	if(memcmp(res->vers, "FWS", 3) && memcmp(res->vers, "CWS", 3))
		SWF_error("input not a SWF stream\n");
	res->fsize = SWFInput_getUInt32(input);
	res->compressed = res->vers[0] == 'C';
	if(res->compressed)
	{
#if USE_ZLIB
		static z_stream z = {0};
		int len = SWFInput_length(input);
		unsigned char *zbuf;

		z.next_in = (unsigned char *)malloc(z.avail_in = len - 8);
		SWFInput_read(input, z.next_in, z.avail_in);
		// caller will do, leave it here for double memory consumption
		//destroySWFInput(input);
		zbuf = z.next_out = (unsigned char *)malloc(z.avail_out = res->fsize - 8);
		inflateInit(&z);
		inflate(&z, Z_FINISH);
		inflateEnd(&z);
		input = newSWFInput_allocedBuffer(zbuf, z.next_out-zbuf);
#else
		SWF_error("The SWF to be opened is compressed, but we can't uncompress it (no zlib compiled into this version of Ming).\n");
		return NULL;
#endif
	}
	res->input = input;
	// setup to read that rect...
	alignbits(res);
	res->rectlen = 0;
	res->readc = r_readc;
	rect((BITS) res);
	res->readc = freadc;
	readint2((BITS) res);	// movie rate
	res->frames = readint2((BITS) res);
	return res;
}

/* tag */
struct swftag
{	char lastch;
	char bitoff;
	unsigned char (*readc)(struct swftag *sp);

	short type; int size;
	unsigned char hdr[6]; short hdrlen;
	unsigned char *datbuf, *datptr, *datend;
	short alloced;
};
typedef struct swftag *TAG;

static unsigned char treadc(TAG tp)
{	return *tp->datptr++;
}

static TAG readtag_common(BITS bp)
{	TAG res = (TAG) malloc(sizeof(struct swftag));
	res->type = readint2(bp);
	res->size = res->type & 63;
	putint2(res->hdr, res->type);
	res->type >>= 6;
	res->hdrlen = 2;
	if(res->size == 63)
	{	res->size = readint4(bp);
		putint4(res->hdr+2, res->size);
		res->hdrlen = 6;
	}
	res->bitoff = 0;
	res->readc = treadc;
	res->alloced = 0;
	return res;
}
static TAG readtag_file(struct swfile *sp)
{	TAG res = (TAG) readtag_common((BITS) sp);
	if(res->size)
	{	res->datbuf = res->datptr = (unsigned char *)malloc(res->size);
		res->datend = res->datbuf + res->size;
		SWFInput_read(sp->input, res->datbuf, res->size);
		res->alloced = 1;
	}
	return res;
}
static TAG readtag_sprite(TAG tp)
{	TAG res = readtag_common((BITS) tp);
	if(res->size)
	{	res->datbuf = res->datptr = tp->datptr;
		res->datend = res->datbuf + res->size;
		tp->datptr += res->size;
	}
	return res;
}

static int idoffset = {0}, maxid = {0};
static int change_id(TAG tp)
{	int val = readint2((BITS) tp);
	if(val != 0 && val != 65535)
	{	val += idoffset;
		if(val > maxid)
			maxid = val;
		putint2(tp->datptr-2, val);
	}
	return val;
}

// processing functions for handle()
static void defineshape(TAG tp, int lev);
static void definetext(TAG tp, int lev);
static void placeobject(TAG tp, int lv);
static void definebutton(TAG tp);
static void definebutton2(TAG tp);
static void definetextfield(TAG tp);
static void definesprite(TAG tp);
static void definemorphshape(TAG tp,int lev);
static void exportassets(TAG tp);
static void definebuttonsound(TAG tp);
static void fillandlinestyles(TAG tp, int lev);
static void linestyle(TAG tp, int lev);
static void shape(TAG tp, int lev);
static void morphlinestyle2(TAG tp);

static int drop_tag(TAG tp)
{
	switch(tp->type)
	{
		case SWF_FILEATTRIBUTES:
		case SWF_METADATA:
		case SWF_DEFINESCENEANDFRAMEDATA: // only allowed in main timeline -> merge ?
			return 1;
		default:
			return 0;
	}
}

static int handle_tag(TAG tp)
{	int id;
	int displaylist = 0;
	if(verbose)
	{	char *tagnam = "inknown";
		switch(tp->type)
		{
			case SWF_END:
				tagnam = "stagEnd"; break;
			case SWF_SHOWFRAME:
				tagnam = "stagShowFrame"; break;
			case SWF_DEFINESHAPE:
				tagnam = "stagDefineShape"; break;
			case SWF_FREECHARACTER:
				tagnam = "stagFreeCharacter"; break;
			case SWF_PLACEOBJECT:
				tagnam = "stagPlaceObject"; break;
			case SWF_REMOVEOBJECT:
				tagnam = "stagRemoveObject"; break;
			case SWF_DEFINEBITS:
				tagnam = "stagDefineBits"; break;
			case SWF_DEFINEBUTTON:
				tagnam = "stagDefineButton"; break;
			case SWF_JPEGTABLES:
				tagnam = "stagJPEGTables"; break;
			case SWF_SETBACKGROUNDCOLOR:
				tagnam = "stagSetBackgroundColor"; break;
			case SWF_DEFINEFONT:
				tagnam = "stagDefineFont"; break;
			case SWF_DEFINETEXT:
				tagnam = "stagDefineText"; break;
			case SWF_DOACTION:
				tagnam = "stagDoAction"; break;
			case SWF_DEFINEFONTINFO:
				tagnam = "stagDefineFontInfo"; break;
			case SWF_DEFINESOUND:
				tagnam = "stagDefineSound"; break;
			case SWF_STARTSOUND:
				tagnam = "stagStartSound"; break;
			case SWF_DEFINEBUTTONSOUND:
				tagnam = "stagDefineButtonSound"; break;
			case SWF_SOUNDSTREAMHEAD:
				tagnam = "stagSoundStreamHead"; break;
			case SWF_SOUNDSTREAMBLOCK:
				tagnam = "stagSoundStreamBlock"; break;
			case SWF_DEFINELOSSLESS:
				tagnam = "stagDefineBitsLossless"; break;
			case SWF_DEFINEBITSJPEG2:
				tagnam = "stagDefineBitsJPEG2"; break;
			case SWF_DEFINESHAPE2:
				tagnam = "stagDefineShape2"; break;
			case SWF_DEFINEBUTTONCXFORM:
				tagnam = "stagDefineButtonCxform"; break;
			case SWF_PROTECT:
				tagnam = "stagProtect"; break;
			case SWF_PLACEOBJECT2:
				tagnam = "stagPlaceObject2"; break;
			case SWF_PLACEOBJECT3:
				tagnam = "stagPlaceObject3"; break;
			case SWF_REMOVEOBJECT2:
				tagnam = "stagRemoveObject2"; break;
			case SWF_DEFINESHAPE3:
				tagnam = "stagDefineShape3"; break;
			case SWF_DEFINESHAPE4:
				tagnam = "stagDefineShape4"; break;
			case SWF_DEFINETEXT2:
				tagnam = "stagDefineText2"; break;
			case SWF_DEFINEBUTTON2:
				tagnam = "stagDefineButton2"; break;
			case SWF_DEFINEBITSJPEG3:
				tagnam = "stagDefineBitsJPEG3"; break;
			case SWF_DEFINELOSSLESS2:
				tagnam = "stagDefineBitsLossless2"; break;
			case SWF_DEFINEEDITTEXT:
				tagnam = "stagDefineTextField"; break;
			case SWF_DEFINESPRITE:
				tagnam = "stagDefineSprite"; break;
			case SWF_NAMECHARACTER:
				tagnam = "stagNameCharacter"; break;
			case SWF_FRAMELABEL:
				tagnam = "stagFrameLabel"; break;
			case SWF_SOUNDSTREAMHEAD2:
				tagnam = "stagSoundStreamHead2"; break;
			case SWF_DEFINEMORPHSHAPE:
				tagnam = "stagDefineMorphShape"; break;
			case SWF_DEFINEMORPHSHAPE2:
				tagnam = "stagDefineMorphShape2"; break;
			case SWF_DEFINEFONT2:
				tagnam = "stagDefineFont2"; break;
			case SWF_DEFINEFONT3:
				tagnam = "stagDefineFont3"; break;
			case SWF_DEFINEFONTALIGNZONES:
				tagnam = "stagDefineFontAlignZones"; break;
			case SWF_EXPORTASSETS:
				tagnam = "stagExportAssets"; break;
			/*case stagClipInit:	
				tagnam = "stagClipInit"; break;*/
			/*case stagNewFontInfo:
				tagnam = "stagNewFontInfo"; break;*/
			case SWF_DEFINEVIDEOSTREAM:
				tagnam = "stagDefineVideoStrem"; break;
			case SWF_VIDEOFRAME:
				tagnam = "stagVideoFrame"; break;
			case SWF_INITACTION:
				tagnam = "stagInitAction"; break;
		}
		printf("tag %d %s\n", tp->type, tagnam);
	}
	switch(tp->type)
	{	/* tags without an id */
		case SWF_END:
		case SWF_SHOWFRAME:
		case SWF_SETBACKGROUNDCOLOR:
		case SWF_DOACTION:
		case SWF_PROTECT:
		case SWF_REMOVEOBJECT2:
		case SWF_FRAMELABEL:
		case SWF_SOUNDSTREAMHEAD:
		case SWF_SOUNDSTREAMHEAD2:
		case SWF_SOUNDSTREAMBLOCK:
		case SWF_JPEGTABLES:
		case SWF_SYMBOLCLASS:
		case SWF_DOABC:
		case SWF_DEFINEFONTNAME:
		/*case stagClipInit:*/
			break;
		/* simple tags - one id at beginning */
		case SWF_FREECHARACTER:
		case SWF_PLACEOBJECT:
		case SWF_REMOVEOBJECT:
		case SWF_DEFINEBITS:
		case SWF_DEFINEBITSJPEG2:
		case SWF_DEFINEBITSJPEG3:
		case SWF_DEFINELOSSLESS:
		case SWF_DEFINELOSSLESS2:
		case SWF_DEFINEFONT:
		case SWF_DEFINEFONTINFO:
		case SWF_DEFINESOUND:
		case SWF_STARTSOUND:
		case SWF_NAMECHARACTER:
		case SWF_DEFINEFONT2:
		case SWF_DEFINEFONT3:
		case SWF_DEFINEFONTALIGNZONES:
		case SWF_DEFINEBUTTONCXFORM:
		case SWF_DEFINEVIDEOSTREAM:
		case SWF_VIDEOFRAME:
		case SWF_INITACTION:
		case SWF_DEFINEBINARYDATA:
		/*case SWF_NEWFONTINFO:*/
			id = change_id(tp);
			if(verbose)
				printf("id %d offset %d\n", id, idoffset);
			break;
		/* to be processed specially */
		case SWF_DEFINESHAPE:
			defineshape(tp, 1);
			break;
		case SWF_DEFINETEXT:
			definetext(tp, 1);
			break;
		case SWF_DEFINESHAPE2:
			defineshape(tp, 2);
			break;
		case SWF_PLACEOBJECT2:
			placeobject(tp, 2);
			break;
		case SWF_PLACEOBJECT3:
			placeobject(tp, 3);
			break;
		case SWF_DEFINESHAPE3:
			defineshape(tp, 3);
			break;
		case SWF_DEFINESHAPE4:
			defineshape(tp, 4);
			break;
		case SWF_DEFINETEXT2:
			definetext(tp, 2);
			break;
		case SWF_DEFINEBUTTON:
			definebutton(tp);
			break;
		case SWF_DEFINEBUTTON2:
			definebutton2(tp);
			break;
		case SWF_DEFINEEDITTEXT:
			definetextfield(tp);
			break;
		case SWF_DEFINESPRITE:
			definesprite(tp);
			break;
		case SWF_DEFINEMORPHSHAPE:
			definemorphshape(tp, 1);
			break;
		case SWF_DEFINEMORPHSHAPE2:
			definemorphshape(tp, 2);
			break;
		case SWF_EXPORTASSETS:
			exportassets(tp);
			break;
		case SWF_DEFINEBUTTONSOUND:
			definebuttonsound(tp);
			break;
		default:
			SWF_error("unknown tag %d\n", tp->type);
	}
	switch(tp->type)
	{	case SWF_END:
		case SWF_SHOWFRAME:
		case SWF_DOACTION:
		case SWF_REMOVEOBJECT2:
		case SWF_FRAMELABEL:
		case SWF_SOUNDSTREAMHEAD:
		case SWF_SOUNDSTREAMHEAD2:
		case SWF_SOUNDSTREAMBLOCK:
		case SWF_DEFINEVIDEOSTREAM:
		case SWF_VIDEOFRAME:
		case SWF_INITACTION:
		case SWF_FREECHARACTER:
		case SWF_PLACEOBJECT:
		case SWF_REMOVEOBJECT:
		case SWF_STARTSOUND:
		case SWF_NAMECHARACTER:
		case SWF_PLACEOBJECT2:
		case SWF_PLACEOBJECT3:
			displaylist = 1;
	}
	return displaylist;
}

static void definesprite(TAG sp)
{	TAG tp;
	int type;
	int id, frames;
	id = change_id(sp);
	frames = readint2((BITS) sp);
	if(verbose)
		printf("sprite %d, %d frames\n", id, frames);
	do
	{	tp = readtag_sprite(sp);
		handle_tag(tp);
		type = tp->type;
		free(tp);
	} while(type);
	if(sp->datptr != sp->datend)
		SWF_error("consistency check: file size wrong in sprite\n");
}

/* shape */
static void shaperecord(TAG tp, int nfillbits, int nlinebits, int lev)
{	int type, statenewstyles, statelinestyle, statefillstyle1, statefillstyle0;
	int statemoveto, movebits, movex, movey, fill0style, fill1style, linestyle;
	int edgeflag, nbits, genlineflag, dx, dy, vertlineflag;
	int cdx, cdy, adx, ady;
	
	while(1)
	{	type = getbits((BITS) tp, 1);
		
		if(!type)
		{	statenewstyles = getbits((BITS) tp, 1);
			statelinestyle = getbits((BITS) tp, 1);
			statefillstyle1 = getbits((BITS) tp, 1);
			statefillstyle0 = getbits((BITS) tp, 1);
			if((statemoveto = getbits((BITS) tp, 1)))
			{	movebits = getbits((BITS) tp, 5);
				movex = getsbits((BITS) tp, movebits);
				movey = getsbits((BITS) tp, movebits);
				if(verbose)
					printf("move %d %d\n", movex, movey);
			}
			if(statenewstyles == 0 && statelinestyle == 0 && statefillstyle1 == 0&& statefillstyle0 == 0 && statemoveto == 0)
				break;
			if(statefillstyle0)
			{	fill0style = getbits((BITS) tp, nfillbits);
				if(verbose)
					printf("fillstyle0 %d\n", fill0style);
			}
			if(statefillstyle1)
			{	fill1style = getbits((BITS) tp, nfillbits);
				if(verbose)
					printf("fillstyle1 %d\n", fill1style);
			}
			if(statelinestyle)
			{	linestyle = getbits((BITS) tp, nlinebits);
				if(verbose)
					printf("linestyle %d\n", linestyle);
			}
/* contrary to document, but
 ... seems to work AND make sense
 */
			if(statefillstyle0 || statefillstyle1 || statelinestyle)
				;
			else
			if(statenewstyles)
				fillandlinestyles(tp, lev);
		}
		else
		{	edgeflag = getbits((BITS) tp, 1);
			if(!edgeflag)	/* curve */
			{	nbits = getbits((BITS) tp, 4)+2;
				cdx = getsbits((BITS) tp, nbits);
				cdy = getsbits((BITS) tp, nbits);
				adx = getsbits((BITS) tp, nbits);
				ady = getsbits((BITS) tp, nbits);
				if(verbose)
					printf("curve %d,%d %d,%d\n", cdx, cdy, adx, ady);
			}
			else
			{	nbits = getbits((BITS) tp, 4)+2;
				if((genlineflag = getbits((BITS) tp, 1)))
				{	dx = getsbits((BITS) tp, nbits);
					dy = getsbits((BITS) tp, nbits);
				}
				else if((vertlineflag = getbits((BITS) tp, 1)))
				{	dx = 0;
					dy = getsbits((BITS) tp, nbits);
				}
				else
				{	dx = getsbits((BITS) tp, nbits);
					dy = 0;
				}
				if(verbose)
					printf("line %d,%d\n", dx, dy);
			}
		}
	}
}

static void gradient(TAG tp, int alpha, int cod)
{	int n, ngrad, r1;
	
	alignbits(tp);
	ngrad = tp->readc(tp);
	for(n = 0 ; n < ngrad ; n++)
	{	r1 = tp->readc(tp);
		if(verbose)
			printf("ratio %d\n", r1);
		if(alpha) rgba((BITS) tp); else rgb((BITS) tp);
	}
	if (cod == 0x13)
		readint2((BITS) tp);
}

static void fillstyle(TAG tp, int lev)
{	unsigned short id;
	char cod;

	alignbits(tp);
	cod = tp->readc(tp);
	switch(cod)
	{	case 0:
			if(verbose) printf("solid fill:\n"); break;
		case 0x10:
			if(verbose) printf("linear gradient fill\n"); break;
		case 0x12:
			if(verbose) printf("radial gradient fill:\n"); break;
		case 0x13:
			if(verbose) printf("focal gradient fill:\n"); break;
		case 0x40:
			if(verbose) printf("tiled bitmap fill:\n"); break;
		case 0x41:
			if(verbose) printf("clipped bitmap fill\n"); break;
		case 0x42:
			if(verbose) printf("tilled bitmap fill with hard edges\n"); break;
		case 0x43:
			if(verbose) printf("clipped bitmap fill with hard edges\n"); break;
	}
	if(cod == 0)
		if(lev >= 3) rgba((BITS) tp); else rgb((BITS) tp);
	else if(cod == 0x10 || cod == 0x12 || cod == 0x13)
	{	matrix((BITS) tp);
		gradient(tp, lev >= 3, cod);
	}
	else if(cod == 0x40 || cod == 0x41 || cod == 0x42 || cod == 0x43)
	{	id = change_id(tp);
		if(verbose)
			printf("fill with id %d\n", id);
		matrix((BITS) tp);
	}
	else
		printf("%s:%d: UNEXPEDCED %x\n", __FILE__, __LINE__, cod);
}

static void morphgradient(TAG tp)
{	int n, ngrad, r1, r2;
	
	alignbits(tp);
	ngrad = tp->readc(tp);
	for(n = 0 ; n < ngrad ; n++)
	{	r1 = tp->readc(tp);
		if(verbose)
			printf("ratio %d\n", r1);
		rgba((BITS) tp);
		r2 = tp->readc(tp);
		if(verbose)
			printf("ratio %d\n", r2);
		rgba((BITS) tp);
	}
}

static void morphfillstyle(TAG tp)
{	unsigned short id;
	char cod;

	alignbits(tp);
	cod = tp->readc(tp);
	switch(cod)
	{	case 0:
			if(verbose) printf("solid fill:\n"); break;
		case 0x10:
			if(verbose) printf("linear gradient fill\n"); break;
		case 0x12:
			if(verbose) printf("radial gradient fill:\n"); break;
		case 0x13:
			if(verbose) printf("focal gradient fill:\n"); break;
		case 0x40:
			if(verbose) printf("tiled bitmap fill:\n"); break;
		case 0x41:
			if(verbose) printf("clipped bitmap fill\n"); break;
		case 0x42:
			if(verbose) printf("tilled bitmap fill with hard edges\n"); break;
		case 0x43:
			if(verbose) printf("clipped bitmap fill with hard edges\n"); break;
	}
	if(cod == 0)
	{	rgba((BITS) tp);
		rgba((BITS) tp);
	}
	else if(cod == 0x10 || cod == 0x12 || cod == 0x13)
	{	matrix((BITS) tp);
		alignbits(tp);
		matrix((BITS) tp);
		morphgradient(tp);
	}
	else if(cod == 0x40 || cod == 0x41 || cod == 0x42 || cod == 0x43)
	{	id = change_id(tp);
		if(verbose) printf("fill with id %d\n", id);
		matrix((BITS) tp);
		alignbits(tp);
		matrix((BITS) tp);
	}
	else
		printf("%s:%d: UNEXPEDCED %x\n", __FILE__, __LINE__, cod);
}

static void fillandlinestyles(TAG tp, int lev)
{	int n, nfill, nline;

	alignbits(tp);
	nfill = tp->readc(tp);
	if(nfill == 0xff)
		nfill = readint2((BITS) tp);
	if(verbose)	
		printf ("%d fill styles\n", nfill);
	for(n = 0 ; n < nfill ; n++)
		fillstyle(tp, lev);
	alignbits(tp);
	nline = tp->readc(tp);
	if(nline == 0xff)
		nline = readint2((BITS) tp);
	if(verbose)
		printf ("%d line styles\n", nline);
	for(n = 0 ; n < nline ; n++)
		if (lev == 4)
			morphlinestyle2(tp);
		else
			linestyle(tp, lev);
}

static void linestyle(TAG tp, int lev)
{	unsigned short w;
	w = readint2((BITS) tp);
	if(verbose)
		printf ("w %d\n", w);
	if(lev >= 3) rgba((BITS) tp); else rgb((BITS) tp);
}

static void morphlinestyle(TAG tp)
{	unsigned short w1, w2;
	w1 = readint2((BITS) tp);
	w2 = readint2((BITS) tp);
	if(verbose)
		printf("w %d => %d\n", w1, w2);
	rgba((BITS) tp); rgba((BITS) tp);
}

static void morphlinestyle2(TAG tp)
{	unsigned short w1;
	int join_style;
	int has_fill;
	int is_morph = (tp->type == SWF_DEFINEMORPHSHAPE2);
	w1 = readint2((BITS) tp);
	if (is_morph) {
		unsigned short w2;
		w2 = readint2((BITS) tp);
		if(verbose)
			printf("w %d => %d\n", w1, w2);
	} else {
		if(verbose)
			printf("w %d\n", w1);
	}
	getbits((BITS) tp, 2);
	join_style = getbits((BITS) tp, 2);
	has_fill = getbits((BITS) tp, 1);
	getbits((BITS) tp, 11);
	if (join_style == 2) {
		readint2((BITS) tp);
	} else {
		if (has_fill) {
			if (is_morph)
				morphfillstyle(tp);
			else
				fillstyle(tp, 4);
		} else {
			rgba((BITS) tp);
			if (is_morph)
				rgba((BITS) tp);
		}
	}
}

static void defineshape(TAG tp, int lev)
{	unsigned short id;

	id = change_id(tp);
	if(verbose)
		printf("shape %d\n", id);
	alignbits(tp);
	rect((BITS) tp);
	if(lev == 4) {
		alignbits(tp);
		rect((BITS) tp);
		alignbits(tp);
		tp->readc(tp);
	}
	fillandlinestyles(tp, lev);
	shape(tp, lev);
}

static void shape(TAG tp, int lev)
{	int nfillbits, nlinebits;
	alignbits(tp);
	nfillbits = getbits((BITS) tp, 4);
	nlinebits = getbits((BITS) tp, 4);
	shaperecord(tp, nfillbits, nlinebits,lev);
}

static void definemorphshape(TAG tp, int lev)
{	unsigned short id, fcnt, lcnt;
	unsigned long loff;
	unsigned char *endp;
	int n;

	id = change_id(tp);
	if(verbose)
		printf("morph %d\n", id);
	rect((BITS) tp);
	alignbits(tp);
	rect((BITS) tp);
	if (lev == 2) {
		rect((BITS) tp);
		rect((BITS) tp);
		tp->readc(tp);
	}
	loff = readint4((BITS) tp);
	endp = tp->datptr + loff;
	fcnt = tp->readc(tp);
	if(fcnt == 0xff)
		fcnt = readint2((BITS) tp);
	if(verbose)
		printf ("%d fill styles\n", fcnt);
	for(n = 0 ; n < fcnt ; n++)
	{	alignbits(tp);
		morphfillstyle(tp);
	}
	lcnt = tp->readc(tp);
	if(lcnt == 0xff)
		lcnt = readint2((BITS) tp);
	if(verbose)
		printf ("%d line styles\n", lcnt);
	for(n = 0 ; n < lcnt ; n++)
	{	alignbits(tp);
		if (lev == 2)
			morphlinestyle2(tp);
		else
			morphlinestyle(tp);
	}
/*	for(n = 0 ; tp->datptr < endp ; n++)
	{	alignbits(tp);
		shaperecord(tp, bits_req(fcnt), bits_req(lcnt), 3);
	}
	for( ; n > 0 ; --n)
	{	alignbits(tp);
		shaperecord(tp, bits_req(fcnt), bits_req(lcnt), 3);
	}*/
	shape(tp, 3);
}

static void definetext(TAG tp, int lev)
{	unsigned short textid;
	int nglyphbits, nadvancebits;
	int n, nglyphs, chcode, dx;
	int hasfont, hascolor, hasyoffset, hasxoffset;
	unsigned short font=0, height;
	signed short xoffs, yoffs;
			
	textid = change_id(tp);
	if(verbose) printf("text %d\n", textid);
	rect((BITS) tp);
	alignbits(tp);
	matrix((BITS) tp);
	nglyphbits = tp->readc(tp);
	nadvancebits = tp->readc(tp);
	alignbits(tp);
	while(1)
	{	if(!getbits((BITS) tp, 1))
		{	nglyphs = getbits((BITS) tp, 7);
			if(!nglyphs)
				break;
			if(verbose) printf("%d chars:\n", nglyphs);
			for(n = 0 ; n < nglyphs ; n++)
			{	chcode = getbits((BITS) tp, nglyphbits);
				dx = getsbits((BITS) tp, nadvancebits);
				if(verbose) printf("code %d dx %d\n", chcode, dx);
			}
		}
		else
		{	getbits((BITS) tp, 3);
			hasfont = getbits((BITS) tp, 1);
			hascolor = getbits((BITS) tp, 1);
			hasyoffset = getbits((BITS) tp, 1);
			hasxoffset = getbits((BITS) tp, 1);
			if(hasfont)
				font = change_id(tp);
			if(hascolor)
			{	if(lev == 2) rgba((BITS) tp);
				else rgb((BITS) tp);
			}
			if(hasxoffset)
			{	xoffs = readint2((BITS) tp);
				if(verbose) printf("dx %d\n", xoffs);
			}
			if(hasyoffset)
			{	yoffs = readint2((BITS) tp);
				if(verbose) printf("dy %d\n", yoffs);
			}
			if(hasfont)
			{	height = readint2((BITS) tp);
				if(verbose) printf("font %d size %d\n", font, height);
			}
		}
		alignbits(tp);
	}
}

static void placeobject(TAG tp, int lv)
{	int hasname, hasratio, hascxform, hasmatrix, haschar, hasmove, hasactions, hasmask;
	short depth, charid;
	int hasfilters, hasbitmapcaching, hasblendmode;
	if (lv == 3) {
		getbits((BITS)tp, 5);
		hasbitmapcaching = getbits((BITS)tp, 1);
		hasblendmode = getbits((BITS)tp, 1);
		hasfilters = getbits((BITS)tp, 1);
	}
	hasactions = getbits((BITS)tp, 1);
	hasmask = getbits((BITS)tp, 1);
	hasname = getbits((BITS)tp, 1);
	hasratio = getbits((BITS)tp, 1);
	hascxform = getbits((BITS)tp, 1);
	hasmatrix = getbits((BITS)tp, 1);
	haschar = getbits((BITS)tp, 1);
	hasmove = getbits((BITS)tp, 1);
	depth = readint2((BITS) tp);
	if(haschar)
	{	charid = change_id(tp);
		if(verbose) printf("char %d depth %d\n", charid, depth);
	}
}

static void definebutton(TAG tp)
{	short butid, charid, layer;
	unsigned char bstate;

	butid = change_id(tp);
	if(verbose)
		printf("char %d:\n", butid);
	while((bstate = tp->readc(tp)))
	{	if(bstate & 8)
			if(verbose) printf("hit test ");
		if(bstate & 4)
			if(verbose) printf("down ");
		if(bstate & 2)
			if(verbose) printf("over ");
		if(bstate & 1)
			if(verbose) printf("up");
		if(verbose) printf("\n");
		charid = change_id(tp);
		layer = readint2((BITS) tp);
		if(verbose) printf("char %d layer %d\n", charid, layer);
		alignbits(tp);
		matrix((BITS) tp);
	}
}

static void cxform(TAG tp, int alpha)
{	int hasadd, hasmult, nbits;
	int ra, ga, ba, aa, rm, gm, bm, am=0;
	hasadd = getbits((BITS) tp, 1);
	hasmult = getbits((BITS) tp, 1);
	nbits = getbits((BITS) tp, 4);
	if(verbose) printf("cxform ");
	if(hasmult)
	{	rm = getsbits((BITS) tp, nbits);
		gm = getsbits((BITS) tp, nbits);
		bm = getsbits((BITS) tp, nbits);
		if(alpha) am  = getsbits((BITS) tp, nbits);
		if(verbose) printf("mul %d %d %d ", rm, gm, bm);
		if(verbose) if(alpha) printf("%d ", am);
	}
	if(hasadd)
	{	ra = getsbits((BITS) tp, nbits);
		ga = getsbits((BITS) tp, nbits);
		ba = getsbits((BITS) tp, nbits);
		if(alpha) aa = getsbits((BITS) tp, nbits);
		if(verbose) printf("add %d %d %d", ra, ga, ba);
		if(verbose) if(alpha) printf(" %d", am);
	}
	if(verbose) printf("\n");
}

static void definebutton2(TAG tp)
{	short id, charid, offs, layer;
	unsigned char ch, bstate;

	id = change_id(tp);
	ch = tp->readc(tp);
	offs = readint2((BITS) tp);
	if(verbose)
		printf("id %d %s action offset %d\n", id, ch ? "menu" : "button", offs);
	while((bstate = tp->readc(tp)))
	{	if(bstate & 8)
			if(verbose) printf("hit test ");
		if(bstate & 4)
			if(verbose) printf("down ");
		if(bstate & 2)
			if(verbose) printf("over ");
		if(bstate & 1)
			if(verbose) printf("up");
		if(verbose) printf("\n");
		charid = change_id(tp);
		layer = readint2((BITS) tp);
		if(verbose) printf("char %d layer %d\n", charid, layer);
		alignbits(tp);
		matrix((BITS) tp);
		alignbits(tp);
		cxform(tp, 1);
	}
}

static void definetextfield(TAG tp)
{
	short textid, fontid=0;
	int haslength, noedit, password, multiline, wordwrap, drawbox, noselect, html, usefont;
	int hascolor, haslayout, hastext, hasfont;
	
	textid = change_id(tp);
	if(verbose) printf("textfield %d\n", textid);
	rect((BITS) tp);
	alignbits(tp);
	hastext = getbits((BITS) tp, 1);
	wordwrap = getbits((BITS) tp, 1);
	multiline = getbits((BITS) tp, 1);
	password = getbits((BITS) tp, 1);
	noedit = getbits((BITS) tp, 1);
	hascolor = getbits((BITS) tp, 1);
	haslength = getbits((BITS) tp, 1);
	hasfont = getbits((BITS) tp, 1);
	getbits((BITS) tp, 2);
	haslayout = getbits((BITS) tp, 1);
	noselect = getbits((BITS) tp, 1);
	drawbox = getbits((BITS) tp, 1);
	getbits((BITS) tp, 1);
	html = getbits((BITS) tp, 1);
	usefont = getbits((BITS) tp, 1);
	if(hasfont)
		fontid = change_id(tp);
	if(verbose)
	{	if ( fontid ) printf("font %d ", fontid);
		if(noedit) printf("noedit ");
		if(password) printf("password ");
		if(multiline) printf("multiline ");
		if(wordwrap) printf("wordwrap ");
		if(drawbox) printf("drawbox ");
		if(noselect) printf("noselect ");
		if(html) printf("html ");
		if(usefont) printf("usefont ");
		if(hascolor) rgba((BITS) tp);
		putchar('\n');
	}
}

static void exportassets(TAG tp)
{	short n, nobj, id;
	char ch;
	nobj = readint2((BITS) tp);
	for(n = 0 ; n < nobj ; n++)
	{	id = change_id(tp);
		if(verbose) printf("%d ", id);
		while((ch = tp->readc(tp)))
			if(verbose) putchar(ch);
		if(verbose)
			putchar((n < nobj-1) ? ' ' : '\n');
	}
}

static void soundinfo(TAG tp)
{	char flags, hasenvelope, hasloops, hasoutpoint, hasinpoint, npoints;
	signed short loops, lev0, lev1;
	int soundpoint, n;
	flags = getbits((BITS) tp, 4);
	hasenvelope = getbits((BITS) tp, 1);
	hasloops = getbits((BITS) tp, 1);
	hasoutpoint = getbits((BITS) tp, 1);
	hasinpoint = getbits((BITS) tp, 1);
	if(flags & 1)
		if(verbose) printf("no multiple ");
	if(flags & 2)
		if(verbose) printf("stop");
	if(flags & 3)
		if(verbose) putchar('\n');
	if(hasinpoint)
	{	soundpoint = readint4((BITS) tp);
		if(verbose) printf("inpoint %d\n", soundpoint);
	}
	if(hasoutpoint)
	{	soundpoint = readint4((BITS) tp);
		if(verbose) printf("outpoint %d\n", soundpoint);
	}
	if(hasloops)
	{	loops = readint2((BITS) tp);
		if(verbose) printf("%d loops\n", loops);
	}
	if(hasenvelope)
	{	npoints = tp->readc(tp);
		for(n = 0 ; n < npoints ; n++)
		{	soundpoint = readint4((BITS) tp);
			lev0 = readint2((BITS) tp);
			lev1 = readint2((BITS) tp);
			if(verbose) printf("%d: %d %d\n", soundpoint, lev0, lev1);
		}
	}
}

static void definebuttonsound(TAG tp)
{	int id, n, snds[4];
	id = change_id(tp);
	if(verbose)
		printf("id %d\n", id);
	for(n = 0 ; n < 4 ; n++)
	{	snds[n] = change_id(tp);
		if(verbose)
			printf("sound %d:\n", snds[n]);
		alignbits(tp);
		if(snds[n]) soundinfo(tp);
	}
}

extern int SWF_gNumCharacters;

/* 
 * load and create a SWF File as MovieClip 
 */
SWFPrebuiltClip
newSWFPrebuiltClip_fromInput(SWFInput input)
{	SWFPrebuiltClip clip;
	SWFOutput display, defines, out;
	SWFPrebuilt deps;
	TAG tp;
	struct swfile *swf;
	int type = 0, todisplay;
	
	swf = openswf(input);
	if ( ! swf ) return NULL;
	clip = newSWFPrebuiltClip();
	clip->frames = swf->frames;
	display = clip->display;
	deps = newSWFPrebuilt();
	SWFCharacter_addDependency((SWFCharacter)clip, (SWFCharacter)deps);
	defines = deps->defines;
	idoffset = SWF_gNumCharacters;
	maxid = SWF_gNumCharacters;
	// read the swf file here
	tp = readtag_file(swf);
		if(tp->type != SWF_SETBACKGROUNDCOLOR)
			swfseek(swf, -(tp->hdrlen + tp->size));
	if(tp->alloced)
		free(tp->datbuf);
	free(tp);
	do
	{	tp = readtag_file(swf);
		type = tp->type;
		if(drop_tag(tp))
		{
			if(tp->alloced)
				free(tp->datbuf);
			free(tp);
			continue;
		}
		todisplay = handle_tag(tp);
		out = todisplay ? display : defines;
		SWFOutput_writeBuffer(out, tp->hdr, tp->hdrlen);
		if(tp->size)
			SWFOutput_writeBuffer(out, tp->datbuf, tp->size);
		if(tp->alloced)
			free(tp->datbuf);
		free(tp);
	} while(type);
	if(swf->compressed)
		destroySWFInput(swf->input);
	SWF_gNumCharacters = maxid + 1;
	CHARACTERID(clip) = SWF_gNumCharacters++;
	return clip;
}

/* 
 * load SWF file which can be used as a MovieClip
 */
SWFPrebuiltClip
newSWFPrebuiltClip_fromFile(const char *filename)
{	FILE *fp;
	SWFPrebuiltClip clip;
	SWFInput input;
	if(!(fp = fopen(filename, "rb")))
		return NULL;
	input = newSWFInput_file(fp);
	clip = newSWFPrebuiltClip_fromInput(input);
	destroySWFInput(input);
	fclose(fp);
	return clip;
}
