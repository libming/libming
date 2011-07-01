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

#ifndef _SWFTYPES_H_
#define _SWFTYPES_H_
typedef signed char	SI8;	/* Signed  8-bit integer value */
typedef signed short	SI16;	/* Signed 16-bit integer value */
typedef signed long	SI32;	/* Signed 32-bit integer value */
typedef unsigned char	UI8;	/* Unsigned  8-bit integer value */
typedef unsigned short	UI16;	/* Unsigned 16-bit integer value */
typedef unsigned long	UI32;	/* Unsigned 32-bit integer value */
typedef unsigned long 	U30;	/* Unsigned 30-bit variable-length encoded integer value */
typedef unsigned long 	U32;	/* Unsigned 32-bit variable-length encoded integer value */
typedef signed long	S32;	/* Signed 32-bit variable-length encoded integer value */

typedef unsigned long	FIXED;	/* 32-bit 16.16 fixed-point number */
typedef unsigned short	FIXED8;	/* 16-bit 8.8 fixed-point number */
typedef unsigned long	SB32;	/* Signed bit value */
typedef unsigned long	UB32;	/* Unsigned bit value */
typedef float		FB32;	/* Signed fixed-point bit value */
typedef char	*STRING;/* String */

typedef UI8 LANGCODE;
typedef UI8 BYTE;
typedef SI16 WORD;
typedef UI8 BOOLEAN;
typedef float		FLOAT;	/* 32-bit IEEE single-precision little endian fp value */
typedef double		DOUBLE;	/* 64-bit IEEE double-precision little endian double value */
typedef float 		FLOAT16; /* 16-bit IEEE double-precision little endian double value */

/* Types to represent Actions */

struct SWF_ACTIONRECORD {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
};

/* v3 actions */

struct SWF_ACTIONGOTOFRAME {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	WORD	Frame;
};

struct SWF_ACTIONGETURL {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	STRING	UrlString;
	STRING	TargetString;
};

struct SWF_ACTIONNEXTFRAME {
	UI8	ActionCode;
};

struct SWF_ACTIONPREVIOUSFRAME {
	UI8	ActionCode;
};

struct SWF_ACTIONPLAY {
	UI8	ActionCode;
};

struct SWF_ACTIONSTOP {
	UI8	ActionCode;
};

struct SWF_ACTIONTOGGLEQUALITY {
	UI8	ActionCode;
};

struct SWF_ACTIONSTOPSOUNDS {
	UI8	ActionCode;
};

struct SWF_ACTIONWAITFORFRAME {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	WORD	Frame;
	BYTE	SkipCount;
};

struct SWF_ACTIONSETTARGET {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	STRING	TargetName;
};

struct SWF_ACTIONGOTOLABEL {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	STRING	FrameLabel;
};

/* v4 actions */

struct SWF_ACTIONPUSHPARAM {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	UI8	Type;
	union {
		STRING	String;
		FLOAT	Float;
		int	RegisterNumber;
		int	Boolean;
		DOUBLE	Double;
		SI32	Integer;
		UI8	Constant8;
		UI16	Constant16;
	} p;
};

struct SWF_ACTIONPUSH {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	UI16	NumParam;
	struct SWF_ACTIONPUSHPARAM *Params;
};

struct SWF_ACTIONPOP {
	UI8	ActionCode;
};

struct SWF_ACTIONADD {
	UI8	ActionCode;
};

struct SWF_ACTIONSUBTRACT {
	UI8	ActionCode;
};

struct SWF_ACTIONMULTIPLY {
	UI8	ActionCode;
};

struct SWF_ACTIONDIVIDE {
	UI8	ActionCode;
};

struct SWF_ACTIONEQUALS {
	UI8	ActionCode;
};

struct SWF_ACTIONLESS {
	UI8	ActionCode;
};

struct SWF_ACTIONAND {
	UI8	ActionCode;
};

struct SWF_ACTIONOR {
	UI8	ActionCode;
};

struct SWF_ACTIONNOT {
	UI8	ActionCode;
	BOOLEAN	Boolean;
};

struct SWF_ACTIONSTRINGEQUALS {
	UI8	ActionCode;
};

struct SWF_ACTIONSTRINGLENGTH {
	UI8	ActionCode;
};

struct SWF_ACTIONSTRINGADD {
	UI8	ActionCode;
};

struct SWF_ACTIONSTRINGEXTRACT {
	UI8	ActionCode;
};

struct SWF_ACTIONSTRINGLESS {
	UI8	ActionCode;
};

struct SWF_ACTIONMBSTRINGLEN {
	UI8	ActionCode;
};

struct SWF_ACTIONMBSTRINGEXTRACT {
	UI8	ActionCode;
};

struct SWF_ACTIONTOINTEGER {
	UI8	ActionCode;
};

struct SWF_ACTIONCHARTOASCII {
	UI8	ActionCode;
};

struct SWF_ACTIONASCIITOCHAR {
	UI8	ActionCode;
};

struct SWF_ACTIONMBCHARTOASCII {
	UI8	ActionCode;
};

struct SWF_ACTIONMBASCIITOCHAR {
	UI8	ActionCode;
};

struct SWF_ACTIONJUMP {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	WORD	BranchOffset;
};

struct SWF_ACTIONIF {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	WORD	BranchOffset;
	int	numActions;
	union SWF_ACTION *Actions;
};

struct SWF_ACTIONCALL {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
};

struct SWF_ACTIONGETVARIABLE {
	UI8	ActionCode;
};

struct SWF_ACTIONSETVARIABLE {
	UI8	ActionCode;
};

struct SWF_ACTIONGETURL2 {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	union {
		struct {
			UI8	SendVarsMethod:2;
			UI8	Reserved:4;
			UI8	LoadTargetFlag:1;
			UI8	LoadVariableFlag:1;
		} FlagBits;
		UI8	Flags;
	} f;
};

struct SWF_ACTIONGOTOFRAME2 {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	union {
		struct {
			UI8	Reserved:6;
			UI8	SceneBiasFlag:1;
			UI8	PlayFlag:1;
		} FlagBits;
		UI8	Flags;
	} f;
	UI16	SceneBias;
};

struct SWF_ACTIONSETTARGET2 {
	UI8	ActionCode;
};

struct SWF_ACTIONGETPROPERTY {
	UI8	ActionCode;
};

struct SWF_ACTIONSETPROPERTY {
	UI8	ActionCode;
};

struct SWF_ACTIONCLONESPRITE {
	UI8	ActionCode;
};

struct SWF_ACTIONREMOVESPRITE {
	UI8	ActionCode;
};

struct SWF_ACTIONSTARTDRAG {
	UI8	ActionCode;
};

struct SWF_ACTIONENDDRAG {
	UI8	ActionCode;
};

struct SWF_ACTIONWAITFORFRAME2 {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	UI8	SkipCount;
};

struct SWF_ACTIONTRACE {
	UI8	ActionCode;
};

struct SWF_ACTIONGETTIME {
	UI8	ActionCode;
};

struct SWF_ACTIONRANDOMNUMBER {
	UI8	ActionCode;
};


/* v5 actions */

struct SWF_ACTIONCALLFUNCTION {
	UI8	ActionCode;
};

struct SWF_ACTIONCALLMETHOD {
	UI8	ActionCode;
};

struct SWF_ACTIONCONSTANTPOOL {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	UI16	Count;
	STRING	*ConstantPool;
};

struct SWF_ACTIONDEFINEFUNCTION {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	STRING	FunctionName;
	WORD	NumParams;
	STRING	*Params;
	WORD	CodeSize;
	int	numActions;
	union SWF_ACTION *Actions;
};

struct SWF_ACTIONWITH {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	UI16	Size;
	int	numActions;
	union SWF_ACTION *Actions;
};

struct SWF_ACTIONSTOREREGISTER {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	UI8	Register;
};

/* v6 actions */
struct SWF_ACTIONINSTANCEOF {
	UI8	ActionCode;
};

struct SWF_ACTIONENUMERATE2 {
	UI8	ActionCode;
};

struct SWF_ACTIONSTRICTEQUALS {
	UI8	ActionCode;
};

struct SWF_ACTIONGREATER {
	UI8	ActionCode;
};

struct SWF_ACTIONSTRINGGREATER {
	UI8	ActionCode;
};

