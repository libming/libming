/* action.h */

#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

enum
{
  SWFACTION_END        = 0x00,

/* v3 actions */
  SWFACTION_NEXTFRAME     = 0x04,
  SWFACTION_PREVFRAME     = 0x05,
  SWFACTION_PLAY          = 0x06,
  SWFACTION_STOP          = 0x07,
  SWFACTION_TOGGLEQUALITY = 0x08,
  SWFACTION_STOPSOUNDS    = 0x09,
  SWFACTION_GOTOFRAME     = 0x81, /* >= 0x80 means record has args */
  SWFACTION_GETURL        = 0x83,
  SWFACTION_WAITFORFRAME  = 0x8A,
  SWFACTION_SETTARGET     = 0x8B,
  SWFACTION_GOTOLABEL     = 0x8C,

/* v4 actions */
  SWFACTION_ADD                     = 0x0A,
  SWFACTION_SUBTRACT                = 0x0B,
  SWFACTION_MULTIPLY                = 0x0C,
  SWFACTION_DIVIDE                  = 0x0D,
  SWFACTION_EQUAL                   = 0x0E,
  SWFACTION_LESSTHAN                = 0x0F,
  SWFACTION_LOGICALAND              = 0x10,
  SWFACTION_LOGICALOR               = 0x11,
  SWFACTION_LOGICALNOT              = 0x12,
  SWFACTION_STRINGEQ                = 0x13,
  SWFACTION_STRINGLENGTH            = 0x14,
  SWFACTION_SUBSTRING               = 0x15,
  SWFACTION_INT                     = 0x18,
  SWFACTION_GETVARIABLE             = 0x1C,
  SWFACTION_SETVARIABLE             = 0x1D,
  SWFACTION_SETTARGETEXPRESSION     = 0x20,
  SWFACTION_STRINGCONCAT            = 0x21,
  SWFACTION_GETPROPERTY             = 0x22,
  SWFACTION_SETPROPERTY             = 0x23,
  SWFACTION_DUPLICATECLIP           = 0x24,
  SWFACTION_REMOVECLIP              = 0x25,
  SWFACTION_TRACE                   = 0x26,
  SWFACTION_STARTDRAGMOVIE          = 0x27,
  SWFACTION_STOPDRAGMOVIE           = 0x28,
  SWFACTION_STRINGCOMPARE           = 0x29,
  SWFACTION_RANDOM                  = 0x30,
  SWFACTION_MBLENGTH                = 0x31,
  SWFACTION_ORD                     = 0x32,
  SWFACTION_CHR                     = 0x33,
  SWFACTION_GETTIMER                = 0x34,
  SWFACTION_MBSUBSTRING             = 0x35,
  SWFACTION_MBORD                   = 0x36,
  SWFACTION_MBCHR                   = 0x37,

  SWFACTION_WAITFORFRAMEEXPRESSION  = 0x8D,
  SWFACTION_PUSHDATA                = 0x96,
  SWFACTION_BRANCHALWAYS            = 0x99,
  SWFACTION_GETURL2                 = 0x9A,
  SWFACTION_BRANCHIFTRUE            = 0x9D,
  SWFACTION_CALLFRAME               = 0x9E,
  SWFACTION_GOTOEXPRESSION          = 0x9F
};

enum
{
  SWFACTION_PUSHDATA_STRING         = 0,
  SWFACTION_PUSHDATA_PROPERTY       = 1
};

#endif /* ACTION_H_INCLUDED */
