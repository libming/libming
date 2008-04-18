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

extern void parseSWF_RGB(FILE *f, struct SWF_RGBA *rgb);
extern void parseSWF_RGBA(FILE *f, struct SWF_RGBA *rgb);
extern void parseSWF_RECT(FILE *f, struct SWF_RECT *rect);
extern void parseSWF_MATRIX(FILE *f, struct SWF_MATRIX *matrix);
extern void parseSWF_CXFORMWITHALPHA(FILE *f, struct SWF_CXFORMWITHALPHA *cxform);
extern int parseSWF_CLIPEVENTFLAGS(FILE *f, struct SWF_CLIPEVENTFLAGS *cflags);
extern void parseSWF_GRADIENTRECORD(FILE *f, struct SWF_GRADIENTRECORD *gradientrec, int level);
extern void parseSWF_GRADIENT(FILE *f, struct SWF_GRADIENT *gradient, int level);
extern int parseSWF_SHAPERECORD(FILE *f, SWF_SHAPERECORD *shape, int *fillBits, int *lineBits, int level);
extern void parseSWF_FILLSTYLE(FILE *f, SWF_FILLSTYLE *fillstyle, int level);
extern void parseSWF_FILLSTYLEARRAY(FILE *f, SWF_FILLSTYLEARRAY *fillstyle, int level);
extern void parseSWF_LINESTYLE(FILE *f, SWF_LINESTYLE *linestyle, int level);
extern void parseSWF_LINESTYLEARRAY(FILE *f, SWF_LINESTYLEARRAY *linestyle, int level);
extern void parseSWF_SHAPE(FILE *f, SWF_SHAPE *shape, int level, int len);
extern void parseSWF_SHAPEWITHSTYLE(FILE *f, SWF_SHAPEWITHSTYLE *shape, int level);
extern int parseSWF_ACTIONRECORD(FILE *f, int *this, SWF_ACTION *action);
extern SWF_Parserstruct *parseSWF_CHARACTERSET(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBITS(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBITSJPEG2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBITSJPEG3(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBITSPTR(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBUTTON(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBUTTON2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBUTTONCXFORM(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBUTTONSOUND(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINECOMMANDOBJ(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEEDITTEXT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEFONT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEFONT2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEFONT3(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEFONTINFO(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEFONTINFO2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_CSMTEXTSETTINGS(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEFONTNAME(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEFONTALIGNZONES(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINELOSSLESS(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINELOSSLESS2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEMORPHSHAPE(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEMORPHSHAPE2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINESHAPE(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINESHAPE2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINESHAPE3(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINESHAPE4(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINESOUND(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINESPRITE(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINETEXT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINETEXT2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINETEXTFORMAT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEVIDEO(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEVIDEOSTREAM(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DOACTION(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_ENABLEDEBUGGER(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_ENABLEDEBUGGER2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_END(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_EXPORTASSETS(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_FONTREF(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_FRAMELABEL(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_FRAMETAG(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_FREEALL(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_FREECHARACTER(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_GENCOMMAND(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_IMPORTASSETS(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_IMPORTASSETS2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_INITACTION(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_JPEGTABLES(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_NAMECHARACTER(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_PATHSAREPOSTSCRIPT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_PLACEOBJECT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_PLACEOBJECT2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_PLACEOBJECT3(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_PREBUILT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_PREBUILTCLIP(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_PROTECT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_REMOVEOBJECT(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_REMOVEOBJECT2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SERIALNUMBER(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SETBACKGROUNDCOLOR(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SHOWFRAME(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SOUNDSTREAMBLOCK(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SOUNDSTREAMHEAD(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SOUNDSTREAMHEAD2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_STARTSOUND(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_STARTSOUND2(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SYNCFRAME(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_VIDEOFRAME(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_REFLEX(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_FILEATTRIBUTES(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_METADATA(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SCRIPTLIMITS(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINESCALINGGRID(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SETTABINDEX(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DOABC(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_SYMBOLCLASS(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINEBINARYDATA(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEFINESCENEANDFRAMEDATA(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_UNKNOWNBLOCK(FILE *f, int length);
extern SWF_Parserstruct *parseSWF_DEBUGID(FILE *f, int length);