/* v7 actions */
struct REGISTERPARAM {
	UI8	Register;
	STRING	ParamName;
};

struct SWF_ACTIONDEFINEFUNCTION2 {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	STRING	FunctionName;
	UI16	NumParams;
	UI8	RegisterCount;
	UI16	PreloadParentFlag:1;
	UI16	PreloadRootFlag:1;
	UI16	SuppressSuperFlag:1;
	UI16	PreloadSuperFlag:1;
	UI16	SuppressArgumentsFlag:1;
	UI16	PreloadArgumentsFlag:1;
	UI16	SuppressThisFlag:1;
	UI16	PreloadThisFlag:1;
	UI16	Reserved:7;
	UI16	PreloadGlobalFlag:1;
	struct REGISTERPARAM	*Params;
	UI16	CodeSize;
	int	numActions;
	union SWF_ACTION *Actions;
};

struct SWF_ACTIONTRY {
	UI8	ActionCode;
	UI16	Length;
	UI32	Offset;
	UI8	Reserved:5;
	UI8	CatchInRegisterFlag:1;
	UI8	FinallyBlockFlag:1;
	UI8	CatchBlockFlag:1;
	UI16	TrySize;
	UI16	CatchSize;
	UI16	FinallySize;
	STRING	CatchName;
	UI8	CatchRegister;
	int	numTryActs;
	union SWF_ACTION *TryActs;
	int	numCatchActs;
	union SWF_ACTION *CatchActs;
	int	numFinallyActs;
	union SWF_ACTION *FinallyActs;
};

typedef union SWF_ACTION {
	/* v3 actions */
	struct SWF_ACTIONRECORD SWF_ACTIONRECORD;
	struct SWF_ACTIONGOTOFRAME SWF_ACTIONGOTOFRAME;
	struct SWF_ACTIONGETURL SWF_ACTIONGETURL;
	struct SWF_ACTIONNEXTFRAME SWF_ACTIONNEXTFRAME;
	struct SWF_ACTIONPREVIOUSFRAME SWF_ACTIONPREVIOUSFRAME;
	struct SWF_ACTIONPLAY SWF_ACTIONPLAY;
	struct SWF_ACTIONSTOP SWF_ACTIONSTOP;
	struct SWF_ACTIONTOGGLEQUALITY SWF_ACTIONTOGLEQUALITY;
	struct SWF_ACTIONSTOPSOUNDS SWF_ACTIONSTOPSOUNDS;
	struct SWF_ACTIONWAITFORFRAME SWF_ACTIONWAITFORFRAME;
	struct SWF_ACTIONSETTARGET SWF_ACTIONSETTARGET;
	struct SWF_ACTIONGOTOLABEL SWF_ACTIONGOTOLABEL;
	/* v4 actions */
	struct SWF_ACTIONPUSH SWF_ACTIONPUSH;
	struct SWF_ACTIONPOP SWF_ACTIONPOP;
	struct SWF_ACTIONADD SWF_ACTIONADD;
	struct SWF_ACTIONSUBTRACT SWF_ACTIONSUBTRACT;
	struct SWF_ACTIONMULTIPLY SWF_ACTIONMULTIPLY;
	struct SWF_ACTIONDIVIDE SWF_ACTIONDIVIDE;
	struct SWF_ACTIONEQUALS SWF_ACTIONEQUALS;
	struct SWF_ACTIONLESS SWF_ACTIONLESS;
	struct SWF_ACTIONAND SWF_ACTIONAND;
	struct SWF_ACTIONOR SWF_ACTIONOR;
	struct SWF_ACTIONNOT SWF_ACTIONNOT;
	struct SWF_ACTIONSTRINGEQUALS SWF_ACTIONSTRINGEQUALS;
	struct SWF_ACTIONSTRINGLENGTH SWF_ACTIONSTRINGLENGTH;
	struct SWF_ACTIONSTRINGADD SWF_ACTIONSTRINGADD;
	struct SWF_ACTIONSTRINGEXTRACT SWF_ACTIONSTRINGEXTRACT;
	struct SWF_ACTIONSTRINGLESS SWF_ACTIONSTRINGLESS;
	struct SWF_ACTIONMBSTRINGLEN SWF_ACTIONMBSTRINGLEN;
	struct SWF_ACTIONMBSTRINGEXTRACT SWF_ACTIONMBSTRINGEXTRACT;
	struct SWF_ACTIONTOINTEGER SWF_ACTIONTOINTEGER;
	struct SWF_ACTIONCHARTOASCII SWF_ACTIONCHARTOASCII;
	struct SWF_ACTIONASCIITOCHAR SWF_ACTIONASCIITOCHAR;
	struct SWF_ACTIONMBCHARTOASCII SWF_ACTIONMBCHARTOASCII;
	struct SWF_ACTIONMBASCIITOCHAR SWF_ACTIONMBASCIITOCHAR;
	struct SWF_ACTIONJUMP SWF_ACTIONJUMP;
	struct SWF_ACTIONIF SWF_ACTIONIF;
	struct SWF_ACTIONCALL SWF_ACTIONCALL;
	struct SWF_ACTIONGETVARIABLE SWF_ACTIONGETVARIABLE;
	struct SWF_ACTIONSETVARIABLE SWF_ACTIONSETVARIABLE;
	struct SWF_ACTIONGETURL2 SWF_ACTIONGETURL2;
	struct SWF_ACTIONGOTOFRAME2 SWF_ACTIONGOTOFRAME2;
	struct SWF_ACTIONSETTARGET2 SWF_ACTIONSETTARGET2;
	struct SWF_ACTIONGETPROPERTY SWF_ACTIONGETPROPERTY;
	struct SWF_ACTIONSETPROPERTY SWF_ACTIONSETPROPERTY;
	struct SWF_ACTIONCLONESPRITE SWF_ACTIONCLONESPRITE;
	struct SWF_ACTIONREMOVESPRITE SWF_ACTIONREMOVESPRITE;
	struct SWF_ACTIONSTARTDRAG SWF_ACTIONSTARTDRAG;
	struct SWF_ACTIONENDDRAG SWF_ACTIONENDDRAG;
	struct SWF_ACTIONWAITFORFRAME2 SWF_ACTIONWAITFORFRAME2;
	struct SWF_ACTIONTRACE SWF_ACTIONTRACE;
	struct SWF_ACTIONGETTIME SWF_ACTIONGETTIME;
	struct SWF_ACTIONRANDOMNUMBER SWF_ACTIONRANDOMNUMBER;
	/* v5 actions */
	struct SWF_ACTIONCALLFUNCTION SWF_ACTIONCALLFUNCTION;
	struct SWF_ACTIONCALLMETHOD SWF_ACTIONCALLMETHOD;
	struct SWF_ACTIONCONSTANTPOOL SWF_ACTIONCONSTANTPOOL;
	struct SWF_ACTIONDEFINEFUNCTION SWF_ACTIONDEFINEFUNCTION;
	struct SWF_ACTIONWITH SWF_ACTIONWITH;
	struct SWF_ACTIONSTOREREGISTER SWF_ACTIONSTOREREGISTER;
	/* v6 actions */
	struct SWF_ACTIONINSTANCEOF SWF_ACTIONINSTANCEOF;
	struct SWF_ACTIONENUMERATE2 SWF_ACTIONENUMERATE2;
	struct SWF_ACTIONSTRICTEQUALS SWF_ACTIONSTRICTEQUALS;
	struct SWF_ACTIONGREATER SWF_ACTIONGREATER;
	struct SWF_ACTIONSTRINGGREATER SWF_ACTIONSTRINGGREATER;
	/* v7 actions */
	struct SWF_ACTIONDEFINEFUNCTION2 SWF_ACTIONDEFINEFUNCTION2;
	struct SWF_ACTIONTRY SWF_ACTIONTRY;
} SWF_ACTION;



/* Types Flash Types */

typedef struct SWF_RGBA {
	UI8	red;
	UI8	green;
	UI8	blue;
	UI8	alpha;
} SWF_RGBA;

typedef struct SWF_RECT {
	UI8	Nbits:5;
	SB32	Xmin;
	SB32	Xmax;
	SB32	Ymin;
	SB32	Ymax;
} SWF_RECT;

typedef struct SWF_GRADIENTRECORD {
	UI8		Ratio;
	SWF_RGBA	Color;
} SWF_GRADIENTRECORD;

