
#include "ming_config.h"
#include "libming.h"
#include "error.h"

#if !(USE_PNG) // {

SWFDBLBitmapData newSWFDBLBitmapData_fromPngInput(SWFInput input)
{
	SWF_warn("newSWFDBLBitmapData_fromPngInput can't be used (no png compiled into this build of Ming).\n");
	return NULL;
}

SWFDBLBitmapData newSWFDBLBitmapData_fromPngFile(const char * fileName)
{
	SWF_warn("newSWFDBLBitmapData_fromPngFile can't be used (no png compiled into this build of Ming).\n");
	return NULL;
}

#else // def USE_PNG }{

#include "bitmap.h"
#include "dbl.h"
#include "input.h"

#include <png.h>
#include <zlib.h>
#include <stdlib.h>
#include <math.h>

struct pngdata
{
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	int num_palette;
	int channels;
	png_colorp palette;
	unsigned char *data;
};

// required - read from SWFInput
// png_init_io seems to be the one to change

static png_structp openPngFromFile(FILE *fp)
{	unsigned char header[8];
	png_structp result;

	if(fread(header, 1, 8, fp) != 8)
		return NULL;
	if(!png_check_sig(header, 8))
		return NULL;
	result = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!result)
		return NULL;
	png_init_io(result, fp);
	return result;
}

static int pngReadFunc(png_structp png, unsigned char *buf, int len)
{	SWFInput input = (SWFInput) png->io_ptr;
	return SWFInput_read(input, buf, len);
}

png_structp openPngFromInput(SWFInput input)
{	unsigned char header[8];
	png_structp result;
	
	if(SWFInput_read(input, header, 8) != 8)
		return NULL;
	if(!png_check_sig(header, 8))
		return NULL;
	result = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!result)
		return NULL;
	png_set_read_fn(result, input, (png_rw_ptr) pngReadFunc);
	return result;
}

static void
alignedcopy(struct pngdata png, unsigned char *data)
{
	unsigned int row;
	int alignedrowsize;
	int pngrowsize;
	unsigned char *pngdata;

	alignedrowsize = (png.width * png.channels + 3) & ~3;
	pngdata = png.data;
	pngrowsize = png.width * png.channels;

	for( row = 0; row < png.height; row++ )
	{
		memcpy(data, pngdata, pngrowsize);
		pngdata += pngrowsize;
		data += alignedrowsize;
	}
}

static int readPNG(png_structp png_ptr, dblData result)
{
	struct pngdata png;
	unsigned char *data;

	png_infop info_ptr, end_info;
	png_bytep *row_pointers;

	unsigned int i, rowbytes, depth, alignedsize;
	long outsize;

	info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return 0;
	}

	end_info = png_create_info_struct(png_ptr);

	if(!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return 0;
	}

	if(setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    	return 0;
	}

	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &png.width, &png.height,
	       &png.bit_depth, &png.color_type,
	       NULL, NULL, NULL);

