/****************************************************************************
 *
 *  Copyright (C) 2005-2006 "Stuart R. Anderson" <anderson@netsweng.com>
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
 ****************************************************************************/

/* outputXXX.c */
extern char *methodcall(char *varname, char *method);
extern char *newobj(char *varname, char *obj);
extern OUTPUTRET outputSWF_RGBA(SWF_RGBA *color, char *pname);
extern OUTPUTRET outputSWF_MATRIX(SWF_MATRIX *matrix, char *fname);
extern OUTPUTRET outputSWF_GRADIENT(SWF_GRADIENT *gradient, char *gname);
extern OUTPUTRET outputSWF_FILLSTYLE(SWF_FILLSTYLE *fillstyle, char *parentname, int i);
extern OUTPUTRET outputSWF_FILLSTYLEARRAY(SWF_FILLSTYLEARRAY *fillstylearray, char *parentname);
extern OUTPUTRET outputSWF_LINESTYLE(SWF_LINESTYLE *linestyle, char *parentname, int i);
extern OUTPUTRET outputSWF_LINESTYLE2(SWF_LINESTYLE2 *linestyle, char *parentname, int i);
extern OUTPUTRET outputSWF_LINESTYLEARRAY(SWF_LINESTYLEARRAY *linestylearray, char *parentname);
extern OUTPUTRET outputSWF_SHAPERECORD(SWF_SHAPERECORD *shaperec, char *parentname);
extern OUTPUTRET outputSWF_SHAPE(SWF_SHAPE *shape, char *name);
extern OUTPUTRET outputSWF_SHAPEWITHSTYLE(SWF_SHAPEWITHSTYLE *shape, int level, char *name);
extern OUTPUTRET outputSWF_CHARACTERSET(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBITS(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBITSJPEG2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBITSJPEG3(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBITSPTR(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBUTTON(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBUTTON2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBUTTONCXFORM(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBUTTONSOUND(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINECOMMANDOBJ(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEEDITTEXT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEFONT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEFONT2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEFONT3(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEFONTINFO(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEFONTINFO2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_CSMTEXTSETTINGS(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEFONTALIGNZONES(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEFONTNAME(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINELOSSLESS(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINELOSSLESS2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEMORPHSHAPE(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEMORPHSHAPE2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINESHAPE(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINESHAPE2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINESHAPE3(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINESHAPE4(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINESOUND(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINESPRITE(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINETEXT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINETEXT2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINETEXTFORMAT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEVIDEO(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEVIDEOSTREAM(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DOACTION(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_ENABLEDEBUGGER(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_ENABLEDEBUGGER2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_END(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_EXPORTASSETS(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_FONTREF(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_FRAMELABEL(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_FRAMETAG(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_FREEALL(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_FREECHARACTER(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_GENCOMMAND(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_IMPORTASSETS(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_IMPORTASSETS2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_JPEGTABLES(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_NAMECHARACTER(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_PATHSAREPOSTSCRIPT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_PLACEOBJECT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_PLACEOBJECT2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_PLACEOBJECT3(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_PREBUILT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_PREBUILTCLIP(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_PROTECT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_REMOVEOBJECT(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_REMOVEOBJECT2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SERIALNUMBER(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SETBACKGROUNDCOLOR(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SHOWFRAME(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SOUNDSTREAMBLOCK(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SOUNDSTREAMHEAD(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SOUNDSTREAMHEAD2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_STARTSOUND(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_STARTSOUND2(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SYNCFRAME(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_INITACTION(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_VIDEOFRAME(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_REFLEX(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_FILEATTRIBUTES(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_METADATA(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SCRIPTLIMITS(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINESCALINGGRID(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SETTABINDEX(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DOABC(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_SYMBOLCLASS(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINEBINARYDATA(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEFINESCENEANDFRAMEDATA(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_DEBUGID(SWF_Parserstruct *pblock);
extern OUTPUTRET outputSWF_UNKNOWNBLOCK(SWF_Parserstruct *pblock);
extern void outputHeader(struct Movie *m);
extern void outputTrailer(struct Movie *m);
extern void outputBlock(int type, SWF_Parserstruct *blockp, FILE *stream);