typedef struct SWF_GRADIENT {
	UI8	SpreadMode:2;
	UI8	InterpolationMode:2;
	UI8	NumGradients:4;
	SWF_GRADIENTRECORD	GradientRecords[15];
} SWF_GRADIENT;

typedef struct SWF_FOCALGRADIENT {
	UI8     SpreadMode:2;
	UI8     InterpolationMode:2;
	UI8     NumGradients:4;
	SWF_GRADIENTRECORD      GradientRecords[15];
	FIXED8 	FocalPoint;
} SWF_FOCALGRADIENT;

typedef struct SWF_MATRIX {
	UI8	HasScale:1;
	UI8	NScaleBits:5;
	FB32	ScaleX;
	FB32	ScaleY;
	UI8	HasRotate:1;
	UI8	NRotateBits:5;
	FB32	RotateSkew0;
	FB32	RotateSkew1;
	UI8	NTranslateBits:5;
	SB32	TranslateX;
	SB32	TranslateY;
} SWF_MATRIX;

typedef struct SWF_CXFORM {
	UI8	HasAddTerms:1;
	UI8	HasMultTerms:1;
	UI8	Nbits:4;
	SI32	RedMultTerm;
	SI32	GreenMultTerm;
	SI32	BlueMultTerm;
	SI32	RedAddTerm;
	SI32	GreenAddTerm;
	SI32	BlueAddTerm;
} SWF_CXFORM;

typedef struct SWF_CXFORMWITHALPHA {
	UI8	HasAddTerms:1;
	UI8	HasMultTerms:1;
	UI8	Nbits:4;
	SI32	RedMultTerm;
	SI32	GreenMultTerm;
	SI32	BlueMultTerm;
	SI32	AlphaMultTerm;
	SI32	RedAddTerm;
	SI32	GreenAddTerm;
	SI32	BlueAddTerm;
	SI32	AlphaAddTerm;
} SWF_CXFORMWITHALPHA;

typedef struct SWF_BLURFILTER {
	FIXED	BlurX;
	FIXED	BlurY;
	UI8	Passes:5;
	UI8	Reserved:3;
} SWF_BLURFILTER;

typedef struct SWF_BEVELFILTER {
	SWF_RGBA	ShadowColor;
	SWF_RGBA	HighlightColor;
	FIXED		BlurX;
	FIXED		BlurY;
	FIXED		Angle;
	FIXED		Distance;
	FIXED8		Strength;
	UI8		InnerShadow:1;
	UI8		Kockout:1;
	UI8		CompositeSource:1;
	UI8		OnTop:1;
	UI8		Passes:4;
} SWF_BEVELFILTER;

typedef struct SWF_GRADIENTFILTER
{
	UI8		NumColors;
	SWF_RGBA*	GradientColors;
	UI8*		GradientRatio;
	FIXED		BlurX;
	FIXED		BlurY;
	FIXED		Angle;
	FIXED		Distance;
	FIXED8		Strength;
	UI8		InnerShadow:1;
	UI8		Kockout:1;
	UI8		CompositeSource:1;
	UI8		OnTop:1;
	UI8		Passes:4;
} SWF_GRADIENTFILTER;
	
typedef struct SWF_GLOWFILTER {
	SWF_RGBA 	GlowColor;
	FIXED		BlurX;
	FIXED		BlurY;
	FIXED8		Strength;
	UI8		InnerGlow:1;
	UI8		Kockout:1;
	UI8		CompositeSource:1;
	UI8		Passes:5;
} SWF_GLOWFILTER;

typedef struct SWF_CONVOLUTIONFILTER {
	UI8		MatrixX;
	UI8		MatrixY;
	FLOAT		Divisor;
	FLOAT		Bias;
	FLOAT*		Matrix;
	SWF_RGBA	DefaultColor;
	UI8		Reserved:6;
	UI8		Clamp:1;
	UI8		PreserveAlpha:1;
} SWF_CONVOLUTIONFILTER;

typedef struct SWF_COLORMATRIXFILTER
{
	FLOAT Matrix[20];
} SWF_COLORMATRIXFILTER;

typedef struct SWF_DROPSHADOWFILTER {
	SWF_RGBA	DropShadowColor;
	FIXED		BlurX;
	FIXED		BlurY;
	FIXED		Angle;
	FIXED		Distance;
	FIXED8		Strength;
	UI8		InnerShadow:1;
	UI8		Kockout:1;
	UI8		CompositeSource:1;
	UI8		Passes:5;
} SWF_DROPSHADOWFILTER;

enum
{
        FILTER_DROPSHADOW,
        FILTER_BLUR,
        FILTER_GLOW,
        FILTER_BEVEL,
        FILTER_GRADIENTGLOW,
        FILTER_CONVOLUTION,
        FILTER_COLORMATRIX,
        FILTER_GRADIENTBEVEL
};

typedef struct SWF_FILTER {
	UI8 	FilterId;
	union {
		SWF_DROPSHADOWFILTER dropShadow;
		SWF_BLURFILTER blur;
		SWF_GLOWFILTER glow;
		SWF_BEVELFILTER bevel;
		SWF_GRADIENTFILTER gradientGlow;
		SWF_CONVOLUTIONFILTER convolution;
		SWF_COLORMATRIXFILTER colorMatrix;
		SWF_GRADIENTFILTER gradientBevel;
	} filter;
} SWF_FILTER;

typedef struct SWF_FILTERLIST {
	UI8 		NumberOfFilters;
	SWF_FILTER 	*Filter;
} SWF_FILTERLIST;

typedef struct SWF_BUTTONRECORD {
	UI8	ButtonReserved:2;
	UI8	ButtonHasBlendMode:1;
	UI8	ButtonHasFilterList:1;
	UI8	ButtonStateHitTest:1;
	UI8	ButtonStateDown:1;
	UI8	ButtonStateOver:1;
	UI8	ButtonStateUp:1;
	UI16	CharacterId;
	UI16	PlaceDepth;
	SWF_MATRIX	PlaceMatrix;
	SWF_CXFORMWITHALPHA	ColorTransform;
	SWF_FILTERLIST	FilterList;	
	UI8 	BlendMode;
} SWF_BUTTONRECORD;

typedef struct SWF_BUTTONCONDACTION {
	UI16	CondActionSize;
	UI16	CondIdleToOverDown:1;
	UI16	CondOutDownToIdle:1;
	UI16	CondOutDownToOverDown:1;
	UI16	CondOverDownToOutDown:1;
	UI16	CondOverDownToOverUp:1;
	UI16	CondOverUpToOverDown:1;
	UI16	CondOverUpToIdle:1;
	UI16	CondIdleToOverUp:1;
	UI16	CondKeyPress:7;
	UI16	CondOverDownToIdle:1;
	int	numActions;
	SWF_ACTION *Actions;
} SWF_BUTTONCONDACTION;

typedef struct SWF_CLIPEVENTFLAGS {
	UI8	ClipEventKeyUp:1;
	UI8	ClipEventKeyDown:1;
	UI8	ClipEventMouseUp:1;
	UI8	ClipEventMouseDown:1;
	UI8	ClipEventMouseMove:1;
	UI8	ClipEventUnload:1;
	UI8	ClipEventEnterFrame:1;
	UI8	ClipEventLoad:1;
	UI8	ClipEventDragOver:1;
	UI8	ClipEventRollOut:1;
	UI8	ClipEventRollOver:1;
	UI8	ClipEventReleaseOutside:1;
	UI8	ClipEventRelease:1;
	UI8	ClipEventPress:1;
	UI8	ClipEventInitialize:1;
	UI8	ClipEventData:1;
	UI8	Reserved:5;
	UI8	ClipEventConstruct:1;
	UI8	ClipEventKeyPress:1;
	UI8	ClipEventDragOut:1;
	UI8	Reserved2:8;
} SWF_CLIPEVENTFLAGS;

typedef struct SWF_CLIPACTIONRECORD {
	SWF_CLIPEVENTFLAGS	EventFlag;
	UI32	ActionRecordSize;
	UI8	KeyCode;
	int     numActions;
	SWF_ACTION	*Actions;
} SWF_CLIPACTIONRECORD;

