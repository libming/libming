#include <stdlib.h>
#include <string.h>

#include "gradient.h"
#include "output.h"
#include "filter.h"
#include "error.h"

#include "libming.h"

enum
{
	DROP_SHADOW_FILTER,
	BLUR_FILTER,
	GLOW_FILTER,
	BEVEL_FILTER,
	GRADIENT_GLOW_FILTER,
	CONVOLUTION_FILTER,
	COLOR_MATRIX_FILTER,
	GRADIENT_BEVEL_FILTER
};


struct Shadow_s
{
	float	angle;
	float	distance;
	float 	strength;
};


/*
 * create a new SWFShadow (contructor)
 * This Function creates a new SWFShadow object. Only useful for creating 
 * SWFFilter objects.
 */
SWFShadow 
newSWFShadow(float angle /* angle in radians */, 
             float distance /* distance in px*/, 
             float strength /* strength */)
{
	SWFShadow shadow = (SWFShadow)malloc(sizeof(struct Shadow_s));
	
	shadow->angle = angle;
	shadow->distance = distance;
	shadow->strength = strength;

	return shadow;
}

void destroySWFShadow(SWFShadow s)
{
	free(s);
}

struct Blur_s
{
	float 	blurX;
	float 	blurY;
	int 	passes;
};


/*
 * create a new SWFBlur
 * This function creates a new SWFBlur object. Only useful for creating
 * SWFFilter objects
 */
SWFBlur 
newSWFBlur(float blurX /* horiz. blur amount */, 
           float blurY /* vert. blur amount */, 
           int passes /* number of passes. shoub be <= 3 */)
{
	SWFBlur blur = (SWFBlur)malloc(sizeof(struct Blur_s));

	blur->blurX = blurX;
	blur->blurY = blurY;
	blur->passes = passes;

	return blur;
}

void destroySWFBlur(SWFBlur b)
{
	free(b);
}

struct BlurFilter
{
	SWFBlur blur;
};

struct DropShadowFilter
{
	SWFColor		color;	
	SWFBlur 		blur;
	SWFShadow		shadow;	
	int 			flags;
};

struct GlowFilter
{
	SWFColor	color;
	SWFBlur		blur;
	float 		strength;
	int 		flags;
};

struct BevelFilter
{
	SWFColor	shadowColor;
	SWFColor	highlightColor;
	SWFBlur		blur;
	SWFShadow	shadow;
	int 		flags;
};

struct GradientGlowFilter
{
	SWFGradient	gradient;
	SWFBlur		blur;	
	SWFShadow	shadow;
	int 		flags;	
};

struct GradientBevelFilter
{
	SWFGradient	gradient;
	SWFBlur	 	blur;
	SWFShadow	shadow;	
	int 		flags;
};


struct FilterMatrix_s
{
	int cols;
	int rows;
	float *values;
};

/*
 * create a new FilterMatrix (constructor)
 * This function creates a new SFWFilterMatrix. Only useful 
 * for creating SWFFilter objects.
 */
SWFFilterMatrix 
newSWFFilterMatrix(int cols /* number of cols */, 
                   int rows /* number of rows */, 
                   float *vals /* vals[cols * rows]. Will be copied */)
{
	SWFFilterMatrix matrix;

	if(cols <= 0 || rows <= 0)
		return NULL;
	
	matrix = (SWFFilterMatrix)malloc(sizeof(struct FilterMatrix_s));

	matrix->cols = cols;
	matrix->rows = rows;

	matrix->values = (float *)malloc(cols * rows * sizeof(float));
	memcpy(matrix->values, vals, cols * rows * sizeof(float));

	return matrix;
}

void destroySWFFilterMatrix(SWFFilterMatrix m)
{
	if(m == NULL)
		return;

	free(m->values);
	free(m);
}

struct ConvolutionFilter
{
	SWFFilterMatrix matrix;
	float 		divisor;
	float 		bias;
	SWFColor	color;
	int		flags;
};


