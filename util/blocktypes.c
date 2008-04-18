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

#include "blocks/blocktypes.h"
#include "parser.h"
#include "read.h"

struct SWFBlock
{
  SWFBlocktype type;
  char *name;
  SWFParseFunc parser;
};

#define BlockType( block ) \
{ block, #block, parse##block, }

static struct SWFBlock blocks[] = {
  BlockType (SWF_CHARACTERSET),
  BlockType (SWF_DEFINEBITS),
  BlockType (SWF_DEFINEBITSJPEG2),
  BlockType (SWF_DEFINEBITSJPEG3),
  BlockType (SWF_DEFINEBITSPTR),
  BlockType (SWF_DEFINEBUTTON),
  BlockType (SWF_DEFINEBUTTON2),
  BlockType (SWF_DEFINEBUTTONCXFORM),
  BlockType (SWF_DEFINEBUTTONSOUND),
  BlockType (SWF_DEFINECOMMANDOBJ),
  BlockType (SWF_DEFINEEDITTEXT),
  BlockType (SWF_DEFINEFONT),
  BlockType (SWF_DEFINEFONT2),
  BlockType (SWF_DEFINEFONT3),
  BlockType (SWF_DEFINEFONTINFO),
  BlockType (SWF_DEFINEFONTINFO2),
  BlockType (SWF_CSMTEXTSETTINGS),
  BlockType (SWF_DEFINEFONTNAME),
  BlockType (SWF_DEFINEFONTALIGNZONES),
  BlockType (SWF_DEFINELOSSLESS),
  BlockType (SWF_DEFINELOSSLESS2),
  BlockType (SWF_DEFINEMORPHSHAPE),
  BlockType (SWF_DEFINEMORPHSHAPE2),
  BlockType (SWF_DEFINESHAPE),
  BlockType (SWF_DEFINESHAPE2),
  BlockType (SWF_DEFINESHAPE3),
  BlockType (SWF_DEFINESHAPE4),
  BlockType (SWF_DEFINESOUND),
  BlockType (SWF_DEFINESPRITE),
  BlockType (SWF_DEFINETEXT),
  BlockType (SWF_DEFINETEXT2),
  BlockType (SWF_DEFINETEXTFORMAT),
  BlockType (SWF_DEFINEVIDEO),
  BlockType (SWF_DEFINEVIDEOSTREAM),
  BlockType (SWF_DOACTION),
  BlockType (SWF_ENABLEDEBUGGER),
  BlockType (SWF_ENABLEDEBUGGER2),
  BlockType (SWF_END),
  BlockType (SWF_EXPORTASSETS),
  BlockType (SWF_FONTREF),
  BlockType (SWF_FRAMELABEL),
  BlockType (SWF_FRAMETAG),
  BlockType (SWF_FREEALL),
  BlockType (SWF_FREECHARACTER),
  BlockType (SWF_GENCOMMAND),
  BlockType (SWF_IMPORTASSETS),
  BlockType (SWF_IMPORTASSETS2),
  BlockType (SWF_INITACTION),
  BlockType (SWF_JPEGTABLES),
  BlockType (SWF_NAMECHARACTER),
  BlockType (SWF_PATHSAREPOSTSCRIPT),
  BlockType (SWF_PLACEOBJECT),
  BlockType (SWF_PLACEOBJECT2),
  BlockType (SWF_PLACEOBJECT3),
  BlockType (SWF_PREBUILT),
  BlockType (SWF_PREBUILTCLIP),
  BlockType (SWF_PROTECT),
  BlockType (SWF_REMOVEOBJECT),
  BlockType (SWF_REMOVEOBJECT2),
  BlockType (SWF_SERIALNUMBER),
  BlockType (SWF_SETBACKGROUNDCOLOR),
  BlockType (SWF_SHOWFRAME),
  BlockType (SWF_SOUNDSTREAMBLOCK),
  BlockType (SWF_SOUNDSTREAMHEAD),
  BlockType (SWF_SOUNDSTREAMHEAD2),
  BlockType (SWF_STARTSOUND),
  BlockType (SWF_STARTSOUND2),
  BlockType (SWF_SYNCFRAME),
  BlockType (SWF_VIDEOFRAME),
  BlockType (SWF_REFLEX),
  BlockType (SWF_FILEATTRIBUTES),
  BlockType (SWF_METADATA),
  BlockType (SWF_SCRIPTLIMITS),
  BlockType (SWF_DEFINESCALINGGRID),
  BlockType (SWF_SETTABINDEX),
  BlockType (SWF_DOABC),
  BlockType (SWF_SYMBOLCLASS),
  BlockType (SWF_DEFINESCENEANDFRAMEDATA),
  BlockType (SWF_DEFINEBINARYDATA),
  BlockType (SWF_DEBUGID),
};

static int numBlocks = sizeof (blocks) / sizeof (struct SWFBlock);

const char *
blockName (SWFBlocktype header)
{
  int i;

  for (i = 0; i < numBlocks; i++)
    {
      if (blocks[i].type == header)
	{
	  return blocks[i].name;
	}
    }
  return "Unknown Block Type";	/* Should never get here */

}

SWF_Parserstruct *
blockParse (FILE *f, int length, SWFBlocktype header)
{
  int i;

  for (i = 0; i < numBlocks; i++)
  {
    if (blocks[i].type == header)
    {
      return blocks[i].parser(f,length);
    }
  }
  return parseSWF_UNKNOWNBLOCK(f, length);
}