typedef struct SWF_CLIPACTIONS {
	UI16	Reserved;
	SWF_CLIPEVENTFLAGS	AllEventFlags;
	int	NumClipRecords;
	SWF_CLIPACTIONRECORD	*ClipActionRecords;
	UI32	ClipActionEndFlag;
} SWF_CLIPACTIONS;

typedef struct SWF_FILLSTYLE {
	UI8		FillStyleType;
	SWF_RGBA	Color;
	SWF_MATRIX	GradientMatrix;
	SWF_GRADIENT	Gradient;
	SWF_FOCALGRADIENT FocalGradient;
	UI16		BitmapId;
	SWF_MATRIX	BitmapMatrix;
} SWF_FILLSTYLE;

typedef struct SWF_FILLSTYLEARRAY {
	UI8	FillStyleCount;
	UI16	FillStyleCountExtended;
	SWF_FILLSTYLE	*FillStyles;
} SWF_FILLSTYLEARRAY;

typedef struct SWF_LINESTYLE {
	UI16		Width;
	SWF_RGBA	Color;
} SWF_LINESTYLE;

typedef struct SWF_LINESTYLE2 {
	UI16		Width;
	UI8		StartCapStyle:2;
	UI8		JoinStyle:2;
	UI8		HasFillFlag:1;
	UI8		NoHScaleFlag:1;
	UI8		NoVScaleFlag:1;
	UI8		PixelHintingFlag:1;
	UI8		Reserved:5;
	UI8		NoClose:1;
	UI8		EndCapStyle:2;
	UI16		MiterLimitFactor;
	SWF_RGBA	Color;
	SWF_FILLSTYLE	FillType;
} SWF_LINESTYLE2;

typedef struct SWF_LINESTYLEARRAY {
	UI16	LineStyleCount;
	SWF_LINESTYLE	*LineStyles;
	SWF_LINESTYLE2	*LineStyles2;
} SWF_LINESTYLEARRAY;

typedef struct SWF_ENDSHAPERECORD {
	UI16	TypeFlag:1;
	UI16	EndOfShape:5;
} SWF_ENDSHAPERECORD;

typedef struct SWF_STYLECHANGERECORD {
	UI16	TypeFlag:1;
	UI16	StateNewStyles:1;
	UI16	StateLineStyle:1;
	UI16	StateFillStyle1:1;
	UI16	StateFillStyle0:1;
	UI16	StateMoveTo:1;
	UI16	MoveBits:5;
	SB32	MoveDeltaX;
	SB32	MoveDeltaY;
	UB32	FillStyle0;
	UB32	FillStyle1;
	UB32	LineStyle;
	SWF_FILLSTYLEARRAY	FillStyles;
	SWF_LINESTYLEARRAY	LineStyles;
	UI8	NumFillBits:4;
	UI8	NumLineBits:4;
} SWF_STYLECHANGERECORD;

typedef struct SWF_CURVEDEDGERECORD {
	UI16	TypeFlag:1;
	UI16	StraightFlag:1;
	UI16	NumBits:4;
	SB32	ControlDeltaX;
	SB32	ControlDeltaY;
	SB32	AnchorDeltaX;
	SB32	AnchorDeltaY;
} SWF_CURVEDEDGERECORD;

typedef struct SWF_STRAIGHTEDGERECORD {
	UI16	TypeFlag:1;
	UI16	StraightEdge:1;
	UI16	NumBits:4;
	UI16	GeneralLineFlag:1;
	SB32	DeltaX;
	SB32	DeltaY;
	SI8	VertLineFlag:1;
	SB32	VLDeltaX;
	SB32	VLDeltaY;
} SWF_STRAIGHTEDGERECORD;

typedef union SWF_SHAPERECORD {
	SWF_ENDSHAPERECORD	EndShape;
	SWF_STYLECHANGERECORD	StyleChange;
	SWF_STRAIGHTEDGERECORD	StraightEdge;
	SWF_CURVEDEDGERECORD	CurvedEdge;
} SWF_SHAPERECORD;

typedef struct SWF_MORPHGRADIENTRECORD {
	UI8		StartRatio;
	SWF_RGBA	StartColor;
	UI8		EndRatio;
	SWF_RGBA	EndColor;
} SWF_MORPHGRADIENTRECORD;

typedef struct SWF_MORPHGRADIENT {
	UI8	NumGradients;
	SWF_MORPHGRADIENTRECORD	GradientRecords[8];
} SWF_MORPHGRADIENT;

typedef struct SWF_MORPHFILLSTYLE {
	UI8		FillStyleType;
	SWF_RGBA	StartColor;
	SWF_RGBA	EndColor;
	SWF_MATRIX	StartGradientMatrix;
	SWF_MATRIX	EndGradientMatrix;
	SWF_MORPHGRADIENT	Gradient;
	UI16		BitmapId;
	SWF_MATRIX	StartBitmapMatrix;
	SWF_MATRIX	EndBitmapMatrix;
} SWF_MORPHFILLSTYLE;

typedef struct SWF_MORPHFILLSTYLES {
	UI8	FillStyleCount;
	UI16	FillStyleCountExtended;
	SWF_MORPHFILLSTYLE	*FillStyles;
} SWF_MORPHFILLSTYLES;


typedef struct SWF_MORPHLINESTYLE {
	UI16		StartWidth;
	UI16		EndWidth;
	SWF_RGBA	StartColor;
	SWF_RGBA	EndColor;
} SWF_MORPHLINESTYLE;

typedef struct SWF_MORPHLINESTYLE2 {
	UI16		StartWidth;
	UI16		EndWidth;
	UI16		StartCapStyle:2;
	UI16		JoinStyle:2;
	UI16		HasFillFlag:2;
	UI16		NoHScaleFlag:1;
	UI16		NoVScaleFlag:1;
	UI16		PixelHintingFlag:1;
	UI16		Reserved:5;
	UI16		NoClose:1;
	UI16		EndCapStyle:2;
	UI16		MiterLimitFactor;
	SWF_RGBA	StartColor;
	SWF_RGBA	EndColor;
	SWF_MORPHFILLSTYLE FillType;
} SWF_MORPHLINESTYLE2;

typedef struct SWF_MORPHLINESTYLES {
	UI8	LineStyleCount;
	UI16	LineStyleCountExtended;
	SWF_MORPHLINESTYLE	*LineStyles;
	SWF_MORPHLINESTYLE2	*LineStyles2;
} SWF_MORPHLINESTYLES;

typedef struct SWF_SHAPE {
	UI8	NumFillBits:4;
	UI8	NumLineBits:4;
	int	NumShapeRecords;
	SWF_SHAPERECORD	*ShapeRecords;
} SWF_SHAPE;

typedef struct SWF_SHAPEWITHSTYLE {
	SWF_FILLSTYLEARRAY	FillStyles;
	SWF_LINESTYLEARRAY	LineStyles;
	UI8	NumFillBits:4;
	UI8	NumLineBits:4;
	int	NumShapeRecords;
	SWF_SHAPERECORD	*ShapeRecords;
} SWF_SHAPEWITHSTYLE;

typedef struct SWF_GLYPHENTRY {
	UI32	*GlyphIndex;
	UI32	*GlyphAdvance;
} SWF_GLYPHENTRY;

typedef struct SWF_TEXTRECORD {
	UI8	TextRecordType:1;
	UI8	StyleFlagsReserved:3;
	UI8	StyleFlagHasFont:1;
	UI8	StyleFlagHasColor:1;
	UI8	StyleFlagHasYOffset:1;
	UI8	StyleFlagHasXOffset:1;
	UI16	FontID;
	SWF_RGBA	TextColor;
	SI16	XOffset;
	SI16	YOffset;
	UI16	TextHeight;
	UI8	GlyphCount;
	SWF_GLYPHENTRY *GlyphEntries;
} SWF_TEXTRECORD;

typedef struct SWF_SOUNDENVELOPE {
	UI32	Pos44;
	UI16	LeftLevel;
	UI16	RightLevel;
} SWF_SOUNDENVELOPE;

typedef struct SWF_SOUNDINFO {
	UI8	Reserved:2;
	UI8	SyncStop:1;
	UI8	SyncNoMultiple:1;
	UI8	HasEnvelope:1;
	UI8	HasLoops:1;
	UI8	HasOutPoint:1;
	UI8	HasInPoint:1;
	UI32	InPoint;
	UI32	OutPoint;
	UI16	LoopCount;
	UI8	EnvPoints;
	SWF_SOUNDENVELOPE *EnvelopeRecords;
} SWF_SOUNDINFO;