struct ColorMatrixFilter
{
	SWFFilterMatrix matrix;
};

struct SWFFilter_s
{
	int id;
	union {
		struct DropShadowFilter 	dropShadow;
		struct BlurFilter		blur;
		struct GlowFilter		glow;
		struct BevelFilter		bevel;
		struct GradientGlowFilter	gradientGlow;
		struct ConvolutionFilter	convolution;
		struct ColorMatrixFilter	colorMatrix;
		struct GradientBevelFilter	gradientBevel;
	} filter;
};

static void writeDropShadowFilter(SWFOutput out, struct DropShadowFilter *filter)
{
	int flags = FILTER_MODE_COMPOSITE;

	SWFOutput_writeUInt8(out, filter->color.red);
	SWFOutput_writeUInt8(out, filter->color.green);
	SWFOutput_writeUInt8(out, filter->color.blue);
	SWFOutput_writeUInt8(out, filter->color.alpha);
	
	SWFOutput_writeFixed(out, filter->blur->blurX);
	SWFOutput_writeFixed(out, filter->blur->blurY);	

	SWFOutput_writeFixed(out, filter->shadow->angle);
	SWFOutput_writeFixed(out, filter->shadow->distance);
	SWFOutput_writeFixed8(out, filter->shadow->strength);

	flags |= filter->flags;
	flags |= (0x1F & filter->blur->passes);
	SWFOutput_writeUInt8(out, flags);
}

static void writeBlurFilter(SWFOutput out, struct BlurFilter *filter)
{
	int flags = 0;

	SWFOutput_writeFixed(out, filter->blur->blurX);
	SWFOutput_writeFixed(out, filter->blur->blurY);	

	flags = 0xff & (filter->blur->passes << 3);
	SWFOutput_writeUInt8(out, flags);
}

static void writeGlowFilter(SWFOutput out, struct GlowFilter *filter)
{
	int flags = FILTER_MODE_COMPOSITE;

	SWFOutput_writeUInt8(out, filter->color.red);
	SWFOutput_writeUInt8(out, filter->color.green);
	SWFOutput_writeUInt8(out, filter->color.blue);
	SWFOutput_writeUInt8(out, filter->color.alpha);

	SWFOutput_writeFixed(out, filter->blur->blurX);
	SWFOutput_writeFixed(out, filter->blur->blurY);	

	SWFOutput_writeFixed8(out, filter->strength);
	flags |= filter->flags;
	flags |= (0x1F & filter->blur->passes);
	SWFOutput_writeUInt8(out, flags);
}

static void writeBevelFilter(SWFOutput out, struct BevelFilter *filter)
{
	int flags = FILTER_MODE_COMPOSITE;

	SWFOutput_writeUInt8(out, filter->shadowColor.red);
	SWFOutput_writeUInt8(out, filter->shadowColor.green);
	SWFOutput_writeUInt8(out, filter->shadowColor.blue);
	SWFOutput_writeUInt8(out, filter->shadowColor.alpha);

	SWFOutput_writeUInt8(out, filter->highlightColor.red);
	SWFOutput_writeUInt8(out, filter->highlightColor.green);
	SWFOutput_writeUInt8(out, filter->highlightColor.blue);
	SWFOutput_writeUInt8(out, filter->highlightColor.alpha);

	SWFOutput_writeFixed(out, filter->blur->blurX);
	SWFOutput_writeFixed(out, filter->blur->blurY);	
	
	SWFOutput_writeFixed(out, filter->shadow->angle);
	SWFOutput_writeFixed(out, filter->shadow->distance);
	SWFOutput_writeFixed8(out, filter->shadow->strength);

	flags |= filter->flags;
	flags |= (0xF & filter->blur->passes);
	SWFOutput_writeUInt8(out, flags);
}

