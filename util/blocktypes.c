
#include "blocks/blocktypes.h"
#include "parser.h"

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
  BlockType (SWF_DEFINEFONTINFO),
  BlockType (SWF_DEFINELOSSLESS),
  BlockType (SWF_DEFINELOSSLESS2),
  BlockType (SWF_DEFINEMORPHSHAPE),
  BlockType (SWF_DEFINESHAPE),
  BlockType (SWF_DEFINESHAPE2),
  BlockType (SWF_DEFINESHAPE3),
  BlockType (SWF_DEFINESOUND),
  BlockType (SWF_DEFINESPRITE),
  BlockType (SWF_DEFINETEXT),
  BlockType (SWF_DEFINETEXT2),
  BlockType (SWF_DEFINETEXTFORMAT),
  BlockType (SWF_DEFINEVIDEO),
  BlockType (SWF_DEFINEVIDEOSTREAM),
  BlockType (SWF_DOACTION),
  BlockType (SWF_ENABLEDEBUGGER),
  BlockType (SWF_END),
  BlockType (SWF_EXPORTASSETS),
  BlockType (SWF_FONTREF),
  BlockType (SWF_FRAMELABEL),
  BlockType (SWF_FRAMETAG),
  BlockType (SWF_FREEALL),
  BlockType (SWF_FREECHARACTER),
  BlockType (SWF_GENCOMMAND),
  BlockType (SWF_IMPORTASSETS),
  BlockType (SWF_JPEGTABLES),
  BlockType (SWF_NAMECHARACTER),
  BlockType (SWF_PATHSAREPOSTSCRIPT),
  BlockType (SWF_PLACEOBJECT),
  BlockType (SWF_PLACEOBJECT2),
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
  BlockType (SWF_SYNCFRAME),
  BlockType (SWF_VIDEOFRAME),
};

static int numBlocks = sizeof (blocks) / sizeof (struct SWFBlock);

const char *
blockName (SWFBlocktype header)
{
  int i;

  if (header < 0 || header > numBlocks)
    return "Invalid Block Type";

  for (i = 0; i < numBlocks; i++)
    {
      if (blocks[i].type == header)
	{
	  return blocks[i].name;
	}
    }
  return "Confused Block Type";	/* Should never get here */

}

SWF_Parserstruct *
blockParse (FILE *f, int length, SWFBlocktype header)
{
  int i;

  if (header < 0 || header > numBlocks)
    return NULL;

  for (i = 0; i < numBlocks; i++)
    {
      if (blocks[i].type == header)
	{
	  return blocks[i].parser(f,length);
	}
    }
  return NULL;

}