/* Types to represent Blocks */

struct SWF_CHARACTERSET
{
  int chid;
};

struct SWF_DEFINEBITS
{
  UI16 CharacterID;
  int JPEGDataSize;
  UI8 *JPEGData;
};

struct SWF_DEFINEBITSJPEG2
{
  UI16 CharacterID;
  int JPEGDataSize;
  UI8 *JPEGData;
};

struct SWF_DEFINEBITSJPEG3
{
  UI16 CharacterID;
  UI32 AlphaDataOffset;
  UI8 *JPEGData;
  int AlphaDataSize;
  UI8 *BitmapAlphaData;
};

// XXX: obsolete
struct SWF_DEFINEBITSPTR
{
  int chid;
};

struct SWF_DEFINEBUTTON
{
  UI16 ButtonId;
  SWF_BUTTONRECORD *Characters;
  int numCharacters;
  UI8 CharacterEndFlag;
  int numActions;
  SWF_ACTION *Actions;
  UI8 ActionEndFlag;
};

struct SWF_DEFINEBUTTON2
{
  UI16 Buttonid;
  UI8 ReservedFlags:7;
  UI8 TrackAsMenu:1;
  UI16 ActionOffset;
  int numCharacters;
  SWF_BUTTONRECORD *Characters;
  UI8 CharacterEndFlag;
  int numActions;
  SWF_BUTTONCONDACTION *Actions;
};

struct SWF_DEFINEBUTTONCXFORM
{
  UI16 ButtonId;
  SWF_CXFORM ButtonColorTransform;
};

struct SWF_DEFINEBUTTONSOUND
{
  UI16 CharacterID;
  UI16 ButtonSoundChar0;
  SWF_SOUNDINFO ButtonSoundInfo0;
  UI16 ButtonSoundChar1;
  SWF_SOUNDINFO ButtonSoundInfo1;
  UI16 ButtonSoundChar2;
  SWF_SOUNDINFO ButtonSoundInfo2;
  UI16 ButtonSoundChar3;
  SWF_SOUNDINFO ButtonSoundInfo3;
};


// XXX: obsolete ?
struct SWF_DEFINECOMMANDOBJ
{
  int chid;
};

struct SWF_DEFINEEDITTEXT
{
  UI16 CharacterID;
  SWF_RECT Bounds;
  UI8	HasText:1;
  UI8	WordWrap:1;
  UI8	Multiline:1;
  UI8	Password:1;
  UI8	ReadOnly:1;
  UI8	HasTextColor:1;
  UI8	HasMaxLength:1;
  UI8	HasFont:1;
  UI8	HasFontClass:1;
  UI8	AutoSize:1;
  UI8	HasLayout:1;
  UI8	NoSelect:1;
  UI8	Border:1;
  UI8	WasStatic:1;
  UI8	HTML:1;
  UI8	UseOutlines:1;
  UI16	FontID;
  STRING FontClass;
  UI16	FontHeight;
  SWF_RGBA	TextColor;
  UI16	MaxLength;
  UI8	Align;
  UI16	LeftMargin;
  UI16	RightMargin;
  UI16	Indent;
  UI16	Leading;
  STRING	VariableName;
  STRING	InitialText;
};

struct SWF_DEFINEFONT
{
  UI16 FontID;
  int NumGlyphs;
  UI16 *OffsetTable;
  SWF_SHAPE *GlyphShapeTable;
};

struct SWF_KERNINGRECORD
{
  UI16 FontKerningCode1;
  UI16 FontKerningCode2;
  SI16 FontKerningAdjustment;
};

struct SWF_DEFINEFONT2
{
  UI16 FontID;
  UI8 FontFlagsHasLayout:1;
  UI8 FontFlagsShiftJis:1;
  UI8 FontFlagsSmallText:1;
  UI8 FontFlagsFlagANSI:1;
  UI8 FontFlagsWideOffsets:1;
  UI8 FontFlagsWideCodes:1;
  UI8 FontFlagsFlagsItalics:1;
  UI8 FontFlagsFlagsBold:1;
  LANGCODE LanguageCode;
  UI8 FontNameLen;
  STRING FontName;
  UI16 NumGlyphs;
  union {
    UI16 *UI16;
    UI32 *UI32;
  } OffsetTable;
  union {
    UI16 UI16;
    UI32 UI32;
  } CodeTableOffset;
  SWF_SHAPE *GlyphShapeTable;
  int *CodeTable;
  SI16 FontAscent;
  SI16 FontDecent;
  SI16 FontLeading;
  SI16 *FontAdvanceTable;
  SWF_RECT *FontBoundsTable;
  UI16 KerningCount;
  struct SWF_KERNINGRECORD *FontKerningTable;
};

struct SWF_DEFINEFONT3
{
  UI16 FontID;
  UI8 FontFlagsHasLayout:1;
  UI8 FontFlagsShiftJis:1;
  UI8 FontFlagsSmallText:1;
  UI8 FontFlagsFlagANSI:1;
  UI8 FontFlagsWideOffsets:1;
  UI8 FontFlagsWideCodes:1;
  UI8 FontFlagsFlagsItalics:1;
  UI8 FontFlagsFlagsBold:1;
  LANGCODE LanguageCode;
  UI8 FontNameLen;
  STRING FontName;
  UI16 NumGlyphs;
  union {
    UI16 *UI16;
    UI32 *UI32;
  } OffsetTable;
  union {
    UI16 UI16;
    UI32 UI32;
  } CodeTableOffset;
  SWF_SHAPE *GlyphShapeTable;
  UI16 *CodeTable;
  SI16 FontAscent;
  SI16 FontDecent;
  SI16 FontLeading;
  SI16 *FontAdvanceTable;
  SWF_RECT *FontBoundsTable;
  UI16 KerningCount;
  struct SWF_KERNINGRECORD *FontKerningTable;
};

struct SWF_DEFINEFONTINFO
{
  UI16 FontID;
  UI8 FontNameLen;
  STRING FontName;
  UI8 FontFlagsReserved:2;
  UI8 FontFlagsSmallText:1;
  UI8 FontFlagsShiftJIS:1;
  UI8 FontFlagsANSI:1;
  UI8 FontFlagsItalic:1;
  UI8 FontFlagsBold:1;
  UI8 FontFlagsWideCodes:1;
  int nGlyph;
  UI16 *CodeTable;
};

struct SWF_DEFINEFONTINFO2
{
  UI16 FontID;
  UI8 FontNameLen;
  STRING FontName;
  UI8 FontFlagsReserved:2;
  UI8 FontFlagsSmallText:1;
  UI8 FontFlagsShiftJIS:1;
  UI8 FontFlagsANSI:1;
  UI8 FontFlagsItalic:1;
  UI8 FontFlagsBold:1;
  UI8 FontFlagsWideCodes:1;
  LANGCODE LanguageCode;
  int nGlyph;
  UI16 *CodeTable;
};

struct SWF_ZONEDATA
{
  FLOAT16 AlignmentCoordinate;
  FLOAT16 Range;
};

struct SWF_ZONERECORD
{
  UI8 NumZoneData;
  struct SWF_ZONEDATA *ZoneData;
  UI8 ZoneMaskX:1;
  UI8 ZoneMaskY:1;
  UI8 Reserved:6;
};

struct SWF_DEFINEFONTALIGNZONES
{
  UI16 FontID;
  UI8 CSMTableHint:2;
  UI8 Reserved:6;
  int GlyphCount;
  struct SWF_ZONERECORD *ZoneTable;
};

struct SWF_CSMTEXTSETTINGS
{
  UI16 TextID;
  UI8 UseFlashType:2;
  UI8 GridFit:3;
  UI8 Reserved:3;
  FB32 Thickness;
  FB32 Sharpness;
  UI8 Reserved2;
};

struct SWF_DEFINEFONTNAME
{
  UI16 FontId;
  STRING FontName;
  STRING FontCopyright;
};

struct SWF_DEFINELOSSLESS
{
  UI16 CharacterID;
  UI8  BitmapFormat;
  UI16 BitmapWidth;
  UI16 BitmapHeight;
  UI8  BitmapColorTableSize;
  UI8  *ZlibBitmapData;
};