static void writeGradientGlowFilter(SWFOutput out, struct GradientGlowFilter *filter)
{
	int flags = FILTER_MODE_COMPOSITE;

	SWFOutput_writeGradientAsFilter(out, filter->gradient);
	
	SWFOutput_writeFixed(out, filter->blur->blurX);
	SWFOutput_writeFixed(out, filter->blur->blurY);	
	
	SWFOutput_writeFixed(out, filter->shadow->angle);
	SWFOutput_writeFixed(out, filter->shadow->distance);
	SWFOutput_writeFixed8(out, filter->shadow->strength);

	flags |= filter->flags;
	flags |= (0xF & filter->blur->passes);
	SWFOutput_writeUInt8(out, flags);
}

static void writeConvolutionFilter(SWFOutput out, struct ConvolutionFilter *filter)
{
	int i;

	SWFOutput_writeUInt8(out, filter->matrix->cols);
	SWFOutput_writeUInt8(out, filter->matrix->rows);

	SWFOutput_writeFloat(out, filter->divisor);
	SWFOutput_writeFloat(out, filter->bias);

	for(i = 0; i < filter->matrix->cols * filter->matrix->rows; i++)
		SWFOutput_writeFloat(out, filter->matrix->values[i]);

	SWFOutput_writeUInt8(out, filter->color.red);
	SWFOutput_writeUInt8(out, filter->color.green);
	SWFOutput_writeUInt8(out, filter->color.blue);
	SWFOutput_writeUInt8(out, filter->color.alpha);

	SWFOutput_writeUInt8(out, filter->flags);
}

static void writeColorMatrixFilter(SWFOutput out, struct ColorMatrixFilter *filter)
{
	int i;

	for(i = 0; i < filter->matrix->rows * filter->matrix->cols; i++)
		SWFOutput_writeFloat(out, filter->matrix->values[i]);
}

static void writeGradientBevelFilter(SWFOutput out, struct GradientBevelFilter *filter)
{
	int flags = FILTER_MODE_COMPOSITE;

	SWFOutput_writeGradientAsFilter(out, filter->gradient);
	
	SWFOutput_writeFixed(out, filter->blur->blurX);
	SWFOutput_writeFixed(out, filter->blur->blurY);	
	
	SWFOutput_writeFixed(out, filter->shadow->angle);
	SWFOutput_writeFixed(out, filter->shadow->distance);
	SWFOutput_writeFixed8(out, filter->shadow->strength);

	flags |= filter->flags;
	flags |= (0xF & filter->blur->passes);
	SWFOutput_writeUInt8(out, flags);
}

void 
SWFOutput_writeSWFFilter(SWFOutput out, SWFFilter filter)
{
	if(out == NULL || filter == NULL)
		return;

	SWFOutput_writeUInt8(out, filter->id);
	switch(filter->id)
	{
		case DROP_SHADOW_FILTER:
			writeDropShadowFilter(out, &filter->filter.dropShadow);
			break;
		case BLUR_FILTER:
			writeBlurFilter(out, &filter->filter.blur);
			break;
		case GLOW_FILTER:
			writeGlowFilter(out, &filter->filter.glow);
			break;
		case BEVEL_FILTER:
			writeBevelFilter(out, &filter->filter.bevel);
			break;
		case GRADIENT_GLOW_FILTER:
			writeGradientGlowFilter(out, &filter->filter.gradientGlow);
			break;
		case CONVOLUTION_FILTER:
			writeConvolutionFilter(out, &filter->filter.convolution);
			break;
		case COLOR_MATRIX_FILTER:
			writeColorMatrixFilter(out, &filter->filter.colorMatrix);
			break;
		case GRADIENT_BEVEL_FILTER:
			writeGradientBevelFilter(out, &filter->filter.gradientBevel);
			break;
		default:
			SWF_error("destroySWFFilter: invalid filter\n");
	}
	
}

