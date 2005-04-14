
#include "blocks/blocktypes.h"

const char *blockName(int header)
{
  switch(header)
  {
    case 2:  return "DefineShape";	       	break;
    case 22: return "DefineShape2";	       	break;
    case 32: return "DefineShape3";		break;
    case 46: return "DefineMorphShape";		break;
    case 6:  return "DefineBits";	        break;
    case 21: return "DefineBitsJPEG2";		break;
    case 35: return "DefineBitsJPEG3";		break;
    case 20: return "DefineBitsLossless";	break;
    case 36: return "DefineBitsLossless2";	break;
    case 8:  return "JPEGTables";		break;
    case 7:  return "DefineButton";		break;
    case 34: return "DefineButton2";		break;
    case 23: return "DefineButtonCxForm";	break;
    case 17: return "DefineButtonSound";	break;
    case 10: return "DefineFont";		break;
    case 48: return "DefineFont2";		break;
    case 13: return "DefineFontInfo";		break;
    case 11: return "DefineText";		break;
    case 33: return "DefineText2";		break;
    case 14: return "DefineSound";	        break;
    case 19: return "SoundStreamBlock";		break;
    case 18: return "SoundStreamHead";		break;
    case 45: return "SoundStreamHead2";		break;
    case 39: return "DefineSprite";		break;
    case 4:  return "PlaceObject";		break;
    case 26: return "PlaceObject2";		break;
    case 5:  return "RemoveObject";		break;
    case 28: return "RemoveObject2";		break;
    case 1:  return "ShowFrame";		break;
    case 9:  return "SetBackgroundColor";	break;
    case 43: return "FrameLabel";		break;
    case 24: return "Protect";			break;
    case 15: return "StartSound";	        break;
    case 0:  return "End";		        break;
    case 12: return "DoAction"; 	        break;
    case 37: return "TextField";                break;
    case 56: return "LibrarySymbol";            break;
    case 57: return "ImportAssets";             break;
    case 58: return "Password";                 break;

    default: return "Unknown block type";	break;
  }
}