struct SWF_DEFINELOSSLESS2
{
  UI16 CharacterID;
  UI8  BitmapFormat;
  UI16 BitmapWidth;
  UI16 BitmapHeight;
  UI8  BitmapColorTableSize;
  UI8  *ZlibBitmapData;
};

struct SWF_DEFINEMORPHSHAPE
{
  UI16 CharacterID;
  SWF_RECT StartBounds;
  SWF_RECT EndBounds;
  UI32 Offset;
  SWF_MORPHFILLSTYLES MorphFillStyles;
  SWF_MORPHLINESTYLES MorphLineStyles;
  SWF_SHAPE StartEdges;
  SWF_SHAPE EndEdges;
};

struct SWF_DEFINEMORPHSHAPE2
{
  UI16 CharacterID;
  SWF_RECT StartBounds;
  SWF_RECT EndBounds;
  SWF_RECT StartEdgeBounds;
  SWF_RECT EndEdgeBounds;
  UI8 Reserved:6;
  UI8 UsesNonScalingStrokes:1;
  UI8 UsesScalingStrokes:1;
  UI32 Offset;
  SWF_MORPHFILLSTYLES MorphFillStyles;
  SWF_MORPHLINESTYLES MorphLineStyles;
  SWF_SHAPE StartEdges;
  SWF_SHAPE EndEdges;
};


struct SWF_DEFINESHAPE
{
  UI16 ShapeID;
  SWF_RECT ShapeBounds;
  SWF_SHAPEWITHSTYLE Shapes;
};

struct SWF_DEFINESHAPE2
{
  UI16 ShapeID;
  SWF_RECT ShapeBounds;
  SWF_SHAPEWITHSTYLE Shapes;
};

struct SWF_DEFINESHAPE3
{
  UI16 ShapeID;
  SWF_RECT ShapeBounds;
  SWF_SHAPEWITHSTYLE Shapes;
};

struct SWF_DEFINESHAPE4
{
  UI16 ShapeID;
  SWF_RECT ShapeBounds;
  SWF_RECT EdgeBounds;
  UI8 Reserved:6;
  UI8 UsesNonScalingStrokes:1;
  UI8 UsesScalingStrokes:1;
  SWF_SHAPEWITHSTYLE Shapes;
};

struct SWF_DEFINESPRITE
{
  UI16 SpriteId;
  UI16 FrameCount;
  UI16 BlockCount;
  UI16 *tagTypes;
  struct SWF_Parserstruct_s **Tags;
};

struct SWF_DEFINETEXT
{
  UI16 CharacterID;
  SWF_RECT TextBounds;
  SWF_MATRIX TextMatrix;
  UI8	GlyphBits;
  UI8	AdvanceBits;
  int numTextRecords;
  SWF_TEXTRECORD *TextRecords;
};

struct SWF_DEFINETEXT2
{
  UI16 CharacterID;
  SWF_RECT TextBounds;
  SWF_MATRIX TextMatrix;
  UI8	GlyphBits;
  UI8	AdvanceBits;
  int numTextRecords;
  SWF_TEXTRECORD *TextRecords;
};

// XXX obsolete ?
struct SWF_DEFINETEXTFORMAT
{
  int chid;
};

// XXX obsolete ?
struct SWF_DEFINEVIDEO
{
  int chid;
};

struct SWF_DEFINEVIDEOSTREAM
{
  UI16 CharacterID;
  UI16 NumFrames;
  UI16 Width;
  UI16 Height;
  UI8 Reserved:5;
  UI8 VideoFlagsDeblocking:2;
  UI8 VideoFlagsSmoothing:1;
  UI8 CodecID;
};

struct SWF_DOACTION
{
  int  numActions;
  SWF_ACTION *Actions;
};

struct SWF_ENABLEDEBUGGER
{
  STRING Password;
};

struct SWF_ENABLEDEBUGGER2
{
  UI16 Reserved;
  STRING Password;
};

struct SWF_END
{
#if defined(__SUNPRO_C)
int empty; // See http://bugs.libming.org/show_bug.cgi?id=103
#endif
};

struct SWF_EXPORTASSETS
{
  UI16 Count;
  UI16 *Tags;
  STRING *Names;
};

// XXX obsolete ?
struct SWF_FONTREF
{
  int chid;
};

struct SWF_FRAMELABEL
{
  STRING	Name;
  UI8		IsAnchor;
};

// XXX obsolete ?
struct SWF_FRAMETAG
{
  int chid;
};

// XXX obsolete ?
struct SWF_FREEALL
{
  int chid;
};

// XXX obsolete ?
struct SWF_FREECHARACTER
{
  int chid;
};

// XXX obsolete ?
struct SWF_GENCOMMAND
{
  int chid;
};

struct SWF_IMPORTASSETS
{
  STRING URL;
  UI16	Count;
  UI16 *Tags;
  STRING *Names;
};

struct SWF_IMPORTASSETS2
{
  STRING URL;
  UI8 Reserved;
  UI8 Reserved2;
  UI16	Count;
  UI16 *Tags;
  STRING *Names;
};


struct SWF_JPEGTABLES
{
  int JPEGDataSize;
  UI8 *JPEGData;
};

struct SWF_NAMECHARACTER
{
  UI16 Id;
  STRING Name;
};

// XXX: obsolete?
struct SWF_PATHSAREPOSTSCRIPT
{
  int chid;
};

struct SWF_PLACEOBJECT
{
  UI16 CharacterId;
  UI16 Depth;
  SWF_MATRIX Matrix;
  SWF_CXFORMWITHALPHA ColorTransform;
};

struct SWF_PLACEOBJECT2
{
  UI8 PlaceFlagHasClipActions:1;
  UI8 PlaceFlagHasClipDepth:1;
  UI8 PlaceFlagHasName:1;
  UI8 PlaceFlagHasRatio:1;
  UI8 PlaceFlagHasColorTransform:1;
  UI8 PlaceFlagHasMatrix:1;
  UI8 PlaceFlagHasCharacter:1;
  UI8 PlaceFlagMove:1;
  UI16 Depth;
  UI16 CharacterId;
  SWF_MATRIX Matrix;
  SWF_CXFORMWITHALPHA ColorTransform;
  UI16 Ratio;
  STRING Name;
  UI16 ClipDepth;
  SWF_CLIPACTIONS ClipActions;
};

struct SWF_PLACEOBJECT3
{
  UI8 PlaceFlagHasClipActions:1;
  UI8 PlaceFlagHasClipDepth:1;
  UI8 PlaceFlagHasName:1;
  UI8 PlaceFlagHasRatio:1;
  UI8 PlaceFlagHasColorTransform:1;
  UI8 PlaceFlagHasMatrix:1;
  UI8 PlaceFlagHasCharacter:1;
  UI8 PlaceFlagMove:1;
  UI8 Reserved:3;
  UI8 PlaceFlagHasImage:1;
  UI8 PlaceFlagHasClassName:1;
  UI8 PlaceFlagHasCacheAsBitmap:1;
  UI8 PlaceFlagHasBlendMode:1;
  UI8 PlaceFlagHasFilterList:1;
  UI16 Depth;
  STRING ClassName;
  UI16 CharacterId;
  SWF_MATRIX Matrix;
  SWF_CXFORMWITHALPHA ColorTransform;
  UI16 Ratio;
  STRING Name;
  UI16 ClipDepth;
  SWF_FILTERLIST SurfaceFilterList;
  UI8 BlendMode;
  SWF_CLIPACTIONS ClipActions;
};

struct SWF_PREBUILT
{
  int chid;
};

struct SWF_PREBUILTCLIP
{
  int chid;
};

struct SWF_PROTECT
{
  STRING Password;
};

struct SWF_REMOVEOBJECT
{
  UI16 CharacterId;
  UI16 Depth;
};

struct SWF_REMOVEOBJECT2
{
  UI16 Depth;
};

struct SWF_SERIALNUMBER
{ 
  UI32 Id;
  UI32 Edition;
  UI8 Major;
  UI8 Minor;
  UI32 BuildL;
  UI32 BuildH;
  UI32 TimestampL;
  UI32 TimestampH;
};

struct SWF_SETBACKGROUNDCOLOR
{
  SWF_RGBA rgb;
};

struct SWF_SHOWFRAME
{
  int chid;
};