void 
destroySWFFilter(SWFFilter filter)
{
	if(filter == NULL)
		return;

	switch(filter->id)
	{
		case DROP_SHADOW_FILTER:
			// destroyDropShadowFilter(&filter->filter.dropShadow);
			break;
		case BLUR_FILTER:
			// destroyBlurFilter(&filter->filter.blur);
			break;
		case GLOW_FILTER:
			// destroyGlowFilter(&filter->filter.glow);
			break;
		case BEVEL_FILTER:
			//destroyBevelFilter(&filter->filter.bevel);
			break;
		case GRADIENT_GLOW_FILTER:
			// destroyGradientGlowFilter(&filter->filter.gradientGlow);
			break;
		case CONVOLUTION_FILTER:
			// destroyConvolutionFilter(&filter->filter.convolution);
			break;
		case COLOR_MATRIX_FILTER:
			// destroyColorMatrixFilter(&filter->filter.colorMatrix);
			break;
		case GRADIENT_BEVEL_FILTER:
			// destroyGradientBevelFilter(&filter->filter.gradientBevel);
			break;
		default:
			SWF_error("destroySWFFilter: invalid filter\n");
	}
	free(filter);
}

/* 
 * creates a new ColormatrixFilter 
 * Matrix has to be 5 x 4
 * [r0 ... r4]
 * [g0 ... g4]
 * [b0 ... b4]
 * [a0 ... a4]
 */
SWFFilter
newColorMatrixFilter(SWFFilterMatrix matrix /* matrix */)
{
	SWFFilter filter;

	if(matrix == NULL)
		return NULL;

	if(matrix->rows != 4 || matrix->cols != 5)
	{
		SWF_warn("newColorMatrixFilter: color matrix has to be 5x4\n");
		return NULL;
	}	

	filter = (SWFFilter)malloc(sizeof(struct SWFFilter_s));
	filter->id = COLOR_MATRIX_FILTER;

	filter->filter.colorMatrix.matrix = matrix;

	return filter;
}

/*
 * creates a new ConvolutionFilter
 */
SWFFilter 
newConvolutionFilter(SWFFilterMatrix matrix /* matrix */, 
                     float divisor /* divisor applied to matrix */,
                     float bias /* matrix bias */,
                     SWFColor color /* default color */,
                     int flags /* FILTER_FLAG_CLAMP , FILTER_FLAG_PRESERVE_ALPHA */)
{
	SWFFilter filter;
	struct ConvolutionFilter *convolution;

	if(matrix == NULL)
		return NULL;

	filter = (SWFFilter)malloc(sizeof(struct SWFFilter_s));
	filter->id = CONVOLUTION_FILTER;

	convolution = &filter->filter.convolution;

	convolution->matrix = matrix;
	convolution->divisor = divisor;
	convolution->bias = bias;
	convolution->color = color;
	convolution->flags = flags;

	return filter;
}
                     


/*
 * create a GradientBevelFilter 
 * This functions creates a gradient bevel filter. Extends BevelFilter by
 * using a gradient instead of a simple color
 */
SWFFilter
newGradientBevelFilter(SWFGradient gradient /* gradient */, 
                       SWFBlur blur /* blur */,
                       SWFShadow shadow /* shadow */,
                       int flags /* FILTER_MODE_INNER,  FILTER_MODE_KO, FILTER_MODE_ONTOP */)
{
	SWFFilter filter;
	struct GradientBevelFilter *gBevel;
	
	if(gradient == NULL || blur == NULL)
		return NULL;

	filter = (SWFFilter)malloc(sizeof(struct SWFFilter_s));
	filter->id = GRADIENT_BEVEL_FILTER;

	gBevel = &filter->filter.gradientBevel;

	gBevel->gradient = gradient;
	gBevel->blur = blur;
	gBevel->shadow = shadow;
	gBevel->flags = flags;

	return filter;
} 

/* 
 * creates a GardientGlowFilter
 * This functions creates a gradient glow filter. Extends GlowFilter by
 * using a gradient instead of a simple color
 */