/*  if(verbose)
  {
    printf("size %ld/%ld color %d/%d/%ld\n",
	   png.width, png.height, png.bit_depth, png.color_type,
	   png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS));
  }*/

	if(png.color_type == PNG_COLOR_TYPE_PALETTE)
	{
    	/*if(verbose)
			printf("color type: PALETTE\n");*/

		png_get_PLTE(png_ptr, info_ptr, &png.palette, &png.num_palette);
	}

	/* force bitdepth of 8 if necessary */

	depth = png.bit_depth;

	if(png.bit_depth < 8)
		png_set_packing(png_ptr);

	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);

	if(png.bit_depth == 16)
	{
		/*if(verbose)
			printf("depth: 16\n");*/

		png_set_strip_16(png_ptr);
	}

	if(png.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		/*if(verbose)
			printf("color type: GRAY ALPHA\n");*/

		png_set_gray_to_rgb(png_ptr);
	}

	if(png.color_type == PNG_COLOR_TYPE_RGB)
	{
		/*if(verbose)
			printf("color type: RGB\n");*/

		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	}


	/* update info w/ the set transformations */
	png_read_update_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &png.width, &png.height,
	       &png.bit_depth, &png.color_type,
	       NULL, NULL, NULL);

	result->width = png.width;
	result->height = png.height;

	png.channels = png_get_channels(png_ptr, info_ptr);


	if(png.color_type == PNG_COLOR_TYPE_GRAY)
	{
		/* turn it into a palettized image, use the gray values as indices to
       		an rgb color table */

		int i;

		/*if (verbose) printf("color type: GRAY\n");*/

		png.color_type = PNG_COLOR_TYPE_PALETTE;
		png.num_palette = 1 << depth;

		png.palette = (png_color*) malloc(sizeof(png_color) * png.num_palette);

		for(i=0; i<(int)png.num_palette; ++i)
			png.palette[i].red = png.palette[i].green = png.palette[i].blue = 
			 (i*255)/(png.num_palette-1);
	}


  /* malloc stuff */
	row_pointers = (png_bytep*) malloc(png.height*sizeof(png_bytep));
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	png.data = (unsigned char*) malloc(png.height * rowbytes);

	for(i=0; i<png.height; ++i)
		row_pointers[i] = png.data + rowbytes*i;

	png_read_image(png_ptr, row_pointers);

	if(png.color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
		png.color_type == PNG_COLOR_TYPE_RGB)
	{
		/* alpha has to be pre-applied, bytes shifted */

		unsigned int x, y;
		unsigned char *p;
		int alpha;
		unsigned char r, g, b;

		/*if(verbose)
			printf("color type: RGB ALPHA\n");*/

		for(y=0; y<png.height; ++y)
		{
			for(x=0; x<png.width; ++x)
			{
				p = png.data + rowbytes*y + 4*x;

				r = p[0];
				g = p[1];
				b = p[2];
				alpha = p[3];

				p[0] = alpha;
				p[1] = (char)((int)(r*alpha)>>8);
				p[2] = (char)((int)(g*alpha)>>8);
				p[3] = (char)((int)(b*alpha)>>8);
			}
		}
	}

	alignedsize = png.height * ((png.width * png.channels + 3) & ~3);

	if(png.color_type == PNG_COLOR_TYPE_PALETTE)
	{
		int tablesize = png.num_palette * sizeof(png_color);
  
		result->format = 3;
		result->format2 = png.num_palette-1;
		data = (unsigned char*) malloc(tablesize + alignedsize);
		memcpy(data, png.palette, tablesize);
		alignedcopy(png, data + tablesize);
		alignedsize += tablesize;
	}
	else
	{
		result->format = 5;
		data = (unsigned char*) malloc(alignedsize);
		alignedcopy(png, data);
	}

// at this point, can free
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(row_pointers);
//	fclose(fp);

	result->hasalpha = png.color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
						png.color_type == PNG_COLOR_TYPE_RGB_ALPHA;

	result->data = (unsigned char*) malloc(outsize = (int)floor(alignedsize*1.01+12));

	/* compress the RGB color table (if present) and image data one block */
	compress2(result->data, (uLongf *) &outsize, data, alignedsize, 9);
	result->length = outsize;

	free(data);
	free(png.data);

	return 1;
}


SWFDBLBitmapData newSWFDBLBitmapData_fromPngFile(const char *fileName)
{
	SWFDBLBitmapData ret;
	FILE *fp;
	png_structp png_ptr;
	struct dbl_data pngdata;
	int ok;

	fp = fopen(fileName, "rb");
	if(!fp)
		return NULL;
	png_ptr = openPngFromFile(fp);
	if(!png_ptr)
		return NULL;
	ok = readPNG(png_ptr, &pngdata);
	fclose(fp);
	if(!ok) return NULL;
	ret = newSWFDBLBitmapData_fromData(&pngdata);
	// ret->input = NULL;
	return ret;
}

SWFDBLBitmapData newSWFDBLBitmapData_fromPngInput(SWFInput input)
{
	SWFDBLBitmapData ret;
	png_structp png_ptr;
	struct dbl_data pngdata;
	int ok;

	png_ptr = openPngFromInput(input);
	if(!png_ptr)
		return NULL;
	ok = readPNG(png_ptr, &pngdata);
	if(!ok) return NULL;
	ret = newSWFDBLBitmapData_fromData(&pngdata);
	// ret->input = NULL;
	return ret;
}

#endif // def USE_PNG }