struct MP3STREAMSOUNDDATA
{
  UI16 SampleCount;
  SI16 SeekSamples;
  UI8 *frames;
};

struct SWF_SOUNDSTREAMBLOCK
{
  union {
    struct MP3STREAMSOUNDDATA mp3;
    UI8 *data;
  } StreamData;
};

struct SWF_SOUNDSTREAMHEAD
{
  UI16	Reserved:4;
  UI16	PlaybackSoundRate:2;
  UI16	PlaybackSoundSize:1;
  UI16	PlaybackSoundType:1;
  UI16	StreamSoundCompression:4;
  UI16	StreamSoundRate:2;
  UI16	StreamSoundSize:1;
  UI16	StreamSoundType:1;
  UI16	StreamSoundSampleCount;
  UI16	LatencySeek;
};

struct SWF_SOUNDSTREAMHEAD2
{
  UI16	Reserved:4;
  UI16	PlaybackSoundRate:2;
  UI16	PlaybackSoundSize:1;
  UI16	PlaybackSoundType:1;
  UI16	StreamSoundCompression:4;
  UI16	StreamSoundRate:2;
  UI16	StreamSoundSize:1;
  UI16	StreamSoundType:1;
  UI16	StreamSoundSampleCount;
  UI16	LatencySeek;
};

struct SWF_DEFINESOUND
{
  UI16 SoundId;
  UI8  SoundFormat:4;
  UI8  SoundRate:2;
  UI8  SoundSize:1;
  UI8  SoundType:1;
  UI32  SoundSampleCount;
  union {
    struct MP3STREAMSOUNDDATA mp3;
    UI8 *data;
  } SoundData;
};

struct SWF_STARTSOUND
{
  UI16 SoundId;
  SWF_SOUNDINFO	SoundInfo;
};

struct SWF_STARTSOUND2
{
  STRING SoundClassName;
  SWF_SOUNDINFO	SoundInfo;
};


// XXX: obsolete?
struct SWF_SYNCFRAME
{
  int chid;
};

struct SWF_INITACTION
{
  UI16 SpriteId;
  int  numActions;
  SWF_ACTION *Actions;
};

struct SWF_VIDEOFRAME
{
  UI16 StreamID;
  UI16 FrameNum;
  UI8 *VideoData;
};

struct SWF_REFLEX
{
  UI8 rfx[3];
};

struct SWF_FILEATTRIBUTES
{
	UI8	Reserved:3;
	UI8	HasMetadata:1;
	UI8	ActionScript3:1;
	UI8	Reserved2:2;
	UI8	UseNetwork:1;
	UI16	Reserved3;
	UI8	Reserved4;
};

struct SWF_METADATA
{
	STRING Metadata;
};

struct SWF_SCRIPTLIMITS
{
	UI16 MaxRecursionDepth;
	UI16 ScriptTimeoutSeconds;
};

struct SWF_DEFINESCALINGGRID
{
	UI16 CharacterId;
	SWF_RECT Splitter;
};

struct SWF_SETTABINDEX
{
	UI16 Depth;
	UI16 TabIndex;
};

struct ABC_OPTION_DETAIL
{
	U30 Val;
	UI8 Kind;
};

struct ABC_OPTION_INFO
{
	U30 OptionCount;
	struct ABC_OPTION_DETAIL *Option;	
};

struct ABC_PARAM_INFO
{
	U30 *ParamNames;
};

struct ABC_METHOD_INFO
{
	U30 ParamCount;
	U30 ReturnType;
	U30 *ParamType;
	U30 Name;
	UI8 Flags;
	struct ABC_OPTION_INFO Options;
	struct ABC_PARAM_INFO ParamNames;
};	

struct ABC_STRING_INFO
{
	U30 Size;
	UI8 *UTF8String;
};

struct ABC_NS_INFO
{
	UI8 Kind;
	U30 Name;
};

struct ABC_NS_SET_INFO
{
	U30 Count;
	U30 *NS;
};

struct ABC_QNAME
{
	U30 NS;
	U30 Name;
};

struct ABC_RTQNAME
{
	U30 Name;
};

struct ABC_RTQNAME_L
{
#if defined(__SUNPRO_C)
int empty; // See http://bugs.libming.org/show_bug.cgi?id=103
#endif
};

struct ABC_MULTINAME
{
	U30 Name;
	U30 NSSet;
};

struct ABC_MULTINAME_L
{
	U30 NSSet;
};

struct ABC_MULTINAME_INFO
{
	UI8 Kind;
	union {
		struct ABC_QNAME QName;
		struct ABC_QNAME QNameA;
		struct ABC_RTQNAME RTQName;
		struct ABC_RTQNAME RTQNameA;
		struct ABC_RTQNAME_L RTQNameL;
		struct ABC_RTQNAME_L RTQNameLA;
		struct ABC_MULTINAME Multiname;
		struct ABC_MULTINAME MultinameA;
		struct ABC_MULTINAME_L MultinameL;
		struct ABC_MULTINAME_L MultinameLA;
	} Data;

};

struct ABC_CONSTANT_POOL
{
	U30 IntCount;
	S32 *Integers;
	U30 UIntCount;
	U32 *UIntegers;
	U30 DoubleCount;
	DOUBLE *Doubles;
	U30 StringCount;
	struct ABC_STRING_INFO *Strings;
	U30 NamespaceCount;
	struct ABC_NS_INFO *Namespaces;
	U30 NamespaceSetCount;
	struct ABC_NS_SET_INFO *NsSets;
	U30 MultinameCount;
	struct ABC_MULTINAME_INFO *Multinames;
};

struct ABC_ITEM_INFO
{
	U30 Key;
	U30 Value;
};

struct ABC_METADATA_INFO
{
	U30 Name;
	U30 ItemCount;
	struct ABC_ITEM_INFO *Items;
};


struct ABC_TRAIT_SLOT
{
	U30 SlotId;
	U30 TypeName;
	U30 VIndex;
	UI8 VKind;

};

struct ABC_TRAIT_CLASS
{
	U30 SlotId;
	U30 ClassIndex;
};

struct ABC_TRAIT_FUNCTION
{
	U30 SlotId;
	U30 Function;
};

struct ABC_TRAIT_METHOD
{
	U30 DispId;
	U30 Method;
};

struct ABC_TRAITS_INFO
{
	U30 Name;
	UI8 Kind;
	UI8 Attr;
	union {
		struct ABC_TRAIT_SLOT Slot;
		struct ABC_TRAIT_CLASS Class;
		struct ABC_TRAIT_FUNCTION Function;
		struct ABC_TRAIT_METHOD Method;
	} Data;
	U30 MetadataCount;
	U30 *Metadata;
};

struct ABC_INSTANCE_INFO
{
	U30 Name;
	U30 SuperName;
	UI8 Flags;
	U30 ProtectedNs;
	U30 InterfaceCount;
	U30 *Interfaces;
	U30 IInit;
	U30 TraitCount;
	struct ABC_TRAITS_INFO *Traits;
};	

struct ABC_CLASS_INFO
{
	U30 CInit;
	U30 TraitCount;
	struct ABC_TRAITS_INFO *Traits;
};

struct ABC_SCRIPT_INFO
{
	U30 Init;
	U30 TraitCount;
	struct ABC_TRAITS_INFO *Traits;
};

struct ABC_EXCEPTION_INFO
{
	U30 From;
	U30 To;
	U30 Target;
	U30 ExcType;
	U30 VarName;
};

struct ABC_METHOD_BODY_INFO
{
	U30 Method;
	U30 MaxStack;
	U30 LocalCount;
	U30 InitScopeDepth;
	U30 MaxScopeDepth;
	U30 CodeLength;
	UI8 *Code;
	U30 ExceptionCount;
	struct ABC_EXCEPTION_INFO *Exceptions;
	U30 TraitCount;
	struct ABC_TRAITS_INFO *Traits;
};

struct ABC_FILE
{
	UI16 Minor;
	UI16 Major;
	struct ABC_CONSTANT_POOL ConstantPool;
	U30 MethodCount;
	struct ABC_METHOD_INFO *Methods;
	U30 MetadataCount;
	struct ABC_METADATA_INFO *Metadata;
	U30 ClassCount;
	struct ABC_INSTANCE_INFO *Instances;
	struct ABC_CLASS_INFO *Classes;
	U30 ScriptCount;
	struct ABC_SCRIPT_INFO *Scripts;
	U30 MethodBodyCount;
	struct ABC_METHOD_BODY_INFO *MethodBodies;	
};