SWFFilter 
newGradientGlowFilter(SWFGradient gradient /* gradient */, 
                      SWFBlur blur /* blur */, 
                      SWFShadow shadow /* shadow */,
                      int flags /* FILTER_MODE_INNER,  FILTER_MODE_KO, FILTER_MODE_ONTOP */)
{
	SWFFilter filter;
	struct GradientGlowFilter *gGlow;

	if(gradient == NULL || blur == NULL)
		return NULL;

	filter = (SWFFilter)malloc(sizeof(struct SWFFilter_s));
	filter->id = GRADIENT_GLOW_FILTER;

	gGlow = &filter->filter.gradientGlow;

	gGlow->gradient = gradient;
	gGlow->blur = blur;
	gGlow->shadow = shadow;
	gGlow->flags = flags;

	return filter;
}

/* 
 * creates a BevelFilter
 * This functions creates a bevel filter. Creates a smooth bevel
 */
SWFFilter 
newBevelFilter(SWFColor shadowColor /* shadow color */, 
               SWFColor highlightColor /* highlight color */, 
               SWFBlur blur /* blur */, 
               SWFShadow shadow /* shadow */,
               int flags /* FILTER_MODE_INNER,  FILTER_MODE_KO, FILTER_MODE_ONTOP */)
{
	SWFFilter filter;
	struct BevelFilter *bevel;

	if(blur == NULL || shadow == NULL)
		return NULL;

	filter = (SWFFilter)malloc(sizeof(struct SWFFilter_s));
	filter->id = BEVEL_FILTER;

	bevel = &filter->filter.bevel;

	bevel->shadowColor = shadowColor;
	bevel->highlightColor = highlightColor;
	bevel->blur = blur; 
	bevel->shadow = shadow;
	bevel->flags = flags;

	return filter;
}

/* 
 * creates a GlowFilter
 * This functions creates a glow filter.
 */
SWFFilter 
newGlowFilter(SWFColor color /* color of shadow */, 
              SWFBlur blur /* blur */, 
              float strength /* strength */, 
              int flags /* FILTER_MODE_INNER,  FILTER_MODE_KO */)
{
	SWFFilter filter;
	struct GlowFilter *glow;
	
	if(blur == NULL)
		return NULL;

	filter = (SWFFilter)malloc(sizeof(struct SWFFilter_s));
	filter->id = GLOW_FILTER;

	glow = &filter->filter.glow;

	glow->color = color;
	glow->blur = blur;
	glow->strength = strength;
	glow->flags = flags;

	return filter;
}

/*
 * creates a BlurFilter 
 * This functions creates a blur filter.
 */
SWFFilter 
newBlurFilter(SWFBlur blur /* blur */)
{
	SWFFilter filter;
	struct BlurFilter *b;
	if(blur == NULL)
		return NULL;

	filter = (SWFFilter)malloc(sizeof(struct SWFFilter_s));
	filter->id = BLUR_FILTER;
	b = &filter->filter.blur;
	b->blur = blur;

	return filter;
}

/*
 * creates a DropShadowFilter 
 * This functions creates a drop shadow filter filter.
 */
SWFFilter 
newDropShadowFilter(SWFColor color /* color of shadow */, 
                    SWFBlur blur /* blur */, 
                    SWFShadow shadow /* shadow */, 
                    int flags /* FILTER_MODE_INNER,  FILTER_MODE_KO */)
{
	struct DropShadowFilter *dropShadow;
	SWFFilter filter;

	if(blur == NULL || shadow == NULL)
		return NULL;

	filter = (SWFFilter)malloc(sizeof(struct SWFFilter_s));
	filter->id = DROP_SHADOW_FILTER;

	dropShadow = &filter->filter.dropShadow;
	dropShadow->color = color;
	dropShadow->blur = blur;
	dropShadow->shadow = shadow;
	dropShadow->flags = flags;

	return filter;
}