struct SWF_DOABC
{
	UI32 Flags;
	STRING Name;
	struct ABC_FILE AbcFile;
};

struct AS_SYMBOL
{
	UI16 SymbolId;
	STRING SymbolName;
};

struct SWF_SYMBOLCLASS
{
	UI16 SymbolCount;
	struct AS_SYMBOL *SymbolList;
};

struct SWF_DEFINEBINARYDATA
{	
	UI32 Reserved;
	int DataLength;
	UI8 *Data;
};

struct SCENE_DATA
{
	U32 Offset;
	STRING Name;
};

struct FRAME_DATA
{
	U32 FrameNum;
	STRING FrameLabel;
};

struct SWF_DEFINESCENEANDFRAMEDATA
{
	U32 SceneCount;
	struct SCENE_DATA *Scenes;
	U32 FrameLabelCount;
	struct FRAME_DATA *Frames;
};

struct SWF_DEBUGID
{
	UI8* UUID;
};

struct SWF_UNKNOWNBLOCK
{
	UI8 *Data;
};

typedef union _inline_SWF_Parserstruct
{
  struct SWF_CHARACTERSET SWF_CHARACTERSET;
  struct SWF_DEFINEBITS SWF_DEFINEBITS;
  struct SWF_DEFINEBITSJPEG2 SWF_DEFINEBITSJPEG2;
  struct SWF_DEFINEBITSJPEG3 SWF_DEFINEBITSJPEG3;
  struct SWF_DEFINEBITSPTR SWF_DEFINEBITSPTR;
  struct SWF_DEFINEBUTTON SWF_DEFINEBUTTON;
  struct SWF_DEFINEBUTTON2 SWF_DEFINEBUTTON2;
  struct SWF_DEFINEBUTTONCXFORM SWF_DEFINEBUTTONCXFORM;
  struct SWF_DEFINEBUTTONSOUND SWF_DEFINEBUTTONSOUND;
  struct SWF_DEFINECOMMANDOBJ SWF_DEFINECOMMANDOBJ;
  struct SWF_DEFINEEDITTEXT SWF_DEFINEEDITTEXT;
  struct SWF_DEFINEFONT SWF_DEFINEFONT;
  struct SWF_DEFINEFONT2 SWF_DEFINEFONT2;
  struct SWF_DEFINEFONT3 SWF_DEFINEFONT3;
  struct SWF_DEFINEFONTINFO SWF_DEFINEFONTINFO;
  struct SWF_DEFINEFONTINFO2 SWF_DEFINEFONTINFO2;
  struct SWF_DEFINEFONTALIGNZONES SWF_DEFINEFONTALIGNZONES;
  struct SWF_CSMTEXTSETTINGS SWF_CSMTEXTSETTINGS;
  struct SWF_DEFINEFONTNAME SWF_DEFINEFONTNAME;
  struct SWF_DEFINELOSSLESS SWF_DEFINELOSSLESS;
  struct SWF_DEFINELOSSLESS2 SWF_DEFINELOSSLESS2;
  struct SWF_DEFINEMORPHSHAPE SWF_DEFINEMORPHSHAPE;
  struct SWF_DEFINEMORPHSHAPE2 SWF_DEFINEMORPHSHAPE2;
  struct SWF_DEFINESHAPE SWF_DEFINESHAPE;
  struct SWF_DEFINESHAPE2 SWF_DEFINESHAPE2;
  struct SWF_DEFINESHAPE3 SWF_DEFINESHAPE3;
  struct SWF_DEFINESHAPE4 SWF_DEFINESHAPE4;
  struct SWF_DEFINESOUND SWF_DEFINESOUND;
  struct SWF_DEFINESPRITE SWF_DEFINESPRITE;
  struct SWF_DEFINETEXT SWF_DEFINETEXT;
  struct SWF_DEFINETEXT2 SWF_DEFINETEXT2;
  struct SWF_DEFINETEXTFORMAT SWF_DEFINETEXTFORMAT;
  struct SWF_DEFINEVIDEO SWF_DEFINEVIDEO;
  struct SWF_DEFINEVIDEOSTREAM SWF_DEFINEVIDEOSTREAM;
  struct SWF_DOACTION SWF_DOACTION;
  struct SWF_ENABLEDEBUGGER SWF_ENABLEDEBUGGER;
  struct SWF_ENABLEDEBUGGER2 SWF_ENABLEDEBUGGER2;
  struct SWF_END SWF_END;
  struct SWF_EXPORTASSETS SWF_EXPORTASSETS;
  struct SWF_FONTREF SWF_FONTREF;
  struct SWF_FRAMELABEL SWF_FRAMELABEL;
  struct SWF_FRAMETAG SWF_FRAMETAG;
  struct SWF_FREEALL SWF_FREEALL;
  struct SWF_FREECHARACTER SWF_FREECHARACTER;
  struct SWF_GENCOMMAND SWF_GENCOMMAND;
  struct SWF_IMPORTASSETS SWF_IMPORTASSETS;
  struct SWF_IMPORTASSETS2 SWF_IMPORTASSETS2;
  struct SWF_JPEGTABLES SWF_JPEGTABLES;
  struct SWF_NAMECHARACTER SWF_NAMECHARACTER;
  struct SWF_PATHSAREPOSTSCRIPT SWF_PATHSAREPOSTSCRIPT;
  struct SWF_PLACEOBJECT SWF_PLACEOBJECT;
  struct SWF_PLACEOBJECT2 SWF_PLACEOBJECT2;
  struct SWF_PLACEOBJECT3 SWF_PLACEOBJECT3;
  struct SWF_PREBUILT SWF_PREBUILT;
  struct SWF_PREBUILTCLIP SWF_PREBUILTCLIP;
  struct SWF_PROTECT SWF_PROTECT;
  struct SWF_REMOVEOBJECT SWF_REMOVEOBJECT;
  struct SWF_REMOVEOBJECT2 SWF_REMOVEOBJECT2;
  struct SWF_SERIALNUMBER SWF_SERIALNUMBER;
  struct SWF_SETBACKGROUNDCOLOR SWF_SETBACKGROUNDCOLOR;
  struct SWF_SHOWFRAME SWF_SHOWFRAME;
  struct SWF_SOUNDSTREAMBLOCK SWF_SOUNDSTREAMBLOCK;
  struct SWF_SOUNDSTREAMHEAD SWF_SOUNDSTREAMHEAD;
  struct SWF_SOUNDSTREAMHEAD2 SWF_SOUNDSTREAMHEAD2;
  struct SWF_STARTSOUND SWF_STARTSOUND;
  struct SWF_STARTSOUND2 SWF_STARTSOUND2;
  struct SWF_SYNCFRAME SWF_SYNCFRAME;
  struct SWF_INITACTION SWF_INITACTION;
  struct SWF_VIDEOFRAME SWF_VIDEOFRAME;
  struct SWF_REFLEX SWF_REFLEX;
  struct SWF_FILEATTRIBUTES SWF_FILEATTRIBUTES;
  struct SWF_METADATA SWF_METADATA;
  struct SWF_SCRIPTLIMITS SWF_SCRIPTLIMITS;
  struct SWF_DEFINESCALINGGRID SWF_DEFINESCALINGGRID;
  struct SWF_SETTABINDEX SWF_SETTABINDEX;
  struct SWF_DOABC SWF_DOABC;
  struct SWF_SYMBOLCLASS SWF_SYMBOLCLASS;
  struct SWF_DEFINEBINARYDATA SWF_DEFINEBINARYDATA;
  struct SWF_DEFINESCENEANDFRAMEDATA SWF_DEFINESCENEANDFRAMEDATA;
  struct SWF_DEBUGID SWF_DEBUGID;
  struct SWF_UNKNOWNBLOCK SWF_UNKNOWNBLOCK;
} _inline_SWF_Parserstruct;

struct SWF_Parserstruct_s
{
	_inline_SWF_Parserstruct block;
	int offset;
	int length;
};
typedef struct SWF_Parserstruct_s SWF_Parserstruct; 

#endif /* _SWFTYPES_H_ */
