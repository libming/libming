/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Dave Hayden <dave@opaque.net>                               |
  |          Frank M. Kromann <fmk@php.net>                              |
  |          Stuart R. Anderson <anderson@netsweng.com>                  |
  |          Klaus Rechert <klaus@rechert.de>                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/fsock.h"
#include "php_streams.h"

#if HAVE_MING

#include "php_ming.h"
#include <stdio.h>
#include <math.h>
#include <ming.h>

#define BYTE_Z_LVAL_PP(x)  ((byte)Z_LVAL_PP(x))

#ifndef HAVE_DESTROY_SWF_BLOCK
void destroySWFBlock(SWFBlock block);
#endif

static zend_function_entry ming_functions[] = {
	PHP_FALIAS(ming_setcubicthreshold,  ming_setCubicThreshold,  NULL)
	PHP_FALIAS(ming_setscale,           ming_setScale,           NULL)
	PHP_FALIAS(ming_useswfversion,      ming_useSWFVersion,      NULL)
	PHP_FALIAS(ming_keypress,           ming_keypress,           NULL)
#ifdef HAVE_NEW_MING
	PHP_FALIAS(ming_useconstants,		ming_useConstants,       NULL)
	PHP_FALIAS(ming_setswfcompression,	ming_setSWFCompression,  NULL)
#endif
	{ NULL, NULL, NULL }
};

static SWFMovie getMovie(zval *id TSRMLS_DC);
static SWFFill getFill(zval *id TSRMLS_DC);
static SWFGradient getGradient(zval *id TSRMLS_DC);
static SWFBitmap getBitmap(zval *id TSRMLS_DC);
static SWFShape getShape(zval *id TSRMLS_DC);
static SWFFont getFont(zval *id TSRMLS_DC);
static SWFText getText(zval *id TSRMLS_DC);
static SWFTextField getTextField(zval *id TSRMLS_DC);
static SWFDisplayItem getDisplayItem(zval *id TSRMLS_DC);
static SWFButton getButton(zval *id TSRMLS_DC);
static SWFAction getAction(zval *id TSRMLS_DC);
static SWFInitAction getInitAction(zval *id TSRMLS_DC);
static SWFMorph getMorph(zval *id TSRMLS_DC);
static SWFMovieClip getSprite(zval *id TSRMLS_DC);
static SWFSound getSound(zval *id TSRMLS_DC);
#ifdef HAVE_NEW_MING
static SWFFontCharacter getFontCharacter(zval *id TSRMLS_DC);
static SWFSoundInstance getSoundInstance(zval *id TSRMLS_DC);
static SWFVideoStream getVideoStream(zval *id TSRMLS_DC);
static SWFButtonRecord getButtonRecord(zval *id TSRMLS_DC);
static SWFPrebuiltClip getPrebuiltClip(zval *id TSRMLS_DC);
static SWFCharacter getCharacterClass(zval *id TSRMLS_DC);
static SWFBinaryData getBinaryData(zval *id TSRMLS_DC);
static SWFBlur getBlur(zval *id TSRMLS_DC);
static SWFShadow getShadow(zval *id TSRMLS_DC);
static SWFFilterMatrix getFilterMatrix(zval *id TSRMLS_DC);
static SWFFilter getFilter(zval *id TSRMLS_DC);
#endif

#define PHP_MING_FILE_CHK(file) \
	if ((PG(safe_mode) && !php_checkuid((file), NULL, CHECKUID_CHECK_FILE_AND_DIR)) || php_check_open_basedir((file) TSRMLS_CC)) {	\
		RETURN_FALSE;	\
	}	\

/* {{{ proto void ming_setcubicthreshold (int threshold)
   Set cubic threshold (?) */
PHP_FUNCTION(ming_setCubicThreshold)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);
	Ming_setCubicThreshold(Z_LVAL_PP(num));
}
/* }}} */

/* {{{ proto void ming_setscale(int scale)
   Set scale (?) */
PHP_FUNCTION(ming_setScale)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Ming_setScale(Z_DVAL_PP(num));
}
/* }}} */

/* {{{ proto void ming_useswfversion(int version)
   Use SWF version (?) */ 
PHP_FUNCTION(ming_useSWFVersion)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);
	Ming_useSWFVersion(Z_LVAL_PP(num));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void ming_useconstants(int use)
	Use constant pool (?) */ 
PHP_FUNCTION(ming_useConstants)
{
	zval **num;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(num);

	Ming_useConstants(Z_LVAL_PP(num));
}
/* }}} */

/* {{{ set output compression */
PHP_FUNCTION(ming_setSWFCompression)
{  
    zval **num;
    if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE)
        WRONG_PARAM_COUNT;
				   
    convert_to_long_ex(num);
    Ming_setSWFCompression(Z_LVAL_PP(num));
}
/* }}} */

#endif

static int le_swfmoviep;
static int le_swfshapep;
static int le_swffillp;
static int le_swfgradientp;
static int le_swfbitmapp;
static int le_swffontp;
static int le_swftextp;
static int le_swftextfieldp;
static int le_swfdisplayitemp;
static int le_swfbuttonp;
static int le_swfactionp;
static int le_swfmorphp;
static int le_swfmovieclipp;
static int le_swfspritep;
static int le_swfinputp;
static int le_swfsoundp;
#ifdef HAVE_NEW_MING
static int le_swffontcharp;
static int le_swfsoundinstancep;
static int le_swfvideostreamp;
static int le_swfbuttonrecordp;
static int le_swfbinarydatap;
static int le_swfinitactionp;
static int le_swfprebuiltclipp;
static int le_swfsoundstreamp;
static int le_swffilterp;
static int le_swfblurp;
static int le_swfshadowp;
static int le_swffiltermatrixp;
#endif
static int le_swfcharacterp;

static zend_class_entry *movie_class_entry_ptr;
static zend_class_entry *shape_class_entry_ptr;
static zend_class_entry *fill_class_entry_ptr;
static zend_class_entry *gradient_class_entry_ptr;
static zend_class_entry *bitmap_class_entry_ptr;
static zend_class_entry *font_class_entry_ptr;
static zend_class_entry *text_class_entry_ptr;
static zend_class_entry *textfield_class_entry_ptr;
static zend_class_entry *displayitem_class_entry_ptr;
static zend_class_entry *button_class_entry_ptr;
static zend_class_entry *action_class_entry_ptr;
static zend_class_entry *morph_class_entry_ptr;
static zend_class_entry *movieclip_class_entry_ptr;
static zend_class_entry *sprite_class_entry_ptr;
static zend_class_entry *sound_class_entry_ptr;
static zend_class_entry *character_class_entry_ptr;
#ifdef HAVE_NEW_MING
static zend_class_entry *fontchar_class_entry_ptr;
static zend_class_entry *soundinstance_class_entry_ptr;
static zend_class_entry *videostream_class_entry_ptr;
static zend_class_entry *buttonrecord_class_entry_ptr;
static zend_class_entry *binarydata_class_entry_ptr;
static zend_class_entry *initaction_class_entry_ptr;
static zend_class_entry *prebuiltclip_class_entry_ptr;
static zend_class_entry *soundstream_class_entry_ptr;
static zend_class_entry *filter_class_entry_ptr;
static zend_class_entry *blur_class_entry_ptr;
static zend_class_entry *shadow_class_entry_ptr;
static zend_class_entry *filtermatrix_class_entry_ptr;
#endif
static zend_class_entry *character_class_entry_ptr;

/* {{{ internal function SWFgetProperty
*/
static void *SWFgetProperty(zval *id, char *name, int namelen, int proptype TSRMLS_DC)
{
	zval **tmp;
	int id_to_find;
	void *property;
	int type;

	if (id) {
		if (zend_hash_find(Z_OBJPROP_P(id), name, namelen+1, (void **)&tmp) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find property %s", name);
			return NULL;
		}
		id_to_find = Z_LVAL_PP(tmp);
	} else {
		return NULL;
	}

	property = zend_list_find(id_to_find, &type);

	if (!property || type != proptype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find identifier (%d)", id_to_find);
		return NULL;
	}

	return property;
}
/* }}} */

/* {{{ SWFCharacter 
*/

/* {{{ internal function SWFCharacter getCharacter(zval *id)
   Returns the SWFCharacter contained in zval *id */
static SWFCharacter getCharacter(zval *id TSRMLS_DC)
{
	if (Z_OBJCE_P(id) == shape_class_entry_ptr)
		return (SWFCharacter)getShape(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == font_class_entry_ptr)
		return (SWFCharacter)getFont(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == text_class_entry_ptr)
		return (SWFCharacter)getText(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == textfield_class_entry_ptr)
		return (SWFCharacter)getTextField(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == button_class_entry_ptr)
		return (SWFCharacter)getButton(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == morph_class_entry_ptr)
		return (SWFCharacter)getMorph(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == movieclip_class_entry_ptr)
		return (SWFCharacter)getSprite(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == sprite_class_entry_ptr)
		return (SWFCharacter)getSprite(id TSRMLS_CC);
	else if (Z_OBJCE_P(id) == bitmap_class_entry_ptr)
		return (SWFCharacter)getBitmap(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == sound_class_entry_ptr)
		return (SWFCharacter)getSound(id TSRMLS_CC);
#ifdef HAVE_NEW_MING
	else if(Z_OBJCE_P(id) == fontchar_class_entry_ptr)
		return (SWFCharacter)getFontCharacter(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == soundinstance_class_entry_ptr)
		return (SWFCharacter)getSoundInstance(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == videostream_class_entry_ptr)
		return (SWFCharacter)getVideoStream(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == buttonrecord_class_entry_ptr)
		return (SWFCharacter)getButtonRecord(id TSRMLS_CC);
	else if(Z_OBJCE_P(id) == prebuiltclip_class_entry_ptr)
		return (SWFCharacter)getPrebuiltClip(id TSRMLS_CC);
#endif
	else
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFCharacter");
		return NULL;
}

static SWFCharacter getCharacterClass(zval *id TSRMLS_DC)
{
	void *character = SWFgetProperty(id, "character", strlen("character"), le_swfcharacterp TSRMLS_CC);

	if (!character) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFCharacter");
	}
	return (SWFCharacter)character;
}

static zend_function_entry swfcharacter_functions[] = {
	{ NULL, NULL, NULL }
};

/* }}} */
/* }}} */

/* {{{ getInput - utility func for making an SWFInput from an fopened resource
*/
static void destroy_SWFInput_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFInput((SWFInput)resource->ptr);
}

static SWFInput getInput(zval **zfile TSRMLS_DC)
{
	FILE *file;
	php_stream *stream;
	SWFInput input;

	php_stream_from_zval_no_verify(stream, zfile);

	if (stream == NULL)
		return NULL;

	if (php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void *) &file, REPORT_ERRORS) != SUCCESS) {
		return NULL;
	}

	input = newSWFInput_file(file);
	zend_list_addref(Z_LVAL_PP(zfile));
	zend_list_addref(zend_list_insert(input, le_swfinputp));
	return input;
}
/* }}} */

/* {{{ SWFInitAction
*/
/* {{{ proto void swfinitaction::__construct(action)
   Creates a new SWFInitAction object */
PHP_METHOD(swfinitaction, __construct)
{
	SWFInitAction init;
	zval **zaction;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zaction) == FAILURE)
		WRONG_PARAM_COUNT;
	
	convert_to_object_ex(zaction);
	init = newSWFInitAction(getAction(*zaction TSRMLS_CC));

	ret = zend_list_insert(init, le_swfinitactionp);
	object_init_ex(getThis(), initaction_class_entry_ptr);
	add_property_resource(getThis(), "initaction", ret);
	zend_list_addref(ret);
}
/* no destructor for SWFInitAction, it's not a character */
/* }}} */

/* {{{ internal function getInitAction
   Returns the SWFInitAction object contained in zval *id */
static SWFInitAction getInitAction(zval *id TSRMLS_DC)
{
	void *action = SWFgetProperty(id, "initaction", 
		strlen("initaction"), le_swfinitactionp TSRMLS_CC);

	if (!action) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFInitAction");
	}
	return (SWFInitAction)action;
}
/* }}} */

static zend_function_entry swfinitaction_functions[] = {
	PHP_ME(swfinitaction, __construct,          NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ SWFAction
*/
/* {{{ proto void swfaction::__construct(string)
   Creates a new SWFAction object, compiling the given script */
PHP_METHOD(swfaction, __construct)
{
	SWFAction action;
	zval **script;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &script) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(script);
	action = newSWFAction(Z_STRVAL_PP(script));

	if (!action) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Couldn't compile actionscript");
	}
	
	ret = zend_list_insert(action, le_swfactionp);

	object_init_ex(getThis(), action_class_entry_ptr);
	add_property_resource(getThis(), "action", ret);
	zend_list_addref(ret);
}
/* no destructor for SWFAction, it's not a character */
/* }}} */

PHP_METHOD(swfaction, compile)
{
	SWFAction action;
	zval **version;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &version) == FAILURE) 
		WRONG_PARAM_COUNT;
	
}

/* {{{ internal function getAction
   Returns the SWFAction object contained in zval *id */
static SWFAction getAction(zval *id TSRMLS_DC)
{
	void *action = SWFgetProperty(id, "action", 6, le_swfactionp TSRMLS_CC);

	if (!action) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFAction");
	}
	return (SWFAction)action;
}
/* }}} */

static zend_function_entry swfaction_functions[] = {
	PHP_ME(swfaction, __construct,          NULL, 0)
	PHP_ME(swfaction, compile,	 	NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFBitmap 
*/
/* {{{ proto void swfbitmap::__construct(mixed file [, mixed maskfile])
   Creates a new SWFBitmap object from jpg (with optional mask) or dbl file */
PHP_METHOD(swfbitmap, __construct)
{
	zval **zfile, **zmask = NULL;
	SWFBitmap bitmap;
	SWFJpegWithAlpha bitmap_alpha;
	SWFInput input, maskinput;
	int ret;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &zfile) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &zfile, &zmask) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
	
	if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
		convert_to_string_ex(zfile);
		PHP_MING_FILE_CHK(Z_STRVAL_PP(zfile));
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	} else {
		input = getInput(zfile TSRMLS_CC);
	}
	
	if (zmask != NULL) {
		if (Z_TYPE_PP(zmask) != IS_RESOURCE) {
			convert_to_string_ex(zmask);
			maskinput = newSWFInput_buffer(Z_STRVAL_PP(zmask), Z_STRLEN_PP(zmask));
			zend_list_addref(zend_list_insert(maskinput, le_swfinputp));
		} else {
			maskinput = getInput(zmask TSRMLS_CC);
		}
		bitmap_alpha = newSWFJpegWithAlpha_fromInput(input, maskinput);
		if(bitmap_alpha) {
			ret = zend_list_insert(bitmap_alpha, le_swfbitmapp);
			object_init_ex(getThis(), bitmap_class_entry_ptr);
			add_property_resource(getThis(), "bitmap", ret);
			zend_list_addref(ret);
		}
	} else {
		bitmap = newSWFBitmap_fromInput(input);
		if(bitmap) {
			ret = zend_list_insert(bitmap, le_swfbitmapp);
			object_init_ex(getThis(), bitmap_class_entry_ptr);
			add_property_resource(getThis(), "bitmap", ret);
			zend_list_addref(ret);
		}
	}
}

static void destroy_SWFBitmap_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBitmap((SWFBitmap)resource->ptr);
}
/* }}} */

/* {{{ internal function getBitmap
   Returns the SWFBitmap object contained in zval *id */
static SWFBitmap getBitmap(zval *id TSRMLS_DC)
{
	void *bitmap = SWFgetProperty(id, "bitmap", 6, le_swfbitmapp TSRMLS_CC);

	if (!bitmap) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFBitmap");
	}
	return (SWFBitmap)bitmap;
}
/* }}} */

/* {{{ proto float swfbitmap::getWidth()
   Returns the width of this bitmap */
PHP_METHOD(swfbitmap, getWidth)
{
	if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFBitmap_getWidth(getBitmap(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swfbitmap::getHeight()
   Returns the height of this bitmap */
PHP_METHOD(swfbitmap, getHeight)
{
	if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFBitmap_getHeight(getBitmap(getThis() TSRMLS_CC)));
}
/* }}} */

static zend_function_entry swfbitmap_functions[] = {
	PHP_ME(swfbitmap, __construct,         NULL, 0)
	PHP_ME(swfbitmap, getWidth,            NULL, 0)
	PHP_ME(swfbitmap, getHeight,           NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ internal function */
static
SWFButtonRecord getButtonRecord(zval *id TSRMLS_DC)
{
	void *record = SWFgetProperty(id, "buttonrecord", strlen("buttonrecord"), le_swfbuttonrecordp TSRMLS_CC);

	if(!record)
		php_error(E_ERROR, "called object is not an SWFButtonRecord!");

	return (SWFButtonRecord)record;
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::setDepth(int depth)
   sets a button characters depth */
PHP_METHOD(swfbuttonrecord, setDepth)
{
	zval **depth;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &depth) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(depth);
	SWFButtonRecord_setDepth(getButtonRecord(getThis() TSRMLS_CC), Z_LVAL_PP(depth));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::setBlendMode(int mode)
   assigns a blend mode to a button's character */
PHP_METHOD(swfbuttonrecord, setBlendMode)
{
	zval **mode;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(mode);
	SWFButtonRecord_setBlendMode(getButtonRecord(getThis() TSRMLS_CC), Z_LVAL_PP(mode));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::move(double x, double y)
   relative placement */
PHP_METHOD(swfbuttonrecord, move)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFButtonRecord_move(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::moveTo(double x, double y)
   absolute placement */
PHP_METHOD(swfbuttonrecord, moveTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFButtonRecord_moveTo(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::rotate(double deg)
   relative rotation */
PHP_METHOD(swfbuttonrecord, rotate)
{
	zval **deg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &deg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(deg);
	SWFButtonRecord_rotate(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(deg));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::rotateTo(double deg)
   absolute rotation */
PHP_METHOD(swfbuttonrecord, rotateTo)
{
	zval **deg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &deg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(deg);
	SWFButtonRecord_rotateTo(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(deg));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::scale(double x, double y)
   relative scaling */
PHP_METHOD(swfbuttonrecord, scale)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFButtonRecord_scale(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::scaleTo(double x, double y)
   absolute scaling */
PHP_METHOD(swfbuttonrecord, scaleTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFButtonRecord_scaleTo(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */


/* {{{ proto void swfbuttoncharacter::skewX(double x) */
PHP_METHOD(swfbuttonrecord, skewX)
{
	zval **x;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &x) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	SWFButtonRecord_skewX(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(x));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::skewXTo(double x) */
PHP_METHOD(swfbuttonrecord, skewXTo)
{
	zval **x;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &x) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	SWFButtonRecord_skewXTo(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(x));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::skewY(double y) */
PHP_METHOD(swfbuttonrecord, skewY)
{
	zval **y;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(y);
	SWFButtonRecord_skewY(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::skewYTo(double y) */
PHP_METHOD(swfbuttonrecord, skewYTo)
{
	zval **y;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(y);
	SWFButtonRecord_skewYTo(getButtonRecord(getThis() TSRMLS_CC), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfbuttoncharacter::addFilter(SWFFilter f) */
PHP_METHOD(swfbuttonrecord, addFilter)
{
	zval **filter;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filter) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(filter);
	SWFButtonRecord_addFilter(getButtonRecord(getThis() TSRMLS_CC), getFilter( *filter TSRMLS_CC)); 
}
/* }}} */

static zend_function_entry swfbuttonrecord_functions[] = {
	PHP_ME(swfbuttonrecord, setDepth,   NULL, 0)
	PHP_ME(swfbuttonrecord, setBlendMode,  NULL, 0)
	PHP_ME(swfbuttonrecord, move,  NULL, 0)
	PHP_ME(swfbuttonrecord, moveTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, rotate,  NULL, 0)
	PHP_ME(swfbuttonrecord, rotateTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, scale,  NULL, 0)
	PHP_ME(swfbuttonrecord, scaleTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, skewX,  NULL, 0)
	PHP_ME(swfbuttonrecord, skewXTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, skewY,  NULL, 0)
	PHP_ME(swfbuttonrecord, skewYTo,  NULL, 0)
	PHP_ME(swfbuttonrecord, addFilter, NULL, 0)
	{ NULL, NULL, NULL }
};
#endif

/* {{{ SWFButton
*/
/* {{{ proto void swfbutton::__construct()
   Creates a new SWFButton object */
PHP_METHOD(swfbutton, __construct)
{
	SWFButton button = newSWFButton();
	int ret = zend_list_insert(button, le_swfbuttonp);

	object_init_ex(getThis(), button_class_entry_ptr);
	add_property_resource(getThis(), "button", ret);
	zend_list_addref(ret);
}

static void destroy_SWFButton_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFButton((SWFButton)resource->ptr);
}
/* }}} */

/* {{{ internal function getButton
   Returns the SWFButton object contained in zval *id */
static SWFButton getButton(zval *id TSRMLS_DC)
{
	void *button = SWFgetProperty(id, "button", 6, le_swfbuttonp TSRMLS_CC);

	if (!button) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFButton");
	}
	return (SWFButton)button;
}
/* }}} */

/* {{{ proto void swfbutton::setHit(object SWFCharacter)
   Sets the character for this button's hit test state */
PHP_METHOD(swfbutton, setHit)
{
	zval **zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, SWFBUTTONRECORD_HITSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setOver(object SWFCharacter)
   Sets the character for this button's over state */
PHP_METHOD(swfbutton, setOver)
{
	zval **zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, SWFBUTTONRECORD_OVERSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setUp(object SWFCharacter)
   Sets the character for this button's up state */
PHP_METHOD(swfbutton, setUp)
{
	zval **zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, SWFBUTTONRECORD_UPSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::setDown(object SWFCharacter)
   Sets the character for this button's down state */
PHP_METHOD(swfbutton, setDown)
{
	zval **zchar;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	SWFButton_addCharacter(button, character, SWFBUTTONRECORD_DOWNSTATE);
}
/* }}} */

/* {{{ proto void swfbutton::addShape(object SWFCharacter, int flags)
   Sets the character to display for the condition described in flags */
PHP_METHOD(swfbutton, addShape)
{
	zval **zchar, **flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zchar, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	convert_to_long_ex(flags);
	SWFButton_addCharacter(button, character, BYTE_Z_LVAL_PP(flags));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto swfbuttonrecord swfbutton::addCharacter(object SWFCharacter, int flags)
   Sets the character to display for the condition described in flags */
PHP_METHOD(swfbutton, addCharacter)
{
	zval **zchar, **flags;
	SWFButtonRecord record;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFCharacter character;
	int ret;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zchar, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);
	character = getCharacter(*zchar TSRMLS_CC);
	convert_to_long_ex(flags);
	record = SWFButton_addCharacter(button, character, BYTE_Z_LVAL_PP(flags));

	if(record != NULL)
	{
		ret = zend_list_insert(record, le_swfbuttonrecordp);
		object_init_ex(return_value, buttonrecord_class_entry_ptr);
		add_property_resource(return_value, "buttonrecord", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ proto void swfbutton::setMenu(int flag)
	enable track as menu button behaviour */
PHP_METHOD(swfbutton, setMenu)
{
	zval **zflag;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zflag) == FAILURE)
		WRONG_PARAM_COUNT;	
	convert_to_long_ex(zflag);

	SWFButton_setMenu(button, Z_LVAL_PP(zflag));
}
/* }}} */

/* {{{ proto void swfbutton::setScalingGrid(int x, int y, int w, int h) */
PHP_METHOD(swfbutton, setScalingGrid)
{
	zval **zx, **zy, **zw, **zh;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	if(ZEND_NUM_ARGS() != 4 || 
			zend_get_parameters_ex(4, &zx, &zy, &zw, &zh) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zx);
	convert_to_long_ex(zy);
	convert_to_long_ex(zw);
	convert_to_long_ex(zh);

	SWFButton_setScalingGrid(button, Z_LVAL_PP(zx), Z_LVAL_PP(zy), 
					Z_LVAL_PP(zw), Z_LVAL_PP(zh));
}
/* }}} */

/* {{{ proto void swfbutton::removeScalingGrid() */
PHP_METHOD(swfbutton, removeScalingGrid)
{
	SWFButton button = getButton(getThis() TSRMLS_CC);
	if(ZEND_NUM_ARGS() != 0) 
		WRONG_PARAM_COUNT;

	SWFButton_removeScalingGrid(button);
}
/* }}} */
#endif

/* {{{ proto void swfbutton::setAction(object SWFAction)
   Sets the action to perform when button is pressed */
PHP_METHOD(swfbutton, setAction)
{
	zval **zaction;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFAction action;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zaction) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zaction);
	action = getAction(*zaction TSRMLS_CC);
	SWFButton_addAction(button, action, SWFBUTTON_OVERDOWNTOOVERUP);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto SWFSoundInstance swfbutton::addASound(SWFSound sound, int flags)
   associates a sound with a button transition
	NOTE: the transitions are all wrong _UP, _OVER, _DOWN _HIT  */

PHP_METHOD(swfbutton, addSound)
{
	zval **zsound, **flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFSound sound;
	SWFSoundInstance item;
	int ret;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zsound, &flags) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = getSound(*zsound TSRMLS_CC);

	convert_to_long_ex(flags);

	item = SWFButton_addSound(button, sound, Z_LVAL_PP(flags));

	if(item != NULL) {
		/* try and create a soundinstance object */
		ret = zend_list_insert(item, le_swfsoundinstancep);
		object_init_ex(return_value, soundinstance_class_entry_ptr);
		add_property_resource(return_value, "soundinstance", ret);
		zend_list_addref(ret);
	}
}
/* }}} */
#endif

/* {{{ proto void swfbutton::addAction(object SWFAction, int flags)
   Sets the action to perform when conditions described in flags is met */
PHP_METHOD(swfbutton, addAction)
{
	zval **zaction, **flags;
	SWFButton button = getButton(getThis() TSRMLS_CC);
	SWFAction action;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zaction, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zaction);
	action = getAction(*zaction TSRMLS_CC);
	convert_to_long_ex(flags);
	SWFButton_addAction(button, action, Z_LVAL_PP(flags));
}
/* }}} */

/* {{{ proto int ming_keypress(string str)
   Returns the action flag for keyPress(char) */
PHP_FUNCTION(ming_keypress)
{
	zval **key;
	char c;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(key);

	if (Z_STRLEN_PP(key) > 1) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Only one character expected");
	}
	
	c = Z_STRVAL_PP(key)[0];
	RETURN_LONG((c&0x7f)<<9);
}
/* }}} */

static zend_function_entry swfbutton_functions[] = {
	PHP_ME(swfbutton, __construct,   NULL, 0)
	PHP_ME(swfbutton, setHit,        NULL, 0)
	PHP_ME(swfbutton, setOver,       NULL, 0)
	PHP_ME(swfbutton, setUp,         NULL, 0)
	PHP_ME(swfbutton, setDown,       NULL, 0)
	PHP_ME(swfbutton, setAction,     NULL, 0)
	PHP_ME(swfbutton, addShape,      NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfbutton, setMenu,       NULL, 0)
	PHP_ME(swfbutton, setScalingGrid, NULL, 0)
	PHP_ME(swfbutton, removeScalingGrid, NULL, 0)
#endif
	PHP_ME(swfbutton, addAction,     NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfbutton, addSound,      NULL, 0)
	PHP_ME(swfbutton, addCharacter,  NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFDisplayitem
*/
/* {{{ internal function getDisplayItem
   Returns the SWFDisplayItem contained in zval *id */
static SWFDisplayItem getDisplayItem(zval *id TSRMLS_DC)
{
	void *item = SWFgetProperty(id, "displayitem", 11, le_swfdisplayitemp TSRMLS_CC);

	if (!item) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFDisplayItem");
	}

	return (SWFDisplayItem)item;
}
/* }}} */

/* {{{ proto void swfdisplayitem::moveTo(double x, double y)
   Moves this SWFDisplayItem to movie coordinates (x, y) */
PHP_METHOD(swfdisplayitem, moveTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFDisplayItem_moveTo(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::move(double dx, double dy)
   Displaces this SWFDisplayItem by (dx, dy) in movie coordinates */
PHP_METHOD(swfdisplayitem, move)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFDisplayItem_move(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::scaleTo(double xScale [, double yScale])
   Scales this SWFDisplayItem by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swfdisplayitem, scaleTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &x) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(x);
		SWFDisplayItem_scaleTo(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(x));
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &x, &y) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(x);
		convert_to_double_ex(y);
		SWFDisplayItem_scaleTo(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfdisplayitem::scale(double xScale, double yScale)
   Multiplies this SWFDisplayItem's current x scale by xScale, its y scale by yScale */
PHP_METHOD(swfdisplayitem, scale)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFDisplayItem_scale(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::rotateTo(double degrees)
   Rotates this SWFDisplayItem the given (clockwise) degrees from its original orientation */
PHP_METHOD(swfdisplayitem, rotateTo)
{
	zval **degrees;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &degrees) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(degrees);
	SWFDisplayItem_rotateTo(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(degrees));
}
/* }}} */

/* {{{ proto void swfdisplayitem::rotate(double degrees)
   Rotates this SWFDisplayItem the given (clockwise) degrees from its current orientation */
PHP_METHOD(swfdisplayitem, rotate)
{
	zval **degrees;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &degrees) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(degrees);
	SWFDisplayItem_rotate(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(degrees));
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewXTo(double xSkew)
   Sets this SWFDisplayItem's x skew value to xSkew */
PHP_METHOD(swfdisplayitem, skewXTo)
{
	zval **x;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &x) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(x);
	SWFDisplayItem_skewXTo(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(x));
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewX(double xSkew)
   Adds xSkew to this SWFDisplayItem's x skew value */
PHP_METHOD(swfdisplayitem, skewX)
{
	zval **x;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &x) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(x);
	SWFDisplayItem_skewX(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(x));
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewYTo(double ySkew)
   Sets this SWFDisplayItem's y skew value to ySkew */
PHP_METHOD(swfdisplayitem, skewYTo)
{
	zval **y;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(y);
	SWFDisplayItem_skewYTo(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::skewY(double ySkew)
   Adds ySkew to this SWFDisplayItem's y skew value */
PHP_METHOD(swfdisplayitem, skewY)
{
	zval **y;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(y);
	SWFDisplayItem_skewY(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfdisplayitem::setMatrix(double a, double b, double c, double d, double x, double y)
   Sets the item's transform matrix */
PHP_METHOD(swfdisplayitem, setMatrix)
{
	zval **a, **b, **c, **d, **x, **y;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &a, &b, &c, &d, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(a);
	convert_to_double_ex(b);
	convert_to_double_ex(c);
	convert_to_double_ex(d);
	convert_to_double_ex(x);
	convert_to_double_ex(y);

	SWFDisplayItem_setMatrix( getDisplayItem(getThis() TSRMLS_CC), 
		Z_DVAL_PP(a), Z_DVAL_PP(b), Z_DVAL_PP(c), Z_DVAL_PP(d), Z_DVAL_PP(x), Z_DVAL_PP(y)
	);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setDepth(int depth)
   Sets this SWFDisplayItem's z-depth to depth.  Items with higher depth values are drawn on top of those with lower values */
PHP_METHOD(swfdisplayitem, setDepth)
{
	zval **depth;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &depth) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(depth);
	SWFDisplayItem_setDepth(getDisplayItem(getThis() TSRMLS_CC), Z_LVAL_PP(depth));
}
/* }}} */

/* {{{ proto void swfdisplayitem::setRatio(float ratio)
   Sets this SWFDisplayItem's ratio to ratio.  Obviously only does anything if displayitem was created from an SWFMorph */
PHP_METHOD(swfdisplayitem, setRatio)
{
	zval **ratio;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &ratio) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(ratio);
	SWFDisplayItem_setRatio(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(ratio));
}
/* }}} */

/* {{{ proto void swfdisplayitem::addColor(int r, int g, int b [, int a])
   Sets the add color part of this SWFDisplayItem's CXform to (r, g, b [, a]), a defaults to 0 */
PHP_METHOD(swfdisplayitem, addColor)
{
	zval **r, **g, **b, **za;
	int a = 0;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &za) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(za);
		a = Z_LVAL_PP(za);
	} else if (ZEND_NUM_ARGS() == 3) { 
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	SWFDisplayItem_setColorAdd(getDisplayItem(getThis() TSRMLS_CC), Z_LVAL_PP(r), Z_LVAL_PP(g), Z_LVAL_PP(b), a);
}
/* }}} */

/* {{{ proto void swfdisplayitem::multColor(float r, float g, float b [, float a])
   Sets the multiply color part of this SWFDisplayItem's CXform to (r, g, b [, a]), a defaults to 1.0 */
PHP_METHOD(swfdisplayitem, multColor)
{
	zval **r, **g, **b, **za;
	float a = 1.0f;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &za) == FAILURE) { 
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(za);
		a = Z_DVAL_PP(za);
	} else if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);

	SWFDisplayItem_setColorMult(getDisplayItem(getThis() TSRMLS_CC), Z_DVAL_PP(r), Z_DVAL_PP(g), Z_DVAL_PP(b), a);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setName(string name)
   Sets this SWFDisplayItem's name to name */
PHP_METHOD(swfdisplayitem, setName)
{
	zval **name;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(name);
	SWFDisplayItem_setName(item, Z_STRVAL_PP(name));
}
/* }}} */

/* {{{ proto void swfdisplayitem::addAction(object SWFAction, int flags)
   Adds this SWFAction to the given SWFSprite instance */
PHP_METHOD(swfdisplayitem, addAction)
{
	zval **zaction, **flags;
	SWFAction action;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zaction, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_object_ex(zaction);
	convert_to_long_ex(flags);
	action = getAction(*zaction TSRMLS_CC);
	SWFDisplayItem_addAction(item, action, Z_LVAL_PP(flags));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ swfdisplayitem_remove */

PHP_METHOD(swfdisplayitem, remove)
{
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_remove(item);
}
/* }}} */

/* {{{ proto void swfdisplayitem::setMaskLevel(int level)
   defines a MASK layer at level */

PHP_METHOD(swfdisplayitem, setMaskLevel)
{
	zval **level;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &level) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(level);

	SWFDisplayItem_setMaskLevel(getDisplayItem(getThis() TSRMLS_CC), Z_LVAL_PP(level));
}
/* }}} */

/* {{{ proto void swfdisplayitem::endMask()
   another way of defining a MASK layer */

PHP_METHOD(swfdisplayitem, endMask)
{
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_endMask(item);
}
/* }}} */

/* {{{ swfdisplayitem_getX */

PHP_METHOD(swfdisplayitem, getX)
{
	double x, y;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_getPosition(item, &x, &y);
	RETURN_DOUBLE(x);
}
/* }}} */

/* {{{ swfdisplayitem_getY */

PHP_METHOD(swfdisplayitem, getY)
{
	double x, y;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_getPosition(item, &x, &y);
	RETURN_DOUBLE(y);
}
/* }}} */

/* {{{ swfdisplayitem_getXScale */

PHP_METHOD(swfdisplayitem, getXScale)
{
	double sx, sy;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_getScale(item, &sx, &sy);
	RETURN_DOUBLE(sx);
}
/* }}} */

/* {{{ swfdisplayitem_getYScale */

PHP_METHOD(swfdisplayitem, getYScale)
{
	double sx, sy;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_getScale(item, &sx, &sy);
	RETURN_DOUBLE(sy);
}
/* }}} */

/* {{{ swfdisplayitem_getXSkew */

PHP_METHOD(swfdisplayitem, getXSkew)
{
	double sx, sy;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_getSkew(item, &sx, &sy);
	RETURN_DOUBLE(sx);
}
/* }}} */

/* {{{ swfdisplayitem_getYSkew */

PHP_METHOD(swfdisplayitem, getYSkew)
{
	double sx, sy;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_getSkew(item, &sx, &sy);
	RETURN_DOUBLE(sy);
}
/* }}} */

/* {{{ swfdisplayitem_getRot */

PHP_METHOD(swfdisplayitem, getRot)
{
	double ret;
	SWFDisplayItem item = getDisplayItem(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFDisplayItem_getRotation(item, &ret);
	RETURN_DOUBLE(ret);
}
/* }}} */

/* {{{ proto void swfdisplayitem::cacheAsBitmap(int flag)
   caches item as bitmap. can improve rendering speed */

PHP_METHOD(swfdisplayitem, cacheAsBitmap)
{
	zval **flag;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &flag) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(flag);

	SWFDisplayItem_cacheAsBitmap(getDisplayItem(getThis() TSRMLS_CC), Z_LVAL_PP(flag));
}
/* }}} */


/* {{{ proto void swfdisplayitem::setBlendMode(int mode)
   adds blending to item */
PHP_METHOD(swfdisplayitem, setBlendMode)
{
	zval **mode;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &mode) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(mode);

	SWFDisplayItem_cacheAsBitmap(getDisplayItem(getThis() TSRMLS_CC), Z_LVAL_PP(mode));
}
/* }}} */

/* {{{ proto int swfdisplayitem::setDepth()
   gets the items depth */
PHP_METHOD(swfdisplayitem, getDepth)
{
	int ret;

	if(ZEND_NUM_ARGS() != 0) 
		WRONG_PARAM_COUNT;

	ret = SWFDisplayItem_getDepth(getDisplayItem(getThis() TSRMLS_CC));
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int swfdisplayitem::flush() */
PHP_METHOD(swfdisplayitem, flush)
{
	if(ZEND_NUM_ARGS() != 0) 
		WRONG_PARAM_COUNT;

	SWFDisplayItem_flush(getDisplayItem(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfdisplayitem::addFilter(SWFFilter filter) */
PHP_METHOD(swfdisplayitem, addFilter)
{
	zval **filter;
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filter) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(filter);
	SWFDisplayItem_addFilter(getDisplayItem(getThis() TSRMLS_CC), getFilter(*filter TSRMLS_CC)); 

}

#endif

static zend_function_entry swfdisplayitem_functions[] = {
	PHP_ME(swfdisplayitem, moveTo,      NULL, 0)
	PHP_ME(swfdisplayitem, move,        NULL, 0)
	PHP_ME(swfdisplayitem, scaleTo,     NULL, 0)
	PHP_ME(swfdisplayitem, scale,       NULL, 0)
	PHP_ME(swfdisplayitem, rotateTo,    NULL, 0)
	PHP_ME(swfdisplayitem, rotate,      NULL, 0)
	PHP_ME(swfdisplayitem, skewXTo,     NULL, 0)
	PHP_ME(swfdisplayitem, skewX,       NULL, 0)
	PHP_ME(swfdisplayitem, skewYTo,     NULL, 0)
	PHP_ME(swfdisplayitem, skewY,       NULL, 0)
	PHP_ME(swfdisplayitem, setMatrix,   NULL, 0)
	PHP_ME(swfdisplayitem, setDepth,    NULL, 0)
	PHP_ME(swfdisplayitem, setRatio,    NULL, 0)
	PHP_ME(swfdisplayitem, addColor,    NULL, 0)
	PHP_ME(swfdisplayitem, multColor,   NULL, 0)
	PHP_ME(swfdisplayitem, setName,     NULL, 0)
	PHP_ME(swfdisplayitem, addAction,   NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfdisplayitem, remove,      NULL, 0)
	PHP_ME(swfdisplayitem, setMaskLevel,NULL, 0)
	PHP_ME(swfdisplayitem, endMask,     NULL, 0)
	PHP_ME(swfdisplayitem, getX,        NULL, 0)
	PHP_ME(swfdisplayitem, getY,        NULL, 0)
	PHP_ME(swfdisplayitem, getXScale,   NULL, 0)
	PHP_ME(swfdisplayitem, getYScale,   NULL, 0)
	PHP_ME(swfdisplayitem, getXSkew,    NULL, 0)
	PHP_ME(swfdisplayitem, getYSkew,    NULL, 0)
	PHP_ME(swfdisplayitem, getRot,      NULL, 0)
	PHP_ME(swfdisplayitem, cacheAsBitmap, NULL, 0)
	PHP_ME(swfdisplayitem, setBlendMode, NULL, 0)
	PHP_ME(swfdisplayitem, getDepth,    NULL, 0)
	PHP_ME(swfdisplayitem, flush,    NULL, 0)
	PHP_ME(swfdisplayitem, addFilter,    NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFFill
*/
/* {{{ proto void swffill::__construct()
   Creates a new SWFFill object */
PHP_METHOD(swffill, __construct)
{
  php_error_docref(NULL TSRMLS_CC, E_ERROR, "Instantiating SWFFill won't do any good- use SWFShape::addFill() instead");
}

static void destroy_SWFFill_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	/* this only destroys the shallow wrapper for SWFFillStyle,
	   which SWFShape destroys.  So everything's okay.  I hope. */

	destroySWFFill((SWFFill)resource->ptr);
}
/* }}} */

/* {{{ internal function getFill
   Returns the SWFFill object contained in zval *id */
static SWFFill getFill(zval *id TSRMLS_DC)
{
	void *fill = SWFgetProperty(id, "fill", 4, le_swffillp TSRMLS_CC);

	if (!fill) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFFill");
	}
	return (SWFFill)fill;
}
/* }}} */

/* {{{ proto void swffill::moveTo(float x, float y)
   Moves this SWFFill to shape coordinates (x,y) */
PHP_METHOD(swffill, moveTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFFill_moveTo(getFill(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swffill::scaleTo(float xScale [, float yScale])
   Scales this SWFFill by xScale in the x direction, yScale in the y, or both to xScale if only one arg */
PHP_METHOD(swffill, scaleTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &x) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(x);
		SWFFill_scaleXYTo(getFill(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(x));
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &x, &y) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(x);
		convert_to_double_ex(y);
		SWFFill_scaleXYTo(getFill(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swffill::rotateTo(float degrees)
   Rotates this SWFFill the given (clockwise) degrees from its original orientation */
PHP_METHOD(swffill, rotateTo)
{
	zval **degrees;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &degrees) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(degrees);
	SWFFill_rotateTo(getFill(getThis() TSRMLS_CC), Z_DVAL_PP(degrees));
}
/* }}} */

/* {{{ proto void swffill::skewXTo(float xSkew)
   Sets this SWFFill's x skew value to xSkew */
PHP_METHOD(swffill, skewXTo)
{
	zval **x;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &x) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	SWFFill_skewXTo(getFill(getThis() TSRMLS_CC), Z_DVAL_PP(x));
}
/* }}} */

/* {{{ proto void swffill::skewYTo(float ySkew)
   Sets this SWFFill's y skew value to ySkew */
PHP_METHOD(swffill, skewYTo)
{
	zval **y;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(y);
	SWFFill_skewYTo(getFill(getThis() TSRMLS_CC), Z_DVAL_PP(y));
}
/* }}} */
static zend_function_entry swffill_functions[] = {
	PHP_ME(swffill, __construct, NULL, 0)
	PHP_ME(swffill, moveTo,      NULL, 0)
	PHP_ME(swffill, scaleTo,     NULL, 0)
	PHP_ME(swffill, rotateTo,    NULL, 0)
	PHP_ME(swffill, skewXTo,     NULL, 0)
	PHP_ME(swffill, skewYTo,     NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFFontCharacter */
#ifdef HAVE_NEW_MING

/* {{{ internal function SWFText getFont(zval *id)
   Returns the Font object in zval *id */
static
SWFFontCharacter getFontCharacter(zval *id TSRMLS_DC)
{
	void *font = SWFgetProperty(id, "fontcharacter", 13, le_swffontcharp TSRMLS_CC);

	if(!font)
		php_error(E_ERROR, "called object is not an SWFFontCharacter!");

	return (SWFFontCharacter)font;
}
/* }}} */

/* {{{ proto void swffontcha::raddChars(string)
   adds characters to a font for exporting font */
PHP_METHOD(swffontchar, addChars)
{
	zval **zstring;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	SWFFontCharacter_addChars(getFontCharacter(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));
}
/* }}} */

/* {{{ proto void swffontchar::addChars(string)
   adds characters to a font for exporting font */

PHP_METHOD(swffontchar, addUTF8Chars)
{
	zval **zstring;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	SWFFontCharacter_addUTF8Chars(getFontCharacter(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));
}
/* }}} */

/* {{{ proto void swffontchar::addAllChars()
 *    adds all characters to a font for exporting font */

PHP_METHOD(swffontchar, addAllChars)
{
	if(ZEND_NUM_ARGS() != 0 )
		WRONG_PARAM_COUNT;

	SWFFontCharacter_addAllChars(getFontCharacter(getThis() TSRMLS_CC));
}
/* }}} */

static zend_function_entry swffontchar_functions[] = {
	PHP_ME(swffontchar, addChars,   NULL, 0)
	PHP_ME(swffontchar, addUTF8Chars,  NULL, 0)
	PHP_ME(swffontchar, addAllChars, NULL, 0)
	{ NULL, NULL, NULL }
};

#endif
/* }}} */

/* {{{ SWFFont
*/
/* {{{ internal function SWFText getFont(zval *id)
   Returns the Font object in zval *id */
static SWFFont getFont(zval *id TSRMLS_DC)
{
	void *font = SWFgetProperty(id, "font", 4, le_swffontp TSRMLS_CC);

	if (!font) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFFont");
	}
	return (SWFFont)font;
}
/* }}} */

/* {{{ proto void swffont::__construct(string filename)
   Creates a new SWFFont object from given file */
PHP_METHOD(swffont, __construct)
{
	zval **zfile;
	SWFFont font;
	int ret;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zfile) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(zfile);

	if (strcasecmp(Z_STRVAL_PP(zfile)+Z_STRLEN_PP(zfile)-4, ".fdb") == 0 || 
			strcasecmp(Z_STRVAL_PP(zfile)+Z_STRLEN_PP(zfile)-4, ".ttf") == 0) {
		PHP_MING_FILE_CHK(Z_STRVAL_PP(zfile));
		font = newSWFFont_fromFile(Z_STRVAL_PP(zfile));
	} else {
		font = (SWFFont)newSWFBrowserFont(Z_STRVAL_PP(zfile));
	}
	ret = zend_list_insert(font, le_swffontp);

	object_init_ex(getThis(), font_class_entry_ptr);
	add_property_resource(getThis(), "font", ret);
	zend_list_addref(ret);
}

static void destroy_SWFFont_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBlock((SWFBlock)resource->ptr);
}
/* }}} */

/* {{{ proto float swffont::getWidth(string str)
   Calculates the width of the given string in this font at full height */
PHP_METHOD(swffont, getWidth)
{
	zval **zstring;
	float width;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(zstring);
	width = SWFFont_getStringWidth(getFont(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));
	RETURN_DOUBLE(width);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto int swffont::getUTF8Width(string)
   Calculates the width of the given string in this font at full height */

PHP_METHOD(swffont, getUTF8Width)
{
	zval **zstring;
	float width;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	width = SWFFont_getUTF8StringWidth(getFont(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));

	RETURN_DOUBLE(width);
}

/* }}} */
/* not sure about 0 bytes !!!!!!!!! */
/* {{{ proto int swffont::getWideWidth(string)
   Calculates the width of the given string in this font at full height */
/*
PHP_METHOD(swffont, getWideWidth)
{
	zval **zstring;
	float width;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	width = SWFFont_getWideStringWidth(getFont(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));

	RETURN_DOUBLE(width);
}
*/
/* }}} */
#endif

/* {{{ proto float swffont::getAscent()
   Returns the ascent of the font, or 0 if not available */
PHP_METHOD(swffont, getAscent)
{
    if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getAscent(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swffont::getDescent()
   Returns the descent of the font, or 0 if not available */
PHP_METHOD(swffont, getDescent)
{
    if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getDescent(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swffont::getLeading()
   Returns the leading of the font, or 0 if not available */
PHP_METHOD(swffont, getLeading)
{
    if(ZEND_NUM_ARGS() != 0) {
	    WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFFont_getLeading(getFont(getThis() TSRMLS_CC)));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swffont::addChars(string)
   adds characters to a font required within textfields */
/*
PHP_METHOD(swffont, addChars)
{
	zval **zstring;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	SWFFont_addChars(getFont(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));
}
*/
/* }}} */

#endif

static zend_function_entry swffont_functions[] = {
	PHP_ME(swffont, __construct,       NULL, 0)
	PHP_ME(swffont, getWidth,          NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swffont, getUTF8Width,      NULL, 0)
/*	PHP_ME(swffont, getwidewidth,      NULL, 0)*/
#endif
	PHP_ME(swffont, getAscent,         NULL, 0)
	PHP_ME(swffont, getDescent,        NULL, 0)
	PHP_ME(swffont, getLeading,        NULL, 0)
#ifdef HAVE_NEW_MING
/*	PHP_ME(swffont, addchars,          NULL, 0)*/
#endif
	{ NULL, NULL, NULL }
};

/* }}} */


/* {{{ SWFFilterMatrix
*/
/* {{{ proto void swffiltermatrix::__construct(cols, rows, array:vals)
   Creates a new SWFFilterMatrix object */
PHP_METHOD(swffiltermatrix, __construct)
{
	zval **cols, **rows, **vals, **data;
	SWFFilterMatrix matrix;
	HashTable *arr_hash;
	int ret, items, i;
	float *values;
	HashPosition pointer;

	if (ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(3, &cols, &rows, &vals) == FAILURE) 
		WRONG_PARAM_COUNT;

	convert_to_long_ex(cols);
	convert_to_long_ex(rows);
	convert_to_array_ex(vals);

	arr_hash = Z_ARRVAL_PP(vals);
	items = zend_hash_num_elements(arr_hash);
	if(items != Z_LVAL_PP(cols) * Z_LVAL_PP(rows))
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Can't create FilterMatrix."
		"Not enough / too many items it array");
	}
	values = (float *)malloc(items * sizeof(float));
	for(i = 0, zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); 
	    zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; 
	    zend_hash_move_forward_ex(arr_hash, &pointer), i++) 
	{
		zval temp;
		temp = **data;
		zval_copy_ctor(&temp);
		convert_to_double(&temp);
		values[i] = Z_DVAL(temp);
		zval_dtor(&temp);
	}
	
	matrix = newSWFFilterMatrix(Z_LVAL_PP(cols), Z_LVAL_PP(rows), values);
	free(values); // array is copied by libming
	ret = zend_list_insert(matrix, le_swffiltermatrixp);
	object_init_ex(getThis(), filtermatrix_class_entry_ptr);
	add_property_resource(getThis(), "filtermatrix", ret);
	zend_list_addref(ret);
}

static void destroy_SWFFilterMatrix_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFFilterMatrix((SWFFilterMatrix)resource->ptr);
}
/* }}} */

/* {{{ internal function getFilterMatrix
   Returns the SWFFilterMatrix object contained in zval *id */
static SWFFilterMatrix getFilterMatrix(zval *id TSRMLS_DC)
{
	void *matrix = SWFgetProperty(id, "filtermatrix", strlen("filtermatrix"), le_swffiltermatrixp TSRMLS_CC);

	if (!matrix) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFFilterMatrix");
	}
	return (SWFFilterMatrix)matrix;
}
/* }}} */
static zend_function_entry swffiltermatrix_functions[] = {
	PHP_ME(swffiltermatrix, __construct, 		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFShadow
*/
/* {{{ proto void swfshadow::__construct(angle, distance, strength)
   Creates a new SWFShadow object */
PHP_METHOD(swfshadow, __construct)
{
	zval **angle, **distance, **strength;
	SWFShadow shadow;
	int ret;

	if (ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(3, &angle, &distance, &strength) == FAILURE) 
		WRONG_PARAM_COUNT;

	convert_to_double_ex(angle);
	convert_to_double_ex(distance);
	convert_to_double_ex(strength);

	shadow = newSWFShadow(Z_DVAL_PP(angle), Z_DVAL_PP(distance), Z_DVAL_PP(strength));
	ret = zend_list_insert(shadow, le_swfshadowp);
	object_init_ex(getThis(), shadow_class_entry_ptr);
	add_property_resource(getThis(), "shadow", ret);
	zend_list_addref(ret);
}

static void destroy_SWFShadow_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFShadow((SWFShadow)resource->ptr);
}
/* }}} */

/* {{{ internal function getShadow
   Returns the SWFShadow object contained in zval *id */
static SWFShadow getShadow(zval *id TSRMLS_DC)
{
	void *shadow = SWFgetProperty(id, "shadow", strlen("shadow"), le_swfshadowp TSRMLS_CC);

	if (!shadow) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFShadow");
	}
	return (SWFShadow)shadow;
}
/* }}} */
static zend_function_entry swfshadow_functions[] = {
	PHP_ME(swfshadow, __construct, 		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFBlur
*/
/* {{{ proto void swfblur::__construct(blurX, blurY, passes)
   Creates a new SWFBlur object */
PHP_METHOD(swfblur, __construct)
{
	zval **blurX, **blurY, **passes;
	SWFBlur blur;
	int ret;

	if (ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(3, &blurX, &blurY, &passes) == FAILURE) 
		WRONG_PARAM_COUNT;

	convert_to_double_ex(blurX);
	convert_to_double_ex(blurY);
	convert_to_long_ex(passes);

	blur = newSWFBlur(Z_DVAL_PP(blurX), Z_DVAL_PP(blurY), Z_LVAL_PP(passes));
	ret = zend_list_insert(blur, le_swfblurp);
	object_init_ex(getThis(), blur_class_entry_ptr);
	add_property_resource(getThis(), "blur", ret);
	zend_list_addref(ret);
}

static void destroy_SWFBlur_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBlur((SWFBlur)resource->ptr);
}
/* }}} */

/* {{{ internal function getBlur
   Returns the SWFBlur object contained in zval *id */
static SWFBlur getBlur(zval *id TSRMLS_DC)
{
	void *blur = SWFgetProperty(id, "blur", strlen("blur"), le_swfblurp TSRMLS_CC);

	if (!blur) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFBlur");
	}
	return (SWFBlur)blur;
}
/* }}} */
static zend_function_entry swfblur_functions[] = {
	PHP_ME(swfblur, __construct, 		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ SWFGradient
*/
/* {{{ proto void swfgradient::__construct()
   Creates a new SWFGradient object */
PHP_METHOD(swfgradient, __construct)
{
	SWFGradient gradient = newSWFGradient();
	int ret = zend_list_insert(gradient, le_swfgradientp);

	object_init_ex(getThis(), gradient_class_entry_ptr);
	add_property_resource(getThis(), "gradient", ret);
	zend_list_addref(ret);
}

static void destroy_SWFGradient_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFGradient((SWFGradient)resource->ptr);
}
/* }}} */

/* {{{ internal function getGradient
   Returns the SWFGradient object contained in zval *id */
static SWFGradient getGradient(zval *id TSRMLS_DC)
{
	void *gradient = SWFgetProperty(id, "gradient", 8, le_swfgradientp TSRMLS_CC);

	if (!gradient) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFGradient");
	}
	return (SWFGradient)gradient;
}
/* }}} */

/* {{{ proto void swfgradient::addEntry(float ratio, int r, int g, int b [, int a])
   Adds given entry to the gradient */
PHP_METHOD(swfgradient, addEntry)
{
	zval **ratio, **r, **g, **b;
	byte a = 0xff;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &ratio, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 5) {
		zval **za;

		if (zend_get_parameters_ex(5, &ratio, &r, &g, &b, &za) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(za);
		a = BYTE_Z_LVAL_PP(za);
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(ratio);
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	SWFGradient_addEntry( getGradient(getThis() TSRMLS_CC), 
		Z_DVAL_PP(ratio), BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), a
	);
}
/* }}} */

/* {{{ proto void swfgradient::setSpreadMode(mode)
   supported mode values:
   * SWF_GRADIENT_PAD  
   * SWF_GRADIENT_REFLECT
   * SWF_GRADIENT_REPEAT
*/
PHP_METHOD(swfgradient, setSpreadMode)
{
	zval **val;
	if(ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(1, &val) == FAILURE) 
		WRONG_PARAM_COUNT;
	convert_to_long_ex(val);

	SWFGradient_setSpreadMode(getGradient(getThis() TSRMLS_CC), Z_LVAL_PP(val)); 
}
/* }}} */

/* {{{ proto void swfgradient::setInterpolationMode(mode)
 * supported mode values:
   * SWF_GRADIENT_NORMAL
   * SWF_GRADIENT_LINEAR
*/
PHP_METHOD(swfgradient, setInterpolationMode)
{
	zval **val;
	if(ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(1, &val) == FAILURE) 
		WRONG_PARAM_COUNT;
	convert_to_long_ex(val);

	SWFGradient_setInterpolationMode(getGradient(getThis() TSRMLS_CC), Z_LVAL_PP(val)); 
}
/* }}} */

/* {{{ proto void swfgradient::setFocalPoint(mode) */
PHP_METHOD(swfgradient, setFocalPoint)
{
	zval **val;
	if(ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(1, &val) == FAILURE) 
		WRONG_PARAM_COUNT;
	convert_to_double_ex(val);

	SWFGradient_setFocalPoint(getGradient(getThis() TSRMLS_CC), Z_DVAL_PP(val)); 
}
/* }}} */

static zend_function_entry swfgradient_functions[] = {
	PHP_ME(swfgradient, __construct, 		NULL, 0)
	PHP_ME(swfgradient, addEntry,    		NULL, 0)
	PHP_ME(swfgradient, setSpreadMode,		NULL, 0)
	PHP_ME(swfgradient, setInterpolationMode,	NULL, 0)
	PHP_ME(swfgradient, setFocalPoint,		NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFFilter
*/

/* helper functions */
static SWFColor hashToColor(zval **colorHash)
{
	zval **data;
	HashPosition pointer;
	HashTable *arr_hash;
	SWFColor c;
	
	arr_hash = Z_ARRVAL_PP(colorHash);
	if(zend_hash_num_elements(arr_hash) < 3 || zend_hash_num_elements(arr_hash) > 4)
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "not a valid colorHash\n");

	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); 
	    zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; 
	    zend_hash_move_forward_ex(arr_hash, &pointer)) 
	{
		zval temp;
		char *key;
		int key_len;
		long index;
		
		temp = **data;
		if (zend_hash_get_current_key_ex(arr_hash, &key, &key_len, &index, 0, &pointer) 
			== HASH_KEY_IS_STRING)
		{
			zval_copy_ctor(&temp);
			convert_to_long(&temp);
			c.alpha = 0;
			if(strcmp(key, "red") == 0)
				c.red = Z_LVAL(temp);
			else if (strcmp(key, "green") == 0)
				c.green = Z_LVAL(temp);
			else if (strcmp(key, "blue") == 0)
				c.blue = Z_LVAL(temp);
			else if (strcmp(key, "alpha") == 0)
				c.alpha = Z_LVAL(temp);
			else
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "not a valid colorHash\n");

			zval_dtor(&temp);
		}
	}
	return c;
}

static SWFFilter createDropShadowFilter(int argc, zval **argv[])
{
	zval **colorHash, **blur, **shadow, **flags;
	SWFFilter filter;
	SWFColor c;
	
	if(argc != 5)
		WRONG_PARAM_COUNT;

	colorHash = argv[1];
	convert_to_array_ex(colorHash);
	c = hashToColor(colorHash); 
		
	blur = argv[2];
	convert_to_object_ex(blur);
	
	shadow = argv[3];
	convert_to_object_ex(shadow);

	flags = argv[4]; 
	convert_to_long_ex(flags);

	return newDropShadowFilter(c, getBlur(*blur TSRMLS_CC), 
		getShadow(*shadow TSRMLS_CC), Z_LVAL_PP(flags));
}

static SWFFilter createBlurFilter(int argc, zval **argv[])
{
	zval **blur;
	
	if(argc != 2)
		WRONG_PARAM_COUNT;

	blur = argv[1];
	convert_to_object_ex(blur);
	
	return newBlurFilter(getBlur(*blur TSRMLS_CC));
}

static SWFFilter createGlowFilter(int argc, zval **argv[])
{
	zval **color, **blur, **strength, **flags;
	SWFColor c;
	
	if(argc != 5)
		WRONG_PARAM_COUNT;
	
	color = argv[1];
	convert_to_array_ex(color);
	c = hashToColor(color);

	blur = argv[2];
	convert_to_object_ex(blur);

	strength = argv[3];
	convert_to_double_ex(strength);
	
	flags = argv[4]; 
	convert_to_long_ex(flags);

	return newGlowFilter(c, getBlur(*blur TSRMLS_CC), 
		Z_DVAL_PP(strength), Z_LVAL_PP(flags));
}

static SWFFilter createBevelFilter(int argc, zval **argv[])
{
	zval **sColor, **hColor, **blur, **shadow, **flags;
	SWFColor hc, sc;

	if(argc != 6)
		WRONG_PARAM_COUNT;

	sColor = argv[1];
	convert_to_array_ex(sColor);
	sc = hashToColor(sColor);

	hColor = argv[2];
	convert_to_array_ex(hColor);
	hc = hashToColor(hColor);

	blur = argv[3];
	convert_to_object_ex(blur);

	shadow = argv[4];
	convert_to_object_ex(shadow);

	flags = argv[5]; 
	convert_to_long_ex(flags);

	return newBevelFilter(sc, hc, getBlur(*blur TSRMLS_CC),
		getShadow(*shadow TSRMLS_CC), Z_LVAL_PP(flags));
}

static SWFFilter createGradientGlowFilter(int argc, zval **argv[])
{
	zval **gradient, **blur, **shadow, **flags;

	if(argc != 5)
		WRONG_PARAM_COUNT;		

	gradient = argv[1];
	convert_to_object_ex(gradient);

	blur = argv[2];
	convert_to_object_ex(blur);
	
	shadow = argv[3];
	convert_to_object_ex(shadow);

	flags = argv[4]; 
	convert_to_long_ex(flags);

	return newGradientGlowFilter(getGradient(*gradient TSRMLS_CC), 
		getBlur(*blur TSRMLS_CC), getShadow(*shadow TSRMLS_CC), 
		Z_LVAL_PP(flags));
}

static SWFFilter createConvolutionFilter(int argc, zval **argv[])
{
	zval **matrix, **div, **bias, **color, **flags;
	SWFColor c;

	if(argc != 6)
		WRONG_PARAM_COUNT;

	matrix = argv[1];
	convert_to_object_ex(matrix);

	div = argv[2];
	convert_to_double_ex(div);

	bias = argv[3];
	convert_to_double_ex(bias);

	color = argv[4];
	convert_to_array_ex(color);
	c = hashToColor(color);

	flags = argv[5];
	convert_to_long_ex(flags);
	
	return newConvolutionFilter(getFilterMatrix(*matrix TSRMLS_CC), Z_DVAL_PP(div),
		Z_DVAL_PP(bias), c, Z_LVAL_PP(flags));	
}

static SWFFilter createColorMatrixFilter(int argc, zval **argv[])
{
	zval **matrix;

	if(argc != 2)
		WRONG_PARAM_COUNT;

	matrix = argv[1];
	convert_to_object_ex(matrix);

	return newColorMatrixFilter(getFilterMatrix(*matrix TSRMLS_CC));
}

static SWFFilter createGradientBevelFilter(int argc, zval **argv[])
{
	zval **gradient, **blur, **shadow, **flags;

	if(argc != 5)
		WRONG_PARAM_COUNT;		

	gradient = argv[1];
	convert_to_object_ex(gradient);

	blur = argv[2];
	convert_to_object_ex(blur);
	
	shadow = argv[3];
	convert_to_object_ex(shadow);

	flags = argv[4]; 
	convert_to_long_ex(flags);

	return newGradientBevelFilter(getGradient(*gradient TSRMLS_CC), 
		getBlur(*blur TSRMLS_CC), getShadow(*shadow TSRMLS_CC), 
		Z_LVAL_PP(flags));
}

/* {{{ proto void swffilter::__construct(type, ...)
   Creates a new SWFFilter object:
   
   Supported filter types:
   * SWFFILTER_TYPE_DROPSHADOW
     new SWFFilter(SWFFILTER_TYPE_DROPSHADOW, colorHash, blur, shadow, flags);
   * SWFFILTER_TYPE_BLUR
     new SWFFilter(SWFFILTER_TYPE_BLUR, blur);
   * SWFFILTER_TYPE_GLOW
     new SWFFilter(SWFFILTER_TYPE_GLOW, colorHash, blur, strenght:float, flags); 
   * SWFFILTER_TYPE_BEVEL
     new SWFFilter(SWFFILTER_TYPE_BEVEL, colorHash_shadow, colorHash_highlight, blur, shadow, flags); 
   * SWFFILTER_TYPE_GRADIENTGLOW
     new SWFFilter(SWFFILTER_TYPE_GRADIENTGLOW, gradient, blur, shadow, flags);
   * SWFFILTER_TYPE_CONVOLUTION
     new SWFFilter(SWFFILTER_TYPE_CONVOLUTION, filterMatrix, divisor:float, bias:float, colorHash, flags);
   * SWFFILTER_TYPE_COLORMATRIX
     new SWFFilter(SWFFILTER_TYPE_COLORMATRIX, filterMatrix);
   * SWFFILTER_TYPE_GRADIENTBEVEL
     new SWFFilter(SWFFILTER_TYPE_GRADIENTBEVEL, gradient, blur, shadow, flags);

   Supported flags are:
   * FILTER_FLAG_CLAMP
   * FILTER_FLAG_PRESERVE_ALPHA

   A colorHash must have the following entries:
   'red'   => 0...255, 
   'green' =>  0...255,
   'blue'  => 0...255,
   'alpha' => 0...255 (optional)
*/
PHP_METHOD(swffilter, __construct)
{
	zval **argv[6];
	int argc = ZEND_NUM_ARGS();
	int type, ret;
	SWFFilter filter;
	
	if (argc > 6 || argc < 2 || zend_get_parameters_array_ex(argc ,argv) == FAILURE) 
		WRONG_PARAM_COUNT;
	

	convert_to_long_ex(argv[0]);
	type = Z_LVAL_PP(argv[0]);
	switch(type)
	{
	case SWFFILTER_TYPE_DROPSHADOW:
		filter = createDropShadowFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_BLUR:
		filter = createBlurFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_GLOW:
		filter = createGlowFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_BEVEL:
		filter = createBevelFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_GRADIENTGLOW:
		filter = createGradientGlowFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_CONVOLUTION:
		filter = createConvolutionFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_COLORMATRIX:
		filter  = createColorMatrixFilter(argc, argv);
		break;
	case SWFFILTER_TYPE_GRADIENTBEVEL:
		filter = createGradientBevelFilter(argc, argv);
		break;
	default:
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "new SWFFilter: unknown type");	
	}
	ret = zend_list_insert(filter, le_swffilterp);
	object_init_ex(getThis(), filter_class_entry_ptr);
	add_property_resource(getThis(), "filter", ret);
	zend_list_addref(ret);	
}
/* }}} */

/* {{{ internal function getFilter
   Returns the SWFFilter object contained in zval *id */
static SWFFilter getFilter(zval *id TSRMLS_DC)
{
	void *filter = SWFgetProperty(id, "filter", 
		strlen("filter"), le_swffilterp TSRMLS_CC);

	if (!filter) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFFilter");
	}
	return (SWFFilter)filter;
}
/* }}} */

static void destroy_SWFFilter_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFFilter((SWFFilter)resource->ptr);
}

static zend_function_entry swffilter_functions[] = {
	PHP_ME(swffilter, __construct, NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */



/* {{{ SWFMorph 
*/
/* {{{ proto void swfmorph::__construct()
   Creates a new SWFMorph object */
PHP_METHOD(swfmorph, __construct)
{
	SWFMorph morph = newSWFMorphShape();
	int ret = zend_list_insert(morph, le_swfmorphp);

	object_init_ex(getThis(), morph_class_entry_ptr);
	add_property_resource(getThis(), "morph", ret);
	zend_list_addref(ret);
}

static void destroy_SWFMorph_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFMorph((SWFMorph)resource->ptr);
}
/* }}} */

/* {{{ internal function getMorph
   Returns the SWFMorph object contained in zval *id */
static SWFMorph getMorph(zval *id TSRMLS_DC)
{
	void *morph = SWFgetProperty(id, "morph", 5, le_swfmorphp TSRMLS_CC);

	if (!morph) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFMorph");
	}
	return (SWFMorph)morph;
}
/* }}} */

/* {{{ proto object swfmorph::getShape1()
   Return's this SWFMorph's start shape object */
PHP_METHOD(swfmorph, getShape1)
{
	SWFMorph morph = getMorph(getThis() TSRMLS_CC);
	SWFShape shape = SWFMorph_getShape1(morph);
	int ret = zend_list_insert(shape, le_swfshapep);

	object_init_ex(return_value, shape_class_entry_ptr);
	add_property_resource(return_value, "shape", ret);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto object swfmorph::getShape2()
   Return's this SWFMorph's start shape object */
PHP_METHOD(swfmorph, getShape2)
{
	SWFMorph morph = getMorph(getThis() TSRMLS_CC);
	SWFShape shape = SWFMorph_getShape2(morph);
	int ret = zend_list_insert(shape, le_swfshapep);

	object_init_ex(return_value, shape_class_entry_ptr);
	add_property_resource(return_value, "shape", ret);
	zend_list_addref(ret);
}
/* }}} */

static zend_function_entry swfmorph_functions[] = {
	PHP_ME(swfmorph, __construct,     NULL, 0)
	PHP_ME(swfmorph, getShape1,       NULL, 0)
	PHP_ME(swfmorph, getShape2,       NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFSoundStream
 */

/* {{{ proto class soundstream_init(file)
 */
PHP_METHOD(swfsoundstream, __construct)
{
	zval **zfile;
	SWFSoundStream sound = NULL;
	SWFInput input;
	int ret;

	switch(ZEND_NUM_ARGS()) 
	{
	case 1:
		if(zend_get_parameters_ex(1, &zfile) == FAILURE)
			WRONG_PARAM_COUNT;
			
		if(Z_TYPE_PP(zfile) != IS_RESOURCE)
		{
			convert_to_string_ex(zfile);
			input = newSWFInput_filename(Z_STRVAL_PP(zfile));
			zend_list_addref(zend_list_insert(input, le_swfinputp));
		}
		else
			input = getInput(zfile TSRMLS_CC);
		
		sound = newSWFSoundStream_fromInput(input);
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}
	
	if(sound) {
		ret = zend_list_insert(sound, le_swfsoundstreamp);
		object_init_ex(getThis(), soundstream_class_entry_ptr);
		add_property_resource(getThis(), "soundstream", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ internal function getSoundStream
   Returns the SWFSoundStream object contained in zval *id */
static SWFSoundStream getSoundStream(zval *id TSRMLS_DC)
{
	void *sound = SWFgetProperty(id, "soundstream", strlen("soundstream"), 
	                             le_swfsoundstreamp TSRMLS_CC);
	if(!sound)
		php_error(E_ERROR, "called object is not an SWFSoundStream!");
	return (SWFSoundStream)sound;
}
/* }}} */

/* {{{ internal function destroy_SWFSoundStream */
static void destroy_SWFSoundStream_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFSoundStream((SWFSoundStream)resource->ptr);
}
/* }}} */

/* {{{
   returns the duration of the sound stream */
PHP_METHOD(swfsoundstream, getDuration) 
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	RETURN_LONG(SWFSoundStream_getDuration(getSoundStream(getThis() TSRMLS_CC)));
}
/* }}} */


static zend_function_entry swfsoundstream_functions[] = {
	PHP_ME(swfsoundstream, __construct, NULL, 0)
	PHP_ME(swfsoundstream, getDuration, NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */



/* {{{ SWFSound */
/* {{{ internal function SWFSound getSound(zval *id)
   Returns the Sound object in zval *id */

SWFSound getSound(zval *id TSRMLS_DC)
{
	void *sound = SWFgetProperty(id, "sound", 5, le_swfsoundp TSRMLS_CC);

	if(!sound)
		php_error(E_ERROR, "called object is not an SWFSound!");

	return (SWFSound)sound;
}

/* }}} */
/* {{{ proto void swfsound::__construct(string filename, int flags)
   Creates a new SWFSound object from given file */
PHP_METHOD(swfsound, __construct)
{
	zval **zfile, **zflags;
	SWFSound sound = NULL;
	SWFInput input;
	SWFSoundStream stream;
	int flags;
	int ret;

	if(ZEND_NUM_ARGS() == 1)
	{
		if(zend_get_parameters_ex(1, &zfile) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_object_ex(zfile);
		stream = getSoundStream(*zfile TSRMLS_CC);
		sound = newSWFSound_fromSoundStream(stream);
	}
	else if(ZEND_NUM_ARGS() == 2)
	{
		if(zend_get_parameters_ex(2, &zfile, &zflags) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(zflags);
		flags = Z_LVAL_PP(zflags);

		if(Z_TYPE_PP(zfile) != IS_RESOURCE)
		{
			convert_to_string_ex(zfile);
			PHP_MING_FILE_CHK(Z_STRVAL_PP(zfile));
			input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
			zend_list_addref(zend_list_insert(input, le_swfinputp));
		}
		else
			input = getInput(zfile TSRMLS_CC);

		sound = newSWFSound_fromInput(input, flags);
	}
	else
		WRONG_PARAM_COUNT;

	if(sound != NULL)
	{	
		ret = zend_list_insert(sound, le_swfsoundp);
		object_init_ex(getThis(), sound_class_entry_ptr);
		add_property_resource(getThis(), "sound", ret);
		zend_list_addref(ret);
	}
}

static void destroy_SWFSound_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBlock((SWFBlock)resource->ptr);
}

/* }}} */

static zend_function_entry swfsound_functions[] = {
	PHP_ME(swfsound, __construct,             NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFSoundInstance */
#ifdef HAVE_NEW_MING
/* should handle envelope functions */
/* {{{ internal function SWFSoundInstance getSoundInstance(zval *id)
   Returns the SoundInstance object in zval *id */

SWFSoundInstance getSoundInstance(zval *id TSRMLS_DC)
{
	void *inst = SWFgetProperty(id, "soundinstance", 13, le_swfsoundinstancep TSRMLS_CC);

	if (!inst)
		php_error(E_ERROR, "called object is not an SWFSoundInstance!");

	return (SWFSoundInstance)inst;
}
/* }}} */

/* {{{ swfsoundinstance_nomultiple */

PHP_METHOD(swfsoundinstance, noMultiple)
{
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFSoundInstance_setNoMultiple(inst);
}
/* }}} */

/* {{{ swfsoundinstance_loopinpoint(point) */

PHP_METHOD(swfsoundinstance, loopInPoint)
{
	zval **zpoint;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &zpoint) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zpoint);

	SWFSoundInstance_setLoopInPoint(inst, Z_LVAL_PP(zpoint));
}

/* }}} */
/* {{{ swfsoundinstance_loopoutpoint(point) */

PHP_METHOD(swfsoundinstance, loopOutPoint)
{
	zval **zpoint;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &zpoint) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zpoint);

	SWFSoundInstance_setLoopOutPoint(inst, Z_LVAL_PP(zpoint));
}
/* }}} */

/* {{{ swfsoundinstance_loopcount(point) */

PHP_METHOD(swfsoundinstance, loopCount)
{
	zval **zcount;
	SWFSoundInstance inst = getSoundInstance(getThis() TSRMLS_CC);

	if((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &zcount) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zcount);

	SWFSoundInstance_setLoopCount(inst, Z_LVAL_PP(zcount));
}
/* }}} */

static zend_function_entry swfsoundinstance_functions[] = {
	PHP_ME(swfsoundinstance, noMultiple,    NULL, 0)
	PHP_ME(swfsoundinstance, loopInPoint,   NULL, 0)
	PHP_ME(swfsoundinstance, loopOutPoint,  NULL, 0)
	PHP_ME(swfsoundinstance, loopCount,     NULL, 0)
	{ NULL, NULL, NULL }
};

/* {{{ SWFVideoStream */

/* {{{ proto class swfvideostream_init([file])
   Returns a SWVideoStream object */

PHP_METHOD(swfvideostream, __construct)
{
	zval **zfile = NULL;
	SWFVideoStream stream;
	SWFInput input;
	int ret;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &zfile) == FAILURE)
				WRONG_PARAM_COUNT;
	
			if(Z_TYPE_PP(zfile) != IS_RESOURCE)
  			{
			    convert_to_string_ex(zfile);
			    if(strcasecmp(".flv", Z_STRVAL_PP(zfile)+Z_STRLEN_PP(zfile)-4) == 0)
				input = newSWFInput_filename(Z_STRVAL_PP(zfile));
			    else // keep fingers crossed that it is file_get_contents() 
				input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile)); 
			    zend_list_addref(zend_list_insert(input, le_swfinputp));
  			}
  			else
			{
			    input = getInput(zfile TSRMLS_CC);
			}
		
			stream = newSWFVideoStream_fromInput(input);
			break;
		case 0:
			stream = newSWFVideoStream();
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if(stream) {
		ret = zend_list_insert(stream, le_swfvideostreamp);
		object_init_ex(getThis(), videostream_class_entry_ptr);
		add_property_resource(getThis(), "videostream", ret);
		zend_list_addref(ret);
	}
	
}	

static void destroy_SWFVideoStream_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFVideoStream((SWFVideoStream)resource->ptr);
}
/* }}} */

/* {{{ internal function getVideoStream
   Returns the SWFVideoStream object contained in zval *id */
                                                                                                                                             
static SWFVideoStream getVideoStream(zval *id TSRMLS_DC)
{
	void *stream = SWFgetProperty(id, "videostream", 11, le_swfvideostreamp TSRMLS_CC);
	                                                                                                                                         
	if(!stream)
		php_error(E_ERROR, "called object is not an SWFVideoStream!");
	                                                                                                                                         
	return (SWFVideoStream)stream;
}

/* }}} */

/* {{{ setDimension */

PHP_METHOD(swfvideostream, setdimension)
{
	zval **x, **y;
	SWFVideoStream stream = getVideoStream(getThis() TSRMLS_CC);
	if(!stream)
		 php_error(E_ERROR, "getVideoSTream returned NULL");

	if( ZEND_NUM_ARGS() != 2 
			|| zend_get_parameters_ex(2, &x, &y) == FAILURE )
		WRONG_PARAM_COUNT;

	convert_to_long_ex(x);
	convert_to_long_ex(y);

	SWFVideoStream_setDimension(stream, Z_LVAL_PP(x), Z_LVAL_PP(y));
}
/* }}} */

/* {{{ getNumFrames */
PHP_METHOD(swfvideostream, getnumframes) 
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	RETURN_LONG(SWFVideoStream_getNumFrames(getVideoStream(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ hasAudio */
PHP_METHOD(swfvideostream, hasaudio) 
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	RETURN_LONG(SWFVideoStream_hasAudio(getVideoStream(getThis() TSRMLS_CC)));
}
/* }}} */

		
		
static zend_function_entry swfvideostream_functions[] = {
	PHP_ME(swfvideostream, 	__construct,	NULL, 0)
	PHP_ME(swfvideostream, setdimension, NULL, 0)
	PHP_ME(swfvideostream, getnumframes, NULL, 0)
	PHP_ME(swfvideostream, hasaudio, NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFBinaryData
 */

/* {{{ proto class swfbinarydata_init(string, length)
    Returns a SWFBinaryData object */
PHP_METHOD(swfbinarydata, __construct)
{
	zval **zdata, **zlen;
	SWFBinaryData bd = NULL;
	int ret;

	switch(ZEND_NUM_ARGS()) {
		case 2:
			if(zend_get_parameters_ex(2, &zdata, &zlen) == FAILURE)
				WRONG_PARAM_COUNT;
	
			/* this is not perfect for binary data... but will work for now */
			convert_to_string_ex(zdata);
			convert_to_long_ex(zlen);		
			bd = newSWFBinaryData(Z_STRVAL_PP(zdata), Z_LVAL_PP(zlen));
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if(bd) {
		ret = zend_list_insert(bd, le_swfbinarydatap);
		object_init_ex(getThis(), binarydata_class_entry_ptr);
		add_property_resource(getThis(), "binarydata", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ internal function getBinaryData
   Returns the SWFBinaryData object contained in zval *id */
static SWFBinaryData getBinaryData(zval *id TSRMLS_DC)
{
	void *bd = SWFgetProperty(id, "binarydata", strlen("binarydata"), le_swfbinarydatap TSRMLS_CC);
	if(!bd)
		php_error(E_ERROR, "called object is not an SWFBinaryData!");
	return (SWFBinaryData)bd;
}
/* }}} */

/* {{{ internal function destroy_SWFBinaryData */
static void destroy_SWFBinaryData_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFBinaryData((SWFBinaryData)resource->ptr);
}
/* }}} */

static zend_function_entry swfbinarydata_functions[] = {
	PHP_ME(swfbinarydata, __construct, NULL, 0)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SWFPrebuiltClip */
/* {{{ proto class swfprebuiltclip_init([file])
    Returns a SWFPrebuiltClip object */

PHP_METHOD(swfprebuiltclip, __construct)
{
	zval **zfile = NULL;
	SWFPrebuiltClip clip;
	SWFInput input;
	int ret;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &zfile) == FAILURE)
				WRONG_PARAM_COUNT;
	
			if(Z_TYPE_PP(zfile) != IS_RESOURCE)
   			{
			    convert_to_string_ex(zfile);
			    input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
			    zend_list_addref(zend_list_insert(input, le_swfinputp));
   			}
   			else
			    input = getInput(zfile TSRMLS_CC);
		
			clip = newSWFPrebuiltClip_fromInput(input);
			break;
/* not sure whether this makes sense
   there would have to be a function to add contents
		case 0:
			clip = newSWFPrebuiltClip();
			break; */
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if(clip) {
		ret = zend_list_insert(clip, le_swfprebuiltclipp);
		object_init_ex(getThis(), prebuiltclip_class_entry_ptr);
		add_property_resource(getThis(), "prebuiltclip", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ internal function destroy_SWFPrebuiltClip */
static void destroy_SWFPrebuiltClip_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFPrebuiltClip((SWFPrebuiltClip)resource->ptr);
}
/* }}} */

/* {{{ internal function getPrebuiltClip
   Returns the SWFPrebuiltClip object contained in zval *id */
                                                                                                                                             
static SWFPrebuiltClip getPrebuiltClip(zval *id TSRMLS_DC)
{
	void *clip = SWFgetProperty(id, "prebuiltclip", 12, le_swfprebuiltclipp TSRMLS_CC);
                                                                     
	if(!clip)
		php_error(E_ERROR, "called object is not an SWFPrebuiltClip!");
                                                                                                                                             
	return (SWFPrebuiltClip)clip;
}

/* }}} */
static zend_function_entry swfprebuiltclip_functions[] = {
	PHP_ME(swfprebuiltclip, __construct, NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */
#endif

/* }}} */


/* {{{ SWFMovie
*/
/* {{{ proto void swfmovie::__construct(int version) 
  Creates swfmovie object according to the passed version */
PHP_METHOD(swfmovie, __construct)
{
	zval **version;
	SWFMovie movie;
	int ret;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &version) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(version);
		movie = newSWFMovieWithVersion(Z_LVAL_PP(version));
	} else {
		movie = newSWFMovie(); /* default version 4 */
	}
	
	ret = zend_list_insert(movie, le_swfmoviep);
	
	object_init_ex(getThis(), movie_class_entry_ptr);
	add_property_resource(getThis(), "movie", ret);
	zend_list_addref(ret);
}

static void destroy_SWFMovie_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFMovie((SWFMovie)resource->ptr);
}
/* }}} */

/* {{{ getMovie
*/
static SWFMovie getMovie(zval *id TSRMLS_DC)
{
	void *movie = SWFgetProperty(id, "movie", 5, le_swfmoviep TSRMLS_CC);

	if (!movie) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFMovie");
	}
	return (SWFMovie)movie;
}
/* }}} */

/* {{{ proto void swfmovie::nextframe()  
*/
PHP_METHOD(swfmovie, nextFrame)
{
	SWFMovie_nextFrame(getMovie(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfmovie::labelframe(string label)
   Labels frame */
PHP_METHOD(swfmovie, labelFrame)
{
	zval **label;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &label) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(label);
	SWFMovie_labelFrame(getMovie(getThis() TSRMLS_CC), Z_STRVAL_PP(label));
}
/* }}} */

/* {{{ proto void swfmovie::namedanchor()
*/
PHP_METHOD(swfmovie, namedAnchor)
{
  zval **label;

  if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &label) == FAILURE) {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(label);

  SWFMovie_namedAnchor(getMovie(getThis() TSRMLS_CC), Z_STRVAL_PP(label));
}
/* }}} */

/* {{{ proto void swfmovie::protect([ string pasword])
*/
PHP_METHOD(swfmovie, protect)
{
  zval **zchar;
  SWFMovie movie = getMovie(getThis() TSRMLS_CC);

  switch(ZEND_NUM_ARGS() ) {
  case 0:
    SWFMovie_protect(movie,NULL);
  	break;
  case 1:
	if( zend_get_parameters_ex(1, &zchar) == FAILURE) {
      WRONG_PARAM_COUNT;
    }
	convert_to_string_ex(zchar);
    SWFMovie_protect(movie,Z_STRVAL_PP(zchar));
  	break;
  default:
      WRONG_PARAM_COUNT;
  }
}
/* }}} */

/* {{{ proto object swfmovie::add(object SWFBlock) 
*/
PHP_METHOD(swfmovie, add)
{
	zval **zchar;
	int ret;
	SWFBlock block;
	SWFDisplayItem item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);

	/* XXX - SWFMovie_add deals w/ all block types.  Probably will need to add that.. */
	if (Z_OBJCE_PP(zchar) == action_class_entry_ptr) {
		block = (SWFBlock) getAction(*zchar TSRMLS_CC);
	} 
	else if(Z_OBJCE_PP(zchar) == character_class_entry_ptr) {
		block = (SWFBlock) getCharacterClass(*zchar TSRMLS_CC);
	}
	else {
		block = (SWFBlock) getCharacter(*zchar TSRMLS_CC);
	}

	item = SWFMovie_add_internal(movie, (SWFMovieBlockType)block);
	if (item != NULL) {
		/* try and create a displayitem object */
		ret = zend_list_insert(item, le_swfdisplayitemp);
		object_init_ex(return_value, displayitem_class_entry_ptr);
		add_property_resource(return_value, "displayitem", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ proto void swfmovie::remove(object SWFBlock)
*/
PHP_METHOD(swfmovie, remove)
{
	zval **zchar;
	SWFDisplayItem item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(zchar);
	item = getDisplayItem(*zchar TSRMLS_CC);
	SWFMovie_remove(movie, item);
}
/* }}} */

/* {{{ proto int swfmovie::output([int compression])
*/
static void phpByteOutputMethod(byte b, void *data)
{
	TSRMLS_FETCH();

	php_write(&b, 1 TSRMLS_CC);
}

PHP_METHOD(swfmovie, output)
{
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
#if !defined(HAVE_NEW_MING)
	zval **zlimit = NULL;
	int limit = -1;
	int argc = ZEND_NUM_ARGS();
	int oldval = INT_MIN;
	long out; 

	if(argc) {
		if (zend_get_parameters_ex(1, &zlimit) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);
	}
	oldval = Ming_setSWFCompression(limit);			
	out = SWFMovie_output(movie, &phpByteOutputMethod, NULL);
	Ming_setSWFCompression(oldval);
		
	RETURN_LONG(out);
#elif defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	zval **zlimit = NULL;
	int limit = -1;
	int argc = ZEND_NUM_ARGS();

	if(argc) {
		if (zend_get_parameters_ex(1, &zlimit) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);

		if ((limit < 0) || (limit > 9)) {
			php_error(E_WARNING,"compression level must be within 0..9");
			RETURN_FALSE;
		}
	}
	RETURN_LONG(SWFMovie_output(movie, &phpByteOutputMethod, NULL, limit));
#else
	RETURN_LONG(SWFMovie_output(movie, &phpByteOutputMethod, NULL));
#endif

}
/* }}} */

/* {{{ proto int swfmovie::saveToFile(stream x [, int compression])
*/
static void phpStreamOutputMethod(byte b, void * data)
{
	TSRMLS_FETCH();

	php_stream_write((php_stream*)data, &b, 1);
}

/* I'm not sure about the logic here as Ming_setSWFCompression() should be
 * used with current Ming. I'll have to add some tests to the test suite to
 * verify this functionallity before I can say for sure
 */
PHP_METHOD(swfmovie, saveToFile)
{
	zval **x;
	zval **zlimit = NULL;
	int limit = -1;
#if !defined(HAVE_NEW_MING)
	int oldval = INT_MIN;
#endif
	long out;
	
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	php_stream *what;

	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &x) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 2:
		if (zend_get_parameters_ex(2, &x, &zlimit) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);
		if ((limit < 0) || (limit > 9)) {
			php_error(E_WARNING,"compression level must be within 0..9");
			RETURN_FALSE;
		}
#if !defined(HAVE_NEW_MING)
		oldval = Ming_setSWFCompression(limit);
#endif
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(what, php_stream *, x, -1,"File-Handle",php_file_le_stream());
#if !defined(HAVE_NEW_MING)
	out = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, what);
	Ming_setSWFCompression(oldval);
#elif defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	out = SWFMovie_output(movie, &phpStreamOutputMethod, what, limit);
#else
	out = SWFMovie_output(movie, &phpStreamOutputMethod, what);
#endif
	RETURN_LONG(out);
}
/* }}} */

/* {{{ proto int swfmovie::save(mixed where [, int compression])
  Saves the movie. 'where' can be stream and the movie will be saved there otherwise it is treated as string and written in file with that name */
PHP_METHOD(swfmovie, save)
{
	zval **x;
	zval **zlimit = NULL;
	int limit = -1;
#if !defined(HAVE_NEW_MING)
	int oldval = INT_MIN;
#endif
	long retval;
	php_stream *stream;

	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &x) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &x, &zlimit) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);
#if !defined(HAVE_NEW_MING)
		oldval = Ming_setSWFCompression(limit);
#endif
		break;
	default:
		WRONG_PARAM_COUNT;
	}
		  
	if (Z_TYPE_PP(x) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(stream, php_stream *, x, -1,"File-Handle",php_file_le_stream());
#if defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
		RETURN_LONG(SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, stream, limit));
#else
		RETVAL_LONG(SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, stream));
#if !defined(HAVE_NEW_MING)
		Ming_setSWFCompression(oldval);
#endif
		return;
#endif
	}

	convert_to_string_ex(x);
	stream = php_stream_open_wrapper(Z_STRVAL_PP(x), "wb", REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);

	if (stream == NULL) {
		RETURN_FALSE;
	}
	
#if defined(HAVE_NEW_MING) && defined(HAVE_MING_MOVIE_LEVEL)
	retval = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, (void *)stream, limit);
#else
	retval = SWFMovie_output(getMovie(getThis() TSRMLS_CC), &phpStreamOutputMethod, (void *)stream);
#endif
	php_stream_close(stream);
#if!defined(HAVE_NEW_MING)
	Ming_setSWFCompression(oldval);
#endif
    
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto void swfmovie::setBackground(int r, int g, int b)
   Sets background color (r,g,b) */
PHP_METHOD(swfmovie, setBackground)
{
	zval **r, **g, **b;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);
	SWFMovie_setBackground(movie, Z_LVAL_PP(r), Z_LVAL_PP(g), Z_LVAL_PP(b));
}
/* }}} * /

/* {{{ proto void swfmovie::setRate(float rate)
   Sets movie rate */
PHP_METHOD(swfmovie, setRate)
{
	zval **rate;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &rate) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(rate);
	SWFMovie_setRate(movie, Z_DVAL_PP(rate));
}
/* }}} */

/* {{{ proto void swfmovie::setDimension(float x, float y)
   Sets movie dimension */
PHP_METHOD(swfmovie, setDimension)
{
	zval **x, **y;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFMovie_setDimension(movie, Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfmovie::setFrames(int frames)
   Sets number of frames */
PHP_METHOD(swfmovie, setFrames)
{
	zval **frames;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &frames) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(frames);
	SWFMovie_setNumberOfFrames(movie, Z_LVAL_PP(frames));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swfmovie::addMetadata(string xml)
 * */
PHP_METHOD(swfmovie, addMetadata)
{ 
  zval **zchar;
  SWFMovie movie = getMovie(getThis() TSRMLS_CC);
  
  if( ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
        WRONG_PARAM_COUNT;
  } 
  convert_to_string_ex(zchar);
  SWFMovie_addMetadata(movie, Z_STRVAL_PP(zchar));
}
/* }}} */

/* {{{ proto void swfmovie::setScriptLimits(int maxRecursion, int timeout)
*/
PHP_METHOD(swfmovie, setScriptLimits)
{
  zval **maxRecursion, **timeout;
  SWFMovie movie = getMovie(getThis() TSRMLS_CC);

  if( ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &maxRecursion, &timeout) == FAILURE) {
        WRONG_PARAM_COUNT; 
  } 
  convert_to_long_ex(maxRecursion);
  convert_to_long_ex(timeout);
  SWFMovie_setScriptLimits(movie, Z_LVAL_PP(maxRecursion), Z_LVAL_PP(timeout));  
}
/* }}} */

/* {{{ proto void swfmovie::setTabIndex(int depth, int index)
 * */
PHP_METHOD(swfmovie, setTabIndex)
{
  zval **depth, **index;
  SWFMovie movie = getMovie(getThis() TSRMLS_CC);
  
  if( ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &depth, &index) == FAILURE) {
        WRONG_PARAM_COUNT; 
  } 
  convert_to_long_ex(depth);
  convert_to_long_ex(index);
  SWFMovie_setTabIndex(movie, Z_LVAL_PP(depth), Z_LVAL_PP(index));
}
/* }}} */

/* {{{ proto void swfmovie::assignSymbol(SWFCharacter character, string name)
 * */
PHP_METHOD(swfmovie, assignSymbol)
{
  zval **zchar, **name;
  SWFCharacter character;
  SWFMovie movie = getMovie(getThis() TSRMLS_CC);
  
  if( ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zchar, &name) == FAILURE) 
        WRONG_PARAM_COUNT; 
   
  convert_to_string_ex(name);
  convert_to_object_ex(zchar);
  character = getCharacter(*zchar TSRMLS_CC);
  SWFMovie_assignSymbol(movie, character, Z_STRVAL_PP(name));
}
/* }}} */

/* {{{ proto void swfmovie::defineScene(int offset, string name)
 * */
PHP_METHOD(swfmovie, defineScene)
{
  zval **offset, **name;
  SWFMovie movie = getMovie(getThis() TSRMLS_CC);
  
  if( ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &offset, &name) == FAILURE) 
        WRONG_PARAM_COUNT; 
   
  convert_to_string_ex(name);
  convert_to_long_ex(offset);
  SWFMovie_defineScene(movie, Z_LVAL_PP(offset), Z_STRVAL_PP(name));
}
/* }}} */

/* {{{ proto void swfmovie::setNetworkAccess(int flag)
 * */  
PHP_METHOD(swfmovie, setNetworkAccess)
{   
  zval **flag;
  SWFMovie movie = getMovie(getThis() TSRMLS_CC);
    
  if( ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &flag) == FAILURE) {
        WRONG_PARAM_COUNT;
  }  
  convert_to_long_ex(flag);
  SWFMovie_setNetworkAccess(movie, Z_LVAL_PP(flag));
} 
/* }}} */

/* {{{ proto long swfmovie::streamMP3(mixed file)
   Sets sound stream of the SWF movie. The parameter can be stream or string. */
PHP_METHOD(swfmovie, streamMP3)
{
	zval **zfile, **zskip;
	float skip;
	SWFSoundStream sound;
	SWFInput input;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

    switch (ZEND_NUM_ARGS()) {
    case 1:
  	  if(zend_get_parameters_ex(1, &zfile) == FAILURE) {
		WRONG_PARAM_COUNT;
	  }
	  skip = 0;
	  break;
    case 2:
	  if(zend_get_parameters_ex(2, &zfile, &zskip) == FAILURE) {
		 WRONG_PARAM_COUNT;
	  }
	  convert_to_double_ex(zskip);
	  skip = Z_DVAL_PP(zskip);
	  break;
    default:
      WRONG_PARAM_COUNT;
    }
	
	if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
		convert_to_string_ex(zfile);
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	} else {
		input = getInput(zfile TSRMLS_CC);
	}
	
	sound = newSWFSoundStream_fromInput(input);
	SWFMovie_setSoundStreamAt(movie, sound, skip);
	RETURN_LONG(SWFSoundStream_getDuration(sound) / SWFMovie_getRate(movie));
}
/* }}} */

/* {{{ proto long swfmovie::setSoundStream
   Sets sound stream of the SWF movie. The parameter must be a SoundStream object */
PHP_METHOD(swfmovie, setSoundStream)
{
	zval **zstream, **zskip;
	float skip;
	SWFSoundStream sound;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);
	switch (ZEND_NUM_ARGS()) 
	{
	case 1:
		if(zend_get_parameters_ex(1, &zstream) == FAILURE) 
			WRONG_PARAM_COUNT;
		skip = 0;
		break;
	case 2:
		if(zend_get_parameters_ex(1, &zstream, &zskip) == FAILURE) 
			WRONG_PARAM_COUNT;
		convert_to_double_ex(zskip);
		skip = Z_DVAL_PP(zskip);
		break;
	default:
		WRONG_PARAM_COUNT;	
	}

	convert_to_object_ex(zstream);
	sound = getSoundStream(*zstream TSRMLS_CC);	
	SWFMovie_setSoundStreamAt(movie, sound, skip);
	RETURN_LONG(SWFSoundStream_getDuration(sound) / SWFMovie_getRate(movie));
}
/* }}} */

/* {{{ swfmovie_addexport */

PHP_METHOD(swfmovie, addExport)
{
	zval **zchar, **zname;
	SWFBlock block;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zchar, &zname) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zchar);
	convert_to_string_ex(zname);

	block = (SWFBlock)getCharacter(*zchar TSRMLS_CC);

	SWFMovie_addExport(movie, block,  Z_STRVAL_PP(zname));
}

/* }}} */

/* {{{ swfmovie_writeexports */
			
PHP_METHOD(swfmovie, writeExports)
{
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	SWFMovie_writeExports(movie);
}

/* }}} */
/* {{{ SWFSoundInstance swfmovie_startsound */

PHP_METHOD(swfmovie, startSound)
{
	zval **zsound;
	int ret;
	SWFSound sound;
	SWFSoundInstance item;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zsound) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = (SWFSound)getSound(*zsound TSRMLS_CC);

	item = SWFMovie_startSound(movie, sound);

	if(item != NULL)
	{
		/* try and create a soundinstance object */
		ret = zend_list_insert(item, le_swfsoundinstancep);
		object_init_ex(return_value, soundinstance_class_entry_ptr);
		add_property_resource(return_value, "soundinstance", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ void swfmovie::stopsound(sound) */
PHP_METHOD(swfmovie, stopSound)
{
	zval **zsound;
	SWFSound sound;
	SWFMovie movie = getMovie(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zsound) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = (SWFSound)getSound(*zsound TSRMLS_CC);

	SWFMovie_stopSound(movie, sound);
}
/* }}} */

/* {{{ void swfmovie::importChar(filename, importname) */
PHP_METHOD(swfmovie, importChar)
{
	SWFMovie movie;
	SWFCharacter character;
	int ret;
	zval **libswf, **name;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &libswf, &name) == FAILURE)
		WRONG_PARAM_COUNT;
	convert_to_string_ex(libswf);
	convert_to_string_ex(name);
	movie = getMovie(getThis() TSRMLS_CC);
	character = SWFMovie_importCharacter(movie, Z_STRVAL_PP(libswf), Z_STRVAL_PP(name));
	if(character != NULL)
	{
		ret = zend_list_insert(character, le_swfcharacterp);
		object_init_ex(return_value, character_class_entry_ptr);
		add_property_resource(return_value, "character", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ void swfmovie_importFont */
PHP_METHOD(swfmovie, importFont)
{
	SWFMovie movie;
	SWFFontCharacter res;
	int ret;
	zval **libswf, **name;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &libswf, &name) == FAILURE)
		WRONG_PARAM_COUNT;
	convert_to_string_ex(libswf);
	convert_to_string_ex(name);
	movie = getMovie(getThis() TSRMLS_CC);
	PHP_MING_FILE_CHK(Z_STRVAL_PP(libswf));
	res = SWFMovie_importFont(movie, Z_STRVAL_PP(libswf), Z_STRVAL_PP(name));

	if(res != NULL)
	{
		/* try and create a fontchar object */
		ret = zend_list_insert(res, le_swffontcharp);
		object_init_ex(return_value, fontchar_class_entry_ptr);
		add_property_resource(return_value, "fontcharacter", ret);
		zend_list_addref(ret);
	}	
}
/* }}} */

/* {{{ void swfmovie_addFont */
PHP_METHOD(swfmovie, addFont)
{
	SWFMovie movie;
	SWFFontCharacter res;
	int ret;
	SWFFont font;
	zval **zfont;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zfont) == FAILURE)
	    WRONG_PARAM_COUNT;

	convert_to_object_ex(zfont);

	movie = getMovie(getThis() TSRMLS_CC);
	font = getFont(*zfont TSRMLS_CC);
	res = SWFMovie_addFont(movie, font);
	if(res != NULL)
	{
		/* try and create a fontchar object */
		ret = zend_list_insert(res, le_swffontcharp);
		object_init_ex(return_value, fontchar_class_entry_ptr);
		add_property_resource(return_value, "fontcharacter", ret);
		zend_list_addref(ret);
	}	
}
/* }}} */

/* {{{ void swfmovie_replace */
PHP_METHOD(swfmovie, replace)
{
	SWFMovie movie;
	zval **zitem, **zblock;
	SWFDisplayItem item;
	SWFBlock block;
	SWFMovieBlockType ublock;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zitem, &zblock) == FAILURE)
	    WRONG_PARAM_COUNT;
	
	convert_to_object_ex(zitem);
	convert_to_object_ex(zblock);
	item = getDisplayItem(*zitem TSRMLS_CC);

	block = (SWFBlock) getCharacter(*zblock TSRMLS_CC);
	movie =  getMovie(getThis() TSRMLS_CC);
	ublock.block = block;

	SWFMovie_replace_internal(movie, item, ublock);
}
/* }}} */
#endif

static zend_function_entry swfmovie_functions[] = {
	PHP_ME(swfmovie, __construct,       NULL, 0)
	PHP_ME(swfmovie, nextFrame,         NULL, 0)
	PHP_ME(swfmovie, labelFrame,        NULL, 0)
	PHP_ME(swfmovie, add,               NULL, 0)
	PHP_ME(swfmovie, remove,            NULL, 0)
	PHP_ME(swfmovie, output,            NULL, 0)
	PHP_ME(swfmovie, save,              NULL, 0)
	PHP_ME(swfmovie, saveToFile,        NULL, 0)
	PHP_ME(swfmovie, setBackground,     NULL, 0)
	PHP_ME(swfmovie, setRate,           NULL, 0)
	PHP_ME(swfmovie, setDimension,      NULL, 0)
	PHP_ME(swfmovie, setFrames,         NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfmovie, streamMP3,         NULL, 0)
	PHP_ME(swfmovie, setSoundStream,    NULL, 0)
	PHP_ME(swfmovie, addExport,         NULL, 0)
	PHP_ME(swfmovie, writeExports,      NULL, 0)
	PHP_ME(swfmovie, startSound,        NULL, 0)
	PHP_ME(swfmovie, stopSound,         NULL, 0)
	PHP_ME(swfmovie, importChar,        NULL, 0)
	PHP_ME(swfmovie, importFont,        NULL, 0)
	PHP_ME(swfmovie, addFont,           NULL, 0)
	PHP_ME(swfmovie, protect,           NULL, 0)
	PHP_ME(swfmovie, addMetadata,		NULL, 0)
	PHP_ME(swfmovie, setNetworkAccess,	NULL, 0)
	PHP_ME(swfmovie, setScriptLimits,	NULL, 0)
	PHP_ME(swfmovie, setTabIndex,		NULL, 0)
	PHP_ME(swfmovie, assignSymbol,		NULL, 0)
	PHP_ME(swfmovie, defineScene,		NULL, 0)
	PHP_ME(swfmovie, namedAnchor,		NULL, 0)
	PHP_ME(swfmovie, replace,		NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFShape
*/
/* {{{ proto void swfshape::__construct()
   Creates a new SWFShape object */
PHP_METHOD(swfshape, __construct)
{
	SWFShape shape = newSWFShape();
	int ret = zend_list_insert(shape, le_swfshapep);

	object_init_ex(getThis(), shape_class_entry_ptr);
	add_property_resource(getThis(), "shape", ret);
	zend_list_addref(ret);
}

static void destroy_SWFShape_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFShape((SWFShape)resource->ptr);
}
/* }}} */

/* {{{ internal function getShape
   Returns the SWFShape object contained in zval *id */
static SWFShape getShape(zval *id TSRMLS_DC)
{
	void *shape = SWFgetProperty(id, "shape", 5, le_swfshapep TSRMLS_CC);

	if (!shape) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFShape");
	}

	return (SWFShape)shape;
}
/* }}} */

/* {{{ proto void swfshape::setline(int width, int r, int g, int b [, int a])
   Sets the current line style for this SWFShape */
PHP_METHOD(swfshape, setLine)
{
	zval **w, **r, **g, **b, **a;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &w, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 5) {
		if (zend_get_parameters_ex(5, &w, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(a);
	} else if (ZEND_NUM_ARGS() == 1) {
		SWFShape_setLine(getShape(getThis() TSRMLS_CC), 0, 0, 0, 0, 0);
		return;
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(w);
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	if (ZEND_NUM_ARGS() == 4) {
		SWFShape_setLine(getShape(getThis() TSRMLS_CC),
			(unsigned short)Z_LVAL_PP(w), BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), 0xff
		);
	} else {
		SWFShape_setLine(getShape(getThis() TSRMLS_CC),
			(unsigned short)Z_LVAL_PP(w), BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), BYTE_Z_LVAL_PP(a)
		);
	}
}
/* }}} */

/* {{{ proto object swfshape::addfill(mixed arg1, int arg2, [int b [, int a]])
   Returns a fill object, for use with swfshape_setleftfill and swfshape_setrightfill. If 1 or 2 parameter(s) is (are) passed first should be object (from gradient class) and the second int (flags). Gradient fill is performed. If 3 or 4 parameters are passed : r, g, b [, a]. Solid fill is performed. */
PHP_METHOD(swfshape, addFill)
{
	SWFFill fill=NULL;
	int ret;

	if (ZEND_NUM_ARGS() == 1 || ZEND_NUM_ARGS() == 2) {
		/* it's a gradient or bitmap */
		zval **arg1;
		unsigned char flags = 0;

		if (ZEND_NUM_ARGS() == 2) {
			zval **arg2;
			if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(arg2);
			flags = (unsigned char)Z_LVAL_PP(arg2);
		} else {
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
		}

		convert_to_object_ex(arg1);

		if (Z_OBJCE_PP(arg1) == gradient_class_entry_ptr) {
			if (flags == 0) {
				flags = SWFFILL_LINEAR_GRADIENT;
			}
			fill = SWFShape_addGradientFill(getShape(getThis() TSRMLS_CC), getGradient(*arg1 TSRMLS_CC), flags);
		} else if (Z_OBJCE_PP(arg1) == bitmap_class_entry_ptr) {
			if (flags == 0) {
				flags = SWFFILL_TILED_BITMAP;
			}
			fill = SWFShape_addBitmapFill(getShape(getThis() TSRMLS_CC), getBitmap(*arg1 TSRMLS_CC), flags);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Argument is not a bitmap nor a gradient");
		}

	} else if (ZEND_NUM_ARGS() == 3 || ZEND_NUM_ARGS() == 4) {
		/* it's a solid fill */
		zval **r, **g, **b, **za;
		byte a = 0xff;

		if (ZEND_NUM_ARGS() == 3) {
			if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
		} else if (ZEND_NUM_ARGS() == 4) {
			if (zend_get_parameters_ex(4, &r, &g, &b, &za) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(za);
			a = BYTE_Z_LVAL_PP(za);
		} else {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC),
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), a
		);
	} else {
		WRONG_PARAM_COUNT;
	}
	
	if (!fill) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error adding fill to shape");
	}

	/* return an SWFFill object */
	ret = zend_list_insert(fill, le_swffillp);
	object_init_ex(return_value, fill_class_entry_ptr);
	add_property_resource(return_value, "fill", ret);
	zend_list_addref(ret);
}
/* }}} */

/* {{{ proto void swfshape::setleftfill(int arg1 [, int g ,int b [,int a]])
   Sets the left side fill style to fill in case only one parameter is passed. When 3 or 4 parameters are passed they are treated as : int r, int g, int b, int a . Solid fill is performed in this case before setting left side fill type. */
PHP_METHOD(swfshape, setLeftFill)
{
	zval **zfill, **r, **g, **b, **a;
	SWFFill fill;

	if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);
		
		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC), 
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), 0xff
		);

	} else if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);
		convert_to_long_ex(a);

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC),
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), BYTE_Z_LVAL_PP(a)
		);

	} else if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &zfill) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		
		if (Z_LVAL_PP(zfill) != 0) {
			convert_to_object_ex(zfill);
			fill = getFill(*zfill TSRMLS_CC);
		} else {
			fill = NULL;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	SWFShape_setLeftFill(getShape(getThis() TSRMLS_CC), fill);
}
/* }}} */

/* {{{ proto void swfshape::setleftfill(int arg1 [, int g ,int b [,int a]])
   Sets the right side fill style to fill in case only one parameter is passed. When 3 or 4 parameters are passed they are treated as : int r, int g, int b, int a . Solid fill is performed in this case before setting right side fill type. */
PHP_METHOD(swfshape, setRightFill)
{
	zval **zfill, **r, **g, **b, **a;
	SWFFill fill;

	if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC),
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), 0xff
		);

	} else if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(r);
		convert_to_long_ex(g);
		convert_to_long_ex(b);
		convert_to_long_ex(a);

		fill = SWFShape_addSolidFill(getShape(getThis() TSRMLS_CC),
			BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), BYTE_Z_LVAL_PP(a)
		);
	} else if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &zfill) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		if (Z_LVAL_PP(zfill) != 0) {
			convert_to_object_ex(zfill);
			fill = getFill(*zfill TSRMLS_CC);
		} else {
			fill = NULL;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
	
	SWFShape_setRightFill(getShape(getThis() TSRMLS_CC), fill);
}
/* }}} */

/* {{{ proto void swfshape::movepento(double x, double y)
   Moves the pen to shape coordinates (x, y) */
PHP_METHOD(swfshape, movePenTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFShape_movePenTo(getShape(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfshape::movepen(double x, double y)
   Moves the pen from its current location by vector (x, y) */
PHP_METHOD(swfshape, movePen)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFShape_movePen(getShape(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfshape::drawlineto(double x, double y)
   Draws a line from the current pen position to shape coordinates (x, y) in the current line style */
PHP_METHOD(swfshape, drawLineTo)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFShape_drawLineTo(getShape(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfshape::drawline(double dx, double dy)
   Draws a line from the current pen position (x, y) to the point (x+dx, y+dy) in the current line style */
PHP_METHOD(swfshape, drawLine)
{
	zval **x, **y;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFShape_drawLine(getShape(getThis() TSRMLS_CC), Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swfshape::drawcurveto(double ax, double ay, double bx, double by [, double dx, double dy])
   Draws a curve from the current pen position (x,y) to the point (bx, by) in the current line style, using point (ax, ay) as a control point. Or draws a cubic bezier to point (dx, dy) with control points (ax, ay) and (bx, by) */
PHP_METHOD(swfshape, drawCurveTo)
{
	if (ZEND_NUM_ARGS() == 4) {
		zval **cx, **cy, **ax, **ay;

		if (zend_get_parameters_ex(4, &cx, &cy, &ax, &ay) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(cx);
		convert_to_double_ex(cy);
		convert_to_double_ex(ax);
		convert_to_double_ex(ay);

		SWFShape_drawCurveTo(getShape(getThis() TSRMLS_CC),
			Z_DVAL_PP(cx), Z_DVAL_PP(cy), Z_DVAL_PP(ax), Z_DVAL_PP(ay)
		);
	
	} else if (ZEND_NUM_ARGS() == 6) {
		zval **bx, **by, **cx, **cy, **dx, **dy;

		if (zend_get_parameters_ex(6, &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(bx);
		convert_to_double_ex(by);
		convert_to_double_ex(cx);
		convert_to_double_ex(cy);
		convert_to_double_ex(dx);
		convert_to_double_ex(dy);

		RETURN_LONG(SWFShape_drawCubicTo(getShape(getThis() TSRMLS_CC), 
			Z_DVAL_PP(bx), Z_DVAL_PP(by), Z_DVAL_PP(cx), Z_DVAL_PP(cy), Z_DVAL_PP(dx), Z_DVAL_PP(dy))
		);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfshape::drawcurve(double adx, double ady, double bdx, double bdy [, double cdx, double cdy])
   Draws a curve from the current pen position (x, y) to the point (x+bdx, y+bdy) in the current line style, using point (x+adx, y+ady) as a control point or draws a cubic bezier to point (x+cdx, x+cdy) with control points (x+adx, y+ady) and (x+bdx, y+bdy) */
PHP_METHOD(swfshape, drawCurve)
{
	if (ZEND_NUM_ARGS() == 4) {
		zval **cx, **cy, **ax, **ay;

		if (zend_get_parameters_ex(4, &cx, &cy, &ax, &ay) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(cx);
		convert_to_double_ex(cy);
		convert_to_double_ex(ax);
		convert_to_double_ex(ay);

		SWFShape_drawCurve(getShape(getThis() TSRMLS_CC), 
			Z_DVAL_PP(cx), Z_DVAL_PP(cy), Z_DVAL_PP(ax), Z_DVAL_PP(ay)
		);

	} else if (ZEND_NUM_ARGS() == 6) {
		zval **bx, **by, **cx, **cy, **dx, **dy;

		if (zend_get_parameters_ex(6, &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_double_ex(bx);
		convert_to_double_ex(by);
		convert_to_double_ex(cx);
		convert_to_double_ex(cy);
		convert_to_double_ex(dx);
		convert_to_double_ex(dy);

		RETURN_LONG( SWFShape_drawCubic(getShape(getThis() TSRMLS_CC),
			 Z_DVAL_PP(bx), Z_DVAL_PP(by), Z_DVAL_PP(cx), Z_DVAL_PP(cy), Z_DVAL_PP(dx), Z_DVAL_PP(dy))
		);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void swfshape::drawglyph(SWFFont font, string character [, int size])
   Draws the first character in the given string into the shape using the glyph definition from the given font */
PHP_METHOD(swfshape, drawGlyph)
{
	zval **font, **c, **zsize;
	int size=0;

	if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &font, &c) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		size = (int)(1024.0f/Ming_getScale());

	} else if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &font, &c, &zsize) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(zsize);
		size = Z_LVAL_PP(zsize);
	}
	convert_to_string_ex(c);
	SWFShape_drawSizedGlyph(getShape(getThis() TSRMLS_CC), getFont(*font TSRMLS_CC), Z_STRVAL_PP(c)[0], size);
}
/* }}} */

/* {{{ proto void swfshape::drawcircle(double r)
   Draws a circle of radius r centered at the current location, in a counter-clockwise fashion */
PHP_METHOD(swfshape, drawCircle)
{
	zval **r;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &r) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(r);
	SWFShape_drawCircle(getShape(getThis() TSRMLS_CC), Z_DVAL_PP(r));
}
/* }}} */

/* {{{ proto void swfshape::drawarc(double r, double startAngle, double endAngle)
   Draws an arc of radius r centered at the current location, from angle startAngle to angle endAngle measured clockwise from 12 o'clock */
PHP_METHOD(swfshape, drawArc)
{
	zval **r, **start, **end;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &r, &start, &end) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(r);
	convert_to_double_ex(start);
	convert_to_double_ex(end);

	/* convert angles to radians, since that's what php uses elsewhere */
	SWFShape_drawArc(getShape(getThis() TSRMLS_CC), Z_DVAL_PP(r), Z_DVAL_PP(start), Z_DVAL_PP(end));
}
/* }}} */

/* {{{ proto void swfshape::drawcubic(double bx, double by, double cx, double cy, double dx, double dy)
   Draws a cubic bezier curve using the current position and the three given points as control points */
PHP_METHOD(swfshape, drawCubic)
{
	zval **bx, **by, **cx, **cy, **dx, **dy;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(bx);
	convert_to_double_ex(by);
	convert_to_double_ex(cx);
	convert_to_double_ex(cy);
	convert_to_double_ex(dx);
	convert_to_double_ex(dy);

	RETURN_LONG( SWFShape_drawCubic(getShape(getThis() TSRMLS_CC), 
		Z_DVAL_PP(bx), Z_DVAL_PP(by), Z_DVAL_PP(cx), Z_DVAL_PP(cy), Z_DVAL_PP(dx), Z_DVAL_PP(dy))
	);
}
/* }}} */

/* {{{ proto void swfshape::drawcubic(double bx, double by, double cx, double cy, double dx, double dy)
   Draws a cubic bezier curve using the current position and the three given points as control points */
PHP_METHOD(swfshape, drawCubicTo)
{
	zval **bx, **by, **cx, **cy, **dx, **dy;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &bx, &by, &cx, &cy, &dx, &dy) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(bx);
	convert_to_double_ex(by);
	convert_to_double_ex(cx);
	convert_to_double_ex(cy);
	convert_to_double_ex(dx);
	convert_to_double_ex(dy);

	RETURN_LONG( SWFShape_drawCubicTo(getShape(getThis() TSRMLS_CC),
		Z_DVAL_PP(bx), Z_DVAL_PP(by), Z_DVAL_PP(cx), Z_DVAL_PP(cy), Z_DVAL_PP(dx), Z_DVAL_PP(dy))
	);
}
/* }}} */

/* {{{ proto void swfshape::end() */
PHP_METHOD(swfshape, end)
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFShape_end(getShape(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfshape::useVersion() 
 * SWF_SHAPE3
 * SWF_SHAPE4
 */
PHP_METHOD(swfshape, useVersion)
{
	zval **ver;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &ver) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(ver);
	SWFShape_useVersion(getShape(getThis() TSRMLS_CC), Z_LVAL_PP(ver));
}
/* }}} */

/* {{{ proto int swfshape::getVersion() */
PHP_METHOD(swfshape, getVersion)
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;
	
	RETURN_LONG(SWFShape_getVersion(getShape(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto void swfshape::setRenderHintingFlags(flags) 
 * SWF_SHAPE_USESCALINGSTROKES
 * SWF_SHAPE_USENONSCALINGSTROKES
 */
PHP_METHOD(swfshape, setRenderHintingFlags)
{
	zval **flags;
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &flags) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(flags);

	SWFShape_setRenderHintingFlags(getShape(getThis() TSRMLS_CC), Z_LVAL_PP(flags));
}
/* }}} */

/* {{{ proto double swfshape::getPenX() */
PHP_METHOD(swfshape, getPenX)
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	RETURN_DOUBLE(SWFShape_getPenX(getShape(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto double swfshape::getPenY() */
PHP_METHOD(swfshape, getPenY)
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	RETURN_DOUBLE(SWFShape_getPenY(getShape(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto void swfshape::hideLine() */
PHP_METHOD(swfshape, hideLine)
{
	if(ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;

	SWFShape_hideLine(getShape(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfshape::drawCharacterBounds(character) */
PHP_METHOD(swfshape, drawCharacterBounds)
{
	zval **character;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &character) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(character);
	SWFShape_drawCharacterBounds(getShape(getThis() TSRMLS_CC),
		getCharacter(*character TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfshape::setLine2(width, flags, miterLimit, r, g, b, [a]) 
 * Line cap style: select one of the following flags (default is round cap style)
 * SWF_LINESTYLE_CAP_ROUND 
 * SWF_LINESTYLE_CAP_NONE
 * SWF_LINESTYLE_CAP_SQUARE 
 *
 * Line join style: select one of the following flags (default is round join style)
 * SWF_LINESTYLE_JOIN_ROUND
 * SWF_LINESTYLE_JOIN_BEVEL 
 * SWF_LINESTYLE_JOIN_MITER  
 *
 * Scaling flags: disable horizontal / vertical scaling
 * SWF_LINESTYLE_FLAG_NOHSCALE
 * SWF_LINESTYLE_FLAG_NOVSCALE 
 *
 * Enable pixel hinting to correct blurry vertical / horizontal lines
 * -> all anchors will be aligned to full pixels
 * SWF_LINESTYLE_FLAG_HINTING  
 *
 * Disable stroke closure: if no-close flag is set caps will be applied 
 * instead of joins
 * SWF_LINESTYLE_FLAG_NOCLOSE
 *
 * End-cap style: default round
 * SWF_LINESTYLE_FLAG_ENDCAP_ROUND
 * SWF_LINESTYLE_FLAG_ENDCAP_NONE
 * SWF_LINESTYLE_FLAG_ENDCAP_SQUARE
 *
 * If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor 
 * must be set. Miter max length is then calculated as:
 * max miter len = miter limit * width.
 * If join style is not miter, this value will be ignored.
 * */
PHP_METHOD(swfshape, setLine2)
{
	zval **width, **flags, **limit, **r, **g, **b, **a;
	int alpha = 0xff;
	int ret;
	switch(ZEND_NUM_ARGS())
	{
	case 6:
		ret = zend_get_parameters_ex(6, &width, &flags, &limit, &r, &g, &b);
		break;
	case 7:
		ret = zend_get_parameters_ex(7, &width, &flags, &limit, &r, &g, &b, &a);
		convert_to_long_ex(a);
		alpha = Z_LVAL_PP(a);
		break;

	default:
		WRONG_PARAM_COUNT;
	}

	if(ret == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(width);
	convert_to_long_ex(flags);
	convert_to_double_ex(limit);
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	SWFShape_setLine2(getShape(getThis() TSRMLS_CC), Z_LVAL_PP(width), 
		Z_LVAL_PP(r), Z_LVAL_PP(g), Z_LVAL_PP(g), alpha, Z_LVAL_PP(flags),
		Z_DVAL_PP(limit));
}
/* }}} */

/* {{{ proto void swfshape::setLine2Filled(width, fill, flags, limit) 
 * Line cap style: select one of the following flags (default is round cap style)
 * SWF_LINESTYLE_CAP_ROUND 
 * SWF_LINESTYLE_CAP_NONE
 * SWF_LINESTYLE_CAP_SQUARE 
 *
 * Line join style: select one of the following flags (default is round join style)
 * SWF_LINESTYLE_JOIN_ROUND
 * SWF_LINESTYLE_JOIN_BEVEL 
 * SWF_LINESTYLE_JOIN_MITER  
 *
 * Scaling flags: disable horizontal / vertical scaling
 * SWF_LINESTYLE_FLAG_NOHSCALE
 * SWF_LINESTYLE_FLAG_NOVSCALE 
 *
 * Enable pixel hinting to correct blurry vertical / horizontal lines
 * -> all anchors will be aligned to full pixels
 * SWF_LINESTYLE_FLAG_HINTING  
 *
 * Disable stroke closure: if no-close flag is set caps will be applied 
 * instead of joins
 * SWF_LINESTYLE_FLAG_NOCLOSE
 *
 * End-cap style: default round
 * SWF_LINESTYLE_FLAG_ENDCAP_ROUND
 * SWF_LINESTYLE_FLAG_ENDCAP_NONE
 * SWF_LINESTYLE_FLAG_ENDCAP_SQUARE
 *
 * If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor 
 * must be set. Miter max length is then calculated as:
 * max miter len = miter limit * width.
 * If join style is not miter, this value will be ignored.
 */
PHP_METHOD(swfshape, setLine2Filled)
{
	zval **width, **fill, **flags, **limit;
	SWFFill xfill;

	if(ZEND_NUM_ARGS() != 4)
		WRONG_PARAM_COUNT;

	if(zend_get_parameters_ex(4, &width, &fill, &flags, &limit) == FAILURE)
		WRONG_PARAM_COUNT;


	convert_to_long_ex(width);
	convert_to_object_ex(fill);
	convert_to_long_ex(flags);
	convert_to_double_ex(limit);
	xfill = getFill(*fill TSRMLS_CC);
	SWFShape_setLine2Filled(getShape(getThis() TSRMLS_CC), Z_LVAL_PP(width),
		SWFFill_getFillStyle(xfill), Z_LVAL_PP(flags), Z_DVAL_PP(limit));
}
/* }}} */

static zend_function_entry swfshape_functions[] = {
	PHP_ME(swfshape, __construct,        NULL, 0)
	PHP_ME(swfshape, setLine,            NULL, 0)
	PHP_ME(swfshape, addFill,            NULL, 0)
	PHP_ME(swfshape, setLeftFill,        NULL, 0)
	PHP_ME(swfshape, setRightFill,       NULL, 0)
	PHP_ME(swfshape, movePenTo,          NULL, 0)
	PHP_ME(swfshape, movePen,            NULL, 0)
	PHP_ME(swfshape, drawLineTo,         NULL, 0)
	PHP_ME(swfshape, drawLine,           NULL, 0)
	PHP_ME(swfshape, drawCurveTo,        NULL, 0)
	PHP_ME(swfshape, drawCurve,          NULL, 0)
	PHP_ME(swfshape, drawGlyph,          NULL, 0)
	PHP_ME(swfshape, drawCircle,         NULL, 0)
	PHP_ME(swfshape, drawArc,            NULL, 0)
	PHP_ME(swfshape, drawCubic,          NULL, 0)
	PHP_ME(swfshape, drawCubicTo,        NULL, 0)
	PHP_ME(swfshape, end,                NULL, 0)
	PHP_ME(swfshape, useVersion,         NULL, 0)
	PHP_ME(swfshape, setRenderHintingFlags, NULL, 0)
	PHP_ME(swfshape, getPenX,            NULL, 0)
	PHP_ME(swfshape, getPenY,            NULL, 0)
	PHP_ME(swfshape, hideLine,           NULL, 0)
	PHP_ME(swfshape, drawCharacterBounds, NULL, 0)
	PHP_ME(swfshape, setLine2,           NULL, 0)
	PHP_ME(swfshape, setLine2Filled,     NULL, 0)	
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFSprite
*/
/* {{{ proto void swfsprite::__construct()
   Creates a new SWFSprite object */
PHP_METHOD(swfsprite, __construct)
{
	SWFMovieClip sprite = newSWFMovieClip();
	int ret = zend_list_insert(sprite, le_swfspritep);

	object_init_ex(getThis(), sprite_class_entry_ptr);
	add_property_resource(getThis(), "sprite", ret);
	zend_list_addref(ret);
}

static void destroy_SWFSprite_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFMovieClip((SWFMovieClip)resource->ptr);
}
/* }}} */

/* {{{ internal function SWFSprite getSprite(zval *id)
   Returns the SWFSprite object in zval *id */
static SWFMovieClip getSprite(zval *id TSRMLS_DC)
{
	void *sprite = SWFgetProperty(id, "sprite", 6, le_swfspritep TSRMLS_CC);

	if (!sprite) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFSprite");
	}
	return (SWFMovieClip)sprite;
}
/* }}} */

/* {{{ proto object swfsprite::add(object SWFCharacter)
   Adds the character to the sprite, returns a displayitem object */
PHP_METHOD(swfsprite, add)
{
	zval **zchar;
	int ret;
	SWFBlock block;
	SWFDisplayItem item;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_object_ex(zchar);

	if (Z_OBJCE_PP(zchar) == action_class_entry_ptr) {
		block = (SWFBlock)getAction(*zchar TSRMLS_CC);
	} else {
		block = (SWFBlock)getCharacter(*zchar TSRMLS_CC);
	}

	item = SWFMovieClip_add(sprite, block);

	if (item != NULL) {
		/* try and create a displayitem object */
		ret = zend_list_insert(item, le_swfdisplayitemp);
		object_init_ex(return_value, displayitem_class_entry_ptr);
		add_property_resource(return_value, "displayitem", ret);
		zend_list_addref(ret);
	}
}
/* }}} */

/* {{{ proto void swfsprite::remove(object SWFDisplayItem)
   Remove the named character from the sprite's display list */
PHP_METHOD(swfsprite, remove)
{
	zval **zchar;
	SWFDisplayItem item;
	SWFMovieClip movie = getSprite(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zchar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(zchar);
	item = getDisplayItem(*zchar TSRMLS_CC);
	SWFMovieClip_remove(movie, item);
}
/* }}} */

/* {{{ proto void swfsprite::nextFrame()
   Moves the sprite to the next frame */
PHP_METHOD(swfsprite, nextFrame)
{
	SWFMovieClip_nextFrame(getSprite(getThis() TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swfsprite::labelFrame(string label)
   Labels frame */
PHP_METHOD(swfsprite, labelFrame)
{
	zval **label;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &label) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(label);
	SWFMovieClip_labelFrame(getSprite(getThis() TSRMLS_CC), Z_STRVAL_PP(label));
}
/* }}} */

/* {{{ proto void swfsprite::setFrames(int frames)
   Sets the number of frames in this SWFSprite */
PHP_METHOD(swfsprite, setFrames)
{
	zval **frames;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &frames) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(frames);
	SWFMovieClip_setNumberOfFrames(sprite, Z_LVAL_PP(frames));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ SWFSoundInstance swfsprite_startsound */

PHP_METHOD(swfsprite, startSound)
{
	zval **zsound;
	int ret;
	SWFSound sound;
	SWFSoundInstance item;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zsound) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = (SWFSound)getSound(*zsound TSRMLS_CC);

	item = SWFMovieClip_startSound(sprite, sound);

	if(item != NULL) {
		/* try and create a displayitem object */
		ret = zend_list_insert(item, le_swfsoundinstancep);
		object_init_ex(return_value, soundinstance_class_entry_ptr);
		add_property_resource(return_value, "soundinstance", ret);
		zend_list_addref(ret);
	}
}

/* }}} */
/* {{{ void swfsprite_stopsound */

PHP_METHOD(swfsprite, stopSound)
{
	zval **zsound;
	SWFSound sound;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zsound) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_object_ex(zsound);
	sound = (SWFSound)getSound(*zsound TSRMLS_CC);

	SWFMovieClip_stopSound(sprite, sound);
}
/* }}} */

/* {{{ proto long swfmovieclip::setSoundStream(mixed file, rate[, skip])
   Sets sound stream of the SWF movieClip. The parameter can be stream or string. */
PHP_METHOD(swfsprite, setSoundStream)
{
	zval **zfile, **zskip, **zrate;
	float skip, rate;
	SWFSoundStream sound;
	SWFInput input;
	SWFMovieClip mc = getSprite(getThis() TSRMLS_CC);

	switch (ZEND_NUM_ARGS()) 
	{
	case 2:
		if(zend_get_parameters_ex(2, &zfile, &zrate) == FAILURE) 
			WRONG_PARAM_COUNT;
		skip = 0;
		break;
	case 3:
		if(zend_get_parameters_ex(3, &zfile, &zrate, &zskip) == FAILURE) 
			WRONG_PARAM_COUNT;
		convert_to_double_ex(zskip);
		skip = Z_DVAL_PP(zskip);
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(zrate);
	rate = Z_DVAL_PP(zrate);
	if (Z_TYPE_PP(zfile) != IS_RESOURCE) {
		convert_to_string_ex(zfile);
		input = newSWFInput_buffer(Z_STRVAL_PP(zfile), Z_STRLEN_PP(zfile));
		zend_list_addref(zend_list_insert(input, le_swfinputp));
	} else {
		input = getInput(zfile TSRMLS_CC);
	}

	sound = newSWFSoundStream_fromInput(input);
	SWFMovieClip_setSoundStreamAt(mc, sound, Z_DVAL_PP(zrate), skip);
	RETURN_LONG(SWFSoundStream_getDuration(sound) / rate);
}
/* }}} */


/* {{{ proto void swfsprite::setScalingGrid(int x, int y, int w, int h) */
PHP_METHOD(swfsprite, setScalingGrid)
{
	zval **zx, **zy, **zw, **zh;
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);
	if(ZEND_NUM_ARGS() != 4 || 
			zend_get_parameters_ex(4, &zx, &zy, &zw, &zh) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(zx);
	convert_to_long_ex(zy);
	convert_to_long_ex(zw);
	convert_to_long_ex(zh);

	SWFMovieClip_setScalingGrid(sprite, Z_LVAL_PP(zx), Z_LVAL_PP(zy), 
					Z_LVAL_PP(zw), Z_LVAL_PP(zh));
}
/* }}} */

/* {{{ proto void swfsprite::removeScalingGrid() */
PHP_METHOD(swfsprite, removeScalingGrid)
{
	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);
	if(ZEND_NUM_ARGS() != 0) 
		WRONG_PARAM_COUNT;

	SWFMovieClip_removeScalingGrid(sprite);
}
/* }}} */

/* {{{ proto void swfsprite::addInitAction(action) */
PHP_METHOD(swfsprite, addInitAction)
{
	zval **zaction;

	SWFMovieClip sprite = getSprite(getThis() TSRMLS_CC);
	SWFAction action;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zaction) == FAILURE) 
		WRONG_PARAM_COUNT;
	
	convert_to_object_ex(zaction);
	action = getAction(*zaction TSRMLS_CC);
	SWFMovieClip_addInitAction(sprite, action);
}
/* }}} */
#endif

// workaround to support SWFSprite and SWFMovieclip objects
static zend_function_entry swfmovieclip_functions[] = {
	PHP_ME(swfsprite, __construct,  	NULL, 0)
	PHP_ME(swfsprite, add,			NULL, 0)
	PHP_ME(swfsprite, remove,		NULL, 0)
	PHP_ME(swfsprite, nextFrame,		NULL, 0)
	PHP_ME(swfsprite, labelFrame,		NULL, 0)
	PHP_ME(swfsprite, setFrames,		NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfsprite, startSound,		NULL, 0)
	PHP_ME(swfsprite, stopSound,		NULL, 0)
	PHP_ME(swfsprite, setScalingGrid, 	NULL, 0)
	PHP_ME(swfsprite, removeScalingGrid, 	NULL, 0)
	PHP_ME(swfsprite, setSoundStream, 	NULL, 0)
	PHP_ME(swfsprite, addInitAction,	NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

static zend_function_entry swfsprite_functions[] = {
	PHP_ME(swfsprite, __construct,  	NULL, 0)
	PHP_ME(swfsprite, add,			NULL, 0)
	PHP_ME(swfsprite, remove,		NULL, 0)
	PHP_ME(swfsprite, nextFrame,		NULL, 0)
	PHP_ME(swfsprite, labelFrame,		NULL, 0)
	PHP_ME(swfsprite, setFrames,		NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swfsprite, startSound,		NULL, 0)
	PHP_ME(swfsprite, stopSound,		NULL, 0)
	PHP_ME(swfsprite, setScalingGrid, 	NULL, 0)
	PHP_ME(swfsprite, removeScalingGrid, 	NULL, 0)
	PHP_ME(swfsprite, setSoundStream, 	NULL, 0)
	PHP_ME(swfsprite, addInitAction,	NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFText
*/
/* {{{ proto void swftext::__construct()
   Creates new SWFText object */
PHP_METHOD(swftext, __construct)
{
	SWFText text = newSWFText2();
	int ret = zend_list_insert(text, le_swftextp);

	object_init_ex(getThis(), text_class_entry_ptr);
	add_property_resource(getThis(), "text", ret);
	zend_list_addref(ret);
}

static void destroy_SWFText_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFText((SWFText)resource->ptr);
}
/* }}} */

/* {{{ internal function SWFText getText(zval *id)
   Returns the SWFText contained in zval *id */
static SWFText getText(zval *id TSRMLS_DC)
{
	void *text = SWFgetProperty(id, "text", 4, le_swftextp TSRMLS_CC);

	if (!text) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFText");
	}
	return (SWFText)text;
}
/* }}} */

/* {{{ proto void swftext::setFont(object font)
   Sets this SWFText object's current font to given font */
PHP_METHOD(swftext, setFont)
{
	zval **zfont;
	SWFText text = getText(getThis() TSRMLS_CC);
	SWFFont font;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zfont) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(zfont);
	font = getFont(*zfont TSRMLS_CC);
	SWFText_setFont(text, font);
}
/* }}} */

/* {{{ proto void swftext::setHeight(float height)
   Sets this SWFText object's current height to given height */
PHP_METHOD(swftext, setHeight)
{
	zval **height;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &height) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(height);
	SWFText_setHeight(text, Z_DVAL_PP(height));
}
/* }}} */

/* {{{ proto void swftext::setSpacing(float spacing)
   Sets this SWFText object's current letterspacing to given spacing */
PHP_METHOD(swftext, setSpacing)
{
	zval **spacing;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &spacing) == FAILURE) {
 		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(spacing);
	SWFText_setSpacing(text, Z_DVAL_PP(spacing));
}
/* }}} */

/* {{{ proto void swftext::setColor(int r, int g, int b [, int a])
   Sets this SWFText object's current color to the given color */
PHP_METHOD(swftext, setColor)
{
	zval **r, **g, **b, **a;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(a);
	} else {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);

	if (ZEND_NUM_ARGS() == 4) {
		SWFText_setColor(text, BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), BYTE_Z_LVAL_PP(a));
	} else {
		SWFText_setColor(text, BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), 0xff);
	}
}
/* }}} */

/* {{{ proto void swftext::moveTo(float x, float y)
   Moves this SWFText object's current pen position to (x, y) in text coordinates */
PHP_METHOD(swftext, moveTo)
{
	zval **x, **y;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	SWFText_setXY(text, Z_DVAL_PP(x), Z_DVAL_PP(y));
}
/* }}} */

/* {{{ proto void swftext::addString(string text)
   Writes the given text into this SWFText object at the current pen position, using the current font, height, spacing, and color */
PHP_METHOD(swftext, addString)
{
	zval **s;
	SWFText text = getText(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &s) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(s);
	SWFText_addString(text, Z_STRVAL_PP(s), NULL);
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto void swftext::addUTF8String(string text)
   Writes the given text into this SWFText object at the current pen position,
   using the current font, height, spacing, and color */

PHP_METHOD(swftext, addUTF8String)
{
	zval **s;
	SWFText text = getText(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &s) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(s);

	SWFText_addUTF8String(text, Z_STRVAL_PP(s), NULL);
}

/* }}} */
/* {{{ proto void swftext::addWideString(string text)
   Writes the given text into this SWFText object at the current pen position,
   using the current font, height, spacing, and color */
/*
PHP_METHOD(swftext, addWideString)
{
	zval **s;
	SWFText text = getText(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &s) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(s);

	SWFText_addWideString(text, Z_STRVAL_PP(s), NULL);
}
*/
/* }}} */
#endif

/* {{{ proto float swftext::getWidth(string str)
   Calculates the width of the given string in this text objects current font and size */
PHP_METHOD(swftext, getWidth)
{
	zval **zstring;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(zstring);
	RETURN_DOUBLE(SWFText_getStringWidth(getText(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring)));
}
/* }}} */

#ifdef HAVE_NEW_MING
/* {{{ proto double swftext::getUTF8Width(string)
   calculates the width of the given string in this text objects current font and size */

PHP_METHOD(swftext, getUTF8Width)
{
	zval **zstring;
	int width;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	width = SWFText_getUTF8StringWidth(getText(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));

	RETURN_DOUBLE(width);
}

/* }}} */
/* {{{ proto double swftext::getWideWidth(string)
   calculates the width of the given string in this text objects current font and size */
/*
PHP_METHOD(swftext, getWideWidth)
{
	zval **zstring;
	int width;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	width = SWFText_getWideStringWidth(getText(getThis() TSRMLS_CC), Z_STRVAL_PP(zstring));

	RETURN_DOUBLE(width);
}
*/
/* }}} */
#endif

/* {{{ proto float swftext::getAscent()
   Returns the ascent of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getAscent)
{
	if(ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_DOUBLE(SWFText_getAscent(getText(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swftext::getDescent()
   Returns the descent of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getDescent)
{
	if(ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
  RETURN_DOUBLE(SWFText_getDescent(getText(getThis() TSRMLS_CC)));
}
/* }}} */

/* {{{ proto float swftext::getLeading()
   Returns the leading of the current font at its current size, or 0 if not available */
PHP_METHOD(swftext, getLeading)
{
	if(ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
  RETURN_DOUBLE(SWFText_getLeading(getText(getThis() TSRMLS_CC)));
}
/* }}} */

static zend_function_entry swftext_functions[] = {
	PHP_ME(swftext, __construct,       NULL, 0)
	PHP_ME(swftext, setFont,           NULL, 0)
	PHP_ME(swftext, setHeight,         NULL, 0)
	PHP_ME(swftext, setSpacing,        NULL, 0)
	PHP_ME(swftext, setColor,          NULL, 0)
	PHP_ME(swftext, moveTo,            NULL, 0)
	PHP_ME(swftext, addString,         NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swftext, addUTF8String,     NULL, 0)
/*	PHP_ME(swftext, addwidestring,     NULL, 0)*/
#endif
	PHP_ME(swftext, getWidth,          NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swftext, getUTF8Width,      NULL, 0)
/*	PHP_ME(swftext, getwidewidth,      NULL, 0)*/
#endif
	PHP_ME(swftext, getAscent,         NULL, 0)
	PHP_ME(swftext, getDescent,        NULL, 0)
	PHP_ME(swftext, getLeading,        NULL, 0)
	{ NULL, NULL, NULL }
};

/* }}} */

/* {{{ SWFTextField
*/
/* {{{ proto void swftextfield::__construct([int flags])
   Creates a new SWFTextField object */
PHP_METHOD(swftextfield, __construct)
{
	zval **flags;
	SWFTextField field = newSWFTextField();
	int ret = zend_list_insert(field, le_swftextfieldp);

	object_init_ex(getThis(), textfield_class_entry_ptr);
	add_property_resource(getThis(), "textfield", ret);
	zend_list_addref(ret);

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &flags) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(flags);
		SWFTextField_setFlags(field, Z_LVAL_PP(flags));
	}
}
/* }}} */

static void destroy_SWFTextField_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
	destroySWFTextField((SWFTextField)resource->ptr);
}

/* {{{ internal function getTextField
   Returns the SWFTextField object contained in zval *id */
static SWFTextField getTextField(zval *id TSRMLS_DC)
{
	void *field = SWFgetProperty(id, "textfield", 9, le_swftextfieldp TSRMLS_CC);

	if (!field) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Called object is not an SWFTextField");
	}
	return (SWFTextField)field;
}
/* }}} */

/* {{{ proto void swftextfield::setFont(object font)
   Sets the font for this textfield */
static
SWFBlock getFontOrFontChar(zval *id TSRMLS_DC)
{
	if(Z_OBJCE_P(id) == font_class_entry_ptr) {
		return (SWFBlock)getFont(id TSRMLS_CC);
	} else if(Z_OBJCE_P(id) == fontchar_class_entry_ptr) {
		return (SWFBlock)getFontCharacter(id TSRMLS_CC);
	} else {
		php_error(E_ERROR, "called object is not an SWFFont or SWFFontCharacter");
	}
	return NULL;
}

PHP_METHOD(swftextfield, setFont)
{
	zval **font;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &font) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_object_ex(font);
	SWFTextField_setFont(field, getFontOrFontChar(*font TSRMLS_CC));
}
/* }}} */

/* {{{ proto void swftextfield::setBounds(float width, float height)
   Sets the width and height of this textfield */
PHP_METHOD(swftextfield, setBounds)
{
	zval **width, **height;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &width, &height) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(width);
	convert_to_double_ex(height);
	SWFTextField_setBounds(field, Z_DVAL_PP(width), Z_DVAL_PP(height));
}
/* }}} */

/* {{{ proto void swftextfield::align(int alignment)
   Sets the alignment of this textfield */
PHP_METHOD(swftextfield, align)
{
	zval **align;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &align) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(align);
	SWFTextField_setAlignment(field, Z_LVAL_PP(align));
}
/* }}} */

/* {{{ proto void swftextfield::setHeight(float height)
   Sets the font height of this textfield */
PHP_METHOD(swftextfield, setHeight)
{
	zval **height;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &height) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(height);
	SWFTextField_setHeight(field, Z_DVAL_PP(height));
}
/* }}} */

/* {{{ proto void swftextfield::setLeftMargin(float margin)
   Sets the left margin of this textfield */
PHP_METHOD(swftextfield, setLeftMargin)
{
	zval **margin;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &margin) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(margin);
	SWFTextField_setLeftMargin(field, Z_DVAL_PP(margin));
}
/* }}} */

/* {{{ proto void swftextfield::setRightMargin(float margin)
   Sets the right margin of this textfield */
PHP_METHOD(swftextfield, setRightMargin)
{
	zval **margin;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &margin) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(margin);
	SWFTextField_setRightMargin(field, Z_DVAL_PP(margin));
}
/* }}} */

/* {{{ proto void swftextfield::setMargins(float left, float right)
   Sets both margins of this textfield */
PHP_METHOD(swftextfield, setMargins)
{
	zval **left, **right;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(left);
	convert_to_double_ex(right);
	SWFTextField_setLeftMargin(field, Z_DVAL_PP(left));
	SWFTextField_setRightMargin(field, Z_DVAL_PP(right));
}
/* }}} */

/* {{{ proto void swftextfield::setIndentation(float indentation)
   Sets the indentation of the first line of this textfield */
PHP_METHOD(swftextfield, setIndentation)
{
	zval **indent;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &indent) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(indent);
	SWFTextField_setIndentation(field, Z_DVAL_PP(indent));
}
/* }}} */

/* {{{ proto void swftextfield::setLineSpacing(float space)
   Sets the line spacing of this textfield */
PHP_METHOD(swftextfield, setLineSpacing)
{
	zval **spacing;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &spacing) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(spacing);
	SWFTextField_setLineSpacing(field, Z_DVAL_PP(spacing));
}
/* }}} */

/* {{{ proto void swftextfield::setColor(int r, int g, int b [, int a])
   Sets the color of this textfield */
PHP_METHOD(swftextfield, setColor)
{
	zval **r, **g, **b, **a;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);
	byte alpha = 0xff;

	if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &r, &g, &b) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(a);
		alpha = BYTE_Z_LVAL_PP(a);
	} else {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(r);
	convert_to_long_ex(g);
	convert_to_long_ex(b);
	SWFTextField_setColor(field, BYTE_Z_LVAL_PP(r), BYTE_Z_LVAL_PP(g), BYTE_Z_LVAL_PP(b), (byte)alpha);
}
/* }}} */

/* {{{ proto void swftextfield::setName(string var_name)
   Sets the variable name of this textfield */
PHP_METHOD(swftextfield, setName)
{
	zval **name;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	SWFTextField_setVariableName(field, Z_STRVAL_PP(name));
}
/* }}} */

/* {{{ proto void swftextfield::addString(string str)
   Adds the given string to this textfield */
PHP_METHOD(swftextfield, addString)
{
	zval **string;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(string);
	SWFTextField_addString(field, Z_STRVAL_PP(string));
}
/* }}} */

/* {{{ proto void swftextfield::addUTF8String(string str)
   Adds the given UTF8 string to this textfield */
PHP_METHOD(swftextfield, addUTF8String)
{
	zval **string;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(string);
	SWFTextField_addUTF8String(field, Z_STRVAL_PP(string));
}
/* }}} */


#ifdef HAVE_NEW_MING
/* {{{ proto void swftextfield::setPadding(float padding)
   Sets the padding of this textfield */
PHP_METHOD(swftextfield, setPadding)
{
	zval **padding;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &padding) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_double_ex(padding);

	SWFTextField_setPadding(field, Z_DVAL_PP(padding));
}
/* }}} */

/* {{{ proto void swftextfield::addChars(string)
   adds characters to a font that will be available within a textfield */
PHP_METHOD(swftextfield, addChars)
{
	zval **zstring;
	SWFTextField field = getTextField(getThis() TSRMLS_CC);

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstring) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(zstring);

	SWFTextField_addChars(field, Z_STRVAL_PP(zstring));

}
/* }}} */
#endif

static zend_function_entry swftextfield_functions[] = {
	PHP_ME(swftextfield, __construct,     NULL, 0)
	PHP_ME(swftextfield, setFont,         NULL, 0)
	PHP_ME(swftextfield, setBounds,       NULL, 0)
	PHP_ME(swftextfield, align,           NULL, 0)
	PHP_ME(swftextfield, setHeight,       NULL, 0)
	PHP_ME(swftextfield, setLeftMargin,   NULL, 0)
	PHP_ME(swftextfield, setRightMargin,  NULL, 0)
	PHP_ME(swftextfield, setMargins,      NULL, 0)
	PHP_ME(swftextfield, setIndentation,  NULL, 0)
	PHP_ME(swftextfield, setLineSpacing,  NULL, 0)
	PHP_ME(swftextfield, setColor,        NULL, 0)
	PHP_ME(swftextfield, setName,         NULL, 0)
	PHP_ME(swftextfield, addString,       NULL, 0)
	PHP_ME(swftextfield, addUTF8String,   NULL, 0)
#ifdef HAVE_NEW_MING
	PHP_ME(swftextfield, setPadding,      NULL, 0)
	PHP_ME(swftextfield, addChars,        NULL, 0)
#endif
	{ NULL, NULL, NULL }
};

/* }}} */

zend_module_entry ming_module_entry = {
	STANDARD_MODULE_HEADER,
	"ming",
	ming_functions,
	PHP_MINIT(ming), /* module init function */
	NULL,            /* module shutdown function */
	PHP_RINIT(ming), /* request init function */
	NULL,            /* request shutdown function */
	PHP_MINFO(ming), /* module info function */
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MING
ZEND_GET_MODULE(ming)
#endif

/* {{{ todo PHP_MINFO_FUNCTION(ming)
*/
PHP_MINFO_FUNCTION(ming)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Ming SWF output library", "enabled");
	php_info_print_table_row(2, "Version", MING_VERSION_TEXT);
	php_info_print_table_end();
}
/* }}} */

/* {{{ todo PHP_MINIT_FUNCTION(ming)
*/

#if PHP_API_VERSION == 20020918
static php4_fix_funcnames(char *class_name, zend_function_entry *funcs)
{
	zend_function_entry *pf = funcs;
	char *pname;

	while(funcs->fname) {
		if( strcmp(funcs->fname,"__construct") == 0 ) {
			pname=strdup(class_name);
		} else {
			pname=strdup(funcs->fname);
		}
		funcs->fname=pname;
		while(*pname) { *pname=tolower(*pname);pname++;}
		funcs++;
	}
}
#endif

/* custom error handler propagates ming errors up to php */
static void php_ming_error(const char *msg, ...)
{
	va_list args;
	char *buffer;
	
	TSRMLS_FETCH();

	va_start(args, msg);
	vspprintf(&buffer, 0, msg, args);
	va_end(args);

	php_error_docref(NULL TSRMLS_CC, E_ERROR, "%s", buffer);
	efree(buffer);
}

PHP_RINIT_FUNCTION(ming)
{
	/* XXX - this didn't work so well last I tried.. */

	if (Ming_init() != 0) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error initializing Ming module");
		return FAILURE;
	}
	return SUCCESS;
}

PHP_MINIT_FUNCTION(ming)
{
	zend_class_entry shape_class_entry;
	zend_class_entry fill_class_entry;
	zend_class_entry gradient_class_entry;
	zend_class_entry bitmap_class_entry;
	zend_class_entry text_class_entry;
	zend_class_entry textfield_class_entry;
	zend_class_entry font_class_entry;
	zend_class_entry displayitem_class_entry;
	zend_class_entry movie_class_entry;
	zend_class_entry button_class_entry;
	zend_class_entry action_class_entry;
	zend_class_entry morph_class_entry;
	zend_class_entry movieclip_class_entry;
	zend_class_entry sprite_class_entry;
	zend_class_entry sound_class_entry;
#ifdef HAVE_NEW_MING
	zend_class_entry fontchar_class_entry;
	zend_class_entry soundinstance_class_entry;
	zend_class_entry videostream_class_entry;
	zend_class_entry buttonrecord_class_entry;
	zend_class_entry binarydata_class_entry;
	zend_class_entry initaction_class_entry;
	zend_class_entry prebuiltclip_class_entry;
	zend_class_entry soundstream_class_entry;
	zend_class_entry filter_class_entry;
	zend_class_entry filtermatrix_class_entry;
	zend_class_entry blur_class_entry;
	zend_class_entry shadow_class_entry;
#endif
	zend_class_entry character_class_entry;
	Ming_setErrorFunction((void *) php_ming_error);

#define CONSTANT(s,c) REGISTER_LONG_CONSTANT((s), (c), CONST_CS | CONST_PERSISTENT)

#ifdef HAVE_NEW_MING
	CONSTANT("MING_NEW",                    1);
#else
	CONSTANT("MING_NEW",                    0);
#endif
#ifdef HAVE_MING_ZLIB
	CONSTANT("MING_ZLIB",                   1);
#else
	CONSTANT("MING_ZLIB",                   0);
#endif
	/* flags for SWFButton_addShape */
	CONSTANT("SWFBUTTON_HIT",               SWFBUTTONRECORD_HITSTATE);
	CONSTANT("SWFBUTTON_DOWN",              SWFBUTTONRECORD_DOWNSTATE);
	CONSTANT("SWFBUTTON_OVER",              SWFBUTTONRECORD_OVERSTATE);
	CONSTANT("SWFBUTTON_UP",                SWFBUTTONRECORD_UPSTATE);

	/* flags for SWFButton_addAction */
	CONSTANT("SWFBUTTON_MOUSEUPOUTSIDE",    SWFBUTTON_MOUSEUPOUTSIDE);
	CONSTANT("SWFBUTTON_DRAGOVER",          SWFBUTTON_DRAGOVER);
	CONSTANT("SWFBUTTON_DRAGOUT",           SWFBUTTON_DRAGOUT);
	CONSTANT("SWFBUTTON_MOUSEUP",           SWFBUTTON_MOUSEUP);
	CONSTANT("SWFBUTTON_MOUSEDOWN",         SWFBUTTON_MOUSEDOWN);
	CONSTANT("SWFBUTTON_MOUSEOUT",          SWFBUTTON_MOUSEOUT);
	CONSTANT("SWFBUTTON_MOUSEOVER",         SWFBUTTON_MOUSEOVER);

	/* flags for SWFFill */
	CONSTANT("SWFFILL_RADIAL_GRADIENT",     SWFFILL_RADIAL_GRADIENT);
	CONSTANT("SWFFILL_LINEAR_GRADIENT",     SWFFILL_LINEAR_GRADIENT);
	CONSTANT("SWFFILL_TILED_BITMAP",        SWFFILL_TILED_BITMAP);
	CONSTANT("SWFFILL_CLIPPED_BITMAP",      SWFFILL_CLIPPED_BITMAP);

	/* flags for SWFTextField_init */
	CONSTANT("SWFTEXTFIELD_HASLENGTH",      SWFTEXTFIELD_HASLENGTH);
	CONSTANT("SWFTEXTFIELD_NOEDIT",         SWFTEXTFIELD_NOEDIT);
	CONSTANT("SWFTEXTFIELD_PASSWORD",       SWFTEXTFIELD_PASSWORD);
	CONSTANT("SWFTEXTFIELD_MULTILINE",      SWFTEXTFIELD_MULTILINE);
	CONSTANT("SWFTEXTFIELD_WORDWRAP",       SWFTEXTFIELD_WORDWRAP);
	CONSTANT("SWFTEXTFIELD_DRAWBOX",        SWFTEXTFIELD_DRAWBOX);
	CONSTANT("SWFTEXTFIELD_NOSELECT",       SWFTEXTFIELD_NOSELECT);
	CONSTANT("SWFTEXTFIELD_HTML",           SWFTEXTFIELD_HTML);
	CONSTANT("SWFTEXTFIELD_USEFONT",        SWFTEXTFIELD_USEFONT);
	CONSTANT("SWFTEXTFIELD_AUTOSIZE",       SWFTEXTFIELD_AUTOSIZE);

	/* flags for SWFTextField_align */
	CONSTANT("SWFTEXTFIELD_ALIGN_LEFT",     SWFTEXTFIELD_ALIGN_LEFT);
	CONSTANT("SWFTEXTFIELD_ALIGN_RIGHT",    SWFTEXTFIELD_ALIGN_RIGHT);
	CONSTANT("SWFTEXTFIELD_ALIGN_CENTER",   SWFTEXTFIELD_ALIGN_CENTER);
	CONSTANT("SWFTEXTFIELD_ALIGN_JUSTIFY",  SWFTEXTFIELD_ALIGN_JUSTIFY);

	/* flags for SWFDisplayItem_addAction */
	CONSTANT("SWFACTION_ONLOAD",            SWFACTION_ONLOAD);
	CONSTANT("SWFACTION_ENTERFRAME",        SWFACTION_ENTERFRAME);
	CONSTANT("SWFACTION_UNLOAD",            SWFACTION_UNLOAD);
	CONSTANT("SWFACTION_MOUSEMOVE",         SWFACTION_MOUSEMOVE);
	CONSTANT("SWFACTION_MOUSEDOWN",         SWFACTION_MOUSEDOWN);
	CONSTANT("SWFACTION_MOUSEUP",           SWFACTION_MOUSEUP);
	CONSTANT("SWFACTION_KEYDOWN",           SWFACTION_KEYDOWN);
	CONSTANT("SWFACTION_KEYUP",             SWFACTION_KEYUP);
	CONSTANT("SWFACTION_DATA",              SWFACTION_DATA);
	CONSTANT("SWFACTION_INIT",              SWFACTION_INIT);
	CONSTANT("SWFACTION_PRESS",             SWFACTION_PRESS);
	CONSTANT("SWFACTION_RELEASE",           SWFACTION_RELEASE);
	CONSTANT("SWFACTION_RELEASEOUTSIDE",    SWFACTION_RELEASEOUTSIDE);
	CONSTANT("SWFACTION_ROLLOVER",          SWFACTION_ROLLOVER);
	CONSTANT("SWFACTION_DRAGOVER",          SWFACTION_DRAGOVER);
	CONSTANT("SWFACTION_DRAGOUT",           SWFACTION_DRAGOUT);
	CONSTANT("SWFACTION_KEYPRESS",          SWFACTION_KEYPRESS);
	CONSTANT("SWFACTION_CONSTRUCT",         SWFACTION_CONSTRUCT);

  /* flags for SWFSound */
	CONSTANT("SWF_SOUND_NOT_COMPRESSED",    SWF_SOUND_NOT_COMPRESSED);
	CONSTANT("SWF_SOUND_ADPCM_COMPRESSED",  SWF_SOUND_ADPCM_COMPRESSED);
	CONSTANT("SWF_SOUND_MP3_COMPRESSED",    SWF_SOUND_MP3_COMPRESSED);
	CONSTANT("SWF_SOUND_NOT_COMPRESSED_LE", SWF_SOUND_NOT_COMPRESSED_LE);
	CONSTANT("SWF_SOUND_NELLY_COMPRESSED",  SWF_SOUND_NELLY_COMPRESSED);
	CONSTANT("SWF_SOUND_5KHZ",              SWF_SOUND_5KHZ);
	CONSTANT("SWF_SOUND_11KHZ",             SWF_SOUND_11KHZ);
	CONSTANT("SWF_SOUND_22KHZ",             SWF_SOUND_22KHZ);
	CONSTANT("SWF_SOUND_44KHZ",             SWF_SOUND_44KHZ);
	CONSTANT("SWF_SOUND_8BITS",             SWF_SOUND_8BITS);
	CONSTANT("SWF_SOUND_16BITS",            SWF_SOUND_16BITS);
	CONSTANT("SWF_SOUND_MONO",              SWF_SOUND_MONO);
	CONSTANT("SWF_SOUND_STEREO",            SWF_SOUND_STEREO);

#ifdef HAVE_NEW_MING
	/* Blend Modes */
	CONSTANT("SWFBLEND_MODE_NORMAL", 		SWFBLEND_MODE_NORMAL);
	CONSTANT("SWFBLEND_MODE_LAYER", 		SWFBLEND_MODE_LAYER);
	CONSTANT("SWFBLEND_MODE_MULT", 			SWFBLEND_MODE_MULT);
	CONSTANT("SWFBLEND_MODE_SCREEN",		SWFBLEND_MODE_SCREEN);
	CONSTANT("SWFBLEND_MODE_DARKEN",		SWFBLEND_MODE_DARKEN);
	CONSTANT("SWFBLEND_MODE_LIGHTEN",		SWFBLEND_MODE_LIGHTEN);
	CONSTANT("SWFBLEND_MODE_ADD", 			SWFBLEND_MODE_ADD);
	CONSTANT("SWFBLEND_MODE_SUB",			SWFBLEND_MODE_SUB);
	CONSTANT("SWFBLEND_MODE_DIFF", 			SWFBLEND_MODE_DIFF);
	CONSTANT("SWFBLEND_MODE_INV", 			SWFBLEND_MODE_INV);
	CONSTANT("SWFBLEND_MODE_ALPHA", 		SWFBLEND_MODE_ALPHA);
	CONSTANT("SWFBLEND_MODE_ERASE", 		SWFBLEND_MODE_ERASE);
	CONSTANT("SWFBLEND_MODE_OVERLAY", 		SWFBLEND_MODE_OVERLAY);
	CONSTANT("SWFBLEND_MODE_HARDLIGHT", 		SWFBLEND_MODE_HARDLIGHT);

	/* filter types */
	CONSTANT("SWFFILTER_TYPE_DROPSHADOW",		SWFFILTER_TYPE_DROPSHADOW);
	CONSTANT("SWFFILTER_TYPE_BLUR",			SWFFILTER_TYPE_BLUR);
	CONSTANT("SWFFILTER_TYPE_GLOW",			SWFFILTER_TYPE_GLOW);
	CONSTANT("SWFFILTER_TYPE_BEVEL",		SWFFILTER_TYPE_BEVEL);
	CONSTANT("SWFFILTER_TYPE_GRADIENTGLOW",		SWFFILTER_TYPE_GRADIENTGLOW);
	CONSTANT("SWFFILTER_TYPE_CONVOLUTION", 		SWFFILTER_TYPE_CONVOLUTION);
	CONSTANT("SWFFILTER_TYPE_COLORMATRIX",		SWFFILTER_TYPE_COLORMATRIX);
	CONSTANT("SWFFILTER_TYPE_GRADIENTBEVEL", 	SWFFILTER_TYPE_GRADIENTBEVEL);	

	/* filter flags */
	CONSTANT("SWFFILTER_FLAG_CLAMP",		FILTER_FLAG_CLAMP);
	CONSTANT("SWFFILTER_FLAG_PRESERVE_ALPHA",	FILTER_FLAG_PRESERVE_ALPHA);

	/* filter modes */
	CONSTANT("SWFFILTER_MODE_INNER",		FILTER_MODE_INNER);
	CONSTANT("SWFFILTER_MODE_KO",			FILTER_MODE_KO);
	CONSTANT("SWFFILTER_MODE_COMPOSITE",		FILTER_MODE_COMPOSITE);
	CONSTANT("SWFFILTER_MODE_ONTOP",		FILTER_MODE_ONTOP);

	/* new gradient features */
	CONSTANT("SWF_GRADIENT_PAD",			SWF_GRADIENT_PAD);
	CONSTANT("SWF_GRADIENT_REFLECT",		SWF_GRADIENT_REFLECT);
	CONSTANT("SWF_GRADIENT_REPEAT", 		SWF_GRADIENT_REPEAT);
	CONSTANT("SWF_GRADIENT_NORMAL",			SWF_GRADIENT_NORMAL);
	CONSTANT("SWF_GRADIENT_LINEAR",			SWF_GRADIENT_LINEAR);

	/* shape options */
	CONSTANT("SWF_SHAPE3",				SWF_SHAPE3);
	CONSTANT("SWF_SHAPE4", 				SWF_SHAPE4);
	CONSTANT("SWF_SHAPE_USESCALINGSTROKES",		SWF_SHAPE_USESCALINGSTROKES);
	CONSTANT("SWF_SHAPE_USENONSCALINGSTROKES", 	SWF_SHAPE_USENONSCALINGSTROKES);

	/* linestyle 2 flags */
	CONSTANT("SWF_LINESTYLE_CAP_ROUND", 		SWF_LINESTYLE_CAP_ROUND);
	CONSTANT("SWF_LINESTYLE_CAP_NONE", 		SWF_LINESTYLE_CAP_NONE);
	CONSTANT("SWF_LINESTYLE_CAP_SQUARE",		SWF_LINESTYLE_CAP_SQUARE);
	CONSTANT("SWF_LINESTYLE_JOIN_ROUND",		SWF_LINESTYLE_JOIN_ROUND);
	CONSTANT("SWF_LINESTYLE_JOIN_BEVEL", 		SWF_LINESTYLE_JOIN_BEVEL);
	CONSTANT("SWF_LINESTYLE_JOIN_MITER",		SWF_LINESTYLE_JOIN_MITER);
	CONSTANT("SWF_LINESTYLE_FLAG_NOHSCALE",		SWF_LINESTYLE_FLAG_NOHSCALE);
	CONSTANT("SWF_LINESTYLE_FLAG_NOVSCALE", 	SWF_LINESTYLE_FLAG_NOVSCALE);
	CONSTANT("SWF_LINESTYLE_FLAG_HINTING",		SWF_LINESTYLE_FLAG_HINTING);
	CONSTANT("SWF_LINESTYLE_FLAG_NOCLOSE",		SWF_LINESTYLE_FLAG_NOCLOSE);
	CONSTANT("SWF_LINESTYLE_FLAG_ENDCAP_ROUND", 	SWF_LINESTYLE_FLAG_ENDCAP_ROUND);
	CONSTANT("SWF_LINESTYLE_FLAG_ENDCAP_NONE", 	SWF_LINESTYLE_FLAG_ENDCAP_NONE);
	CONSTANT("SWF_LINESTYLE_FLAG_ENDCAP_SQUARE", 	SWF_LINESTYLE_FLAG_ENDCAP_SQUARE);
#endif

	le_swfshapep = zend_register_list_destructors_ex(destroy_SWFShape_resource, NULL, "SWFShape", module_number);
	le_swffillp = zend_register_list_destructors_ex(destroy_SWFFill_resource, NULL, "SWFFill", module_number);
	le_swfgradientp = zend_register_list_destructors_ex(destroy_SWFGradient_resource, NULL, "SWFGradient", module_number);
	le_swfbitmapp = zend_register_list_destructors_ex(destroy_SWFBitmap_resource, NULL, "SWFBitmap", module_number);
	le_swftextp = zend_register_list_destructors_ex(destroy_SWFText_resource, NULL, "SWFText", module_number);
	le_swftextfieldp = zend_register_list_destructors_ex(destroy_SWFTextField_resource, NULL, "SWFTextField", module_number);
	le_swffontp = zend_register_list_destructors_ex(destroy_SWFFont_resource, NULL, "SWFFont", module_number);
	le_swfdisplayitemp = zend_register_list_destructors_ex(NULL, NULL, "SWFDisplayItem", module_number);
	le_swfmoviep = zend_register_list_destructors_ex(destroy_SWFMovie_resource, NULL, "SWFMovie", module_number);
	le_swfbuttonp = zend_register_list_destructors_ex(destroy_SWFButton_resource, NULL, "SWFButton", module_number);
	le_swfactionp = zend_register_list_destructors_ex(NULL, NULL, "SWFAction", module_number);
	le_swfmorphp = zend_register_list_destructors_ex(destroy_SWFMorph_resource, NULL, "SWFMorph", module_number);
	le_swfmovieclipp = zend_register_list_destructors_ex(destroy_SWFSprite_resource, NULL, "SWFMovieClip", module_number);
	le_swfspritep = zend_register_list_destructors_ex(destroy_SWFSprite_resource, NULL, "SWFSprite", module_number);
	le_swfinputp = zend_register_list_destructors_ex(destroy_SWFInput_resource, NULL, "SWFInput", module_number);

	le_swfsoundp = zend_register_list_destructors_ex(destroy_SWFSound_resource, NULL, "SWFSound", module_number);
#ifdef HAVE_NEW_MING
	le_swffontcharp = zend_register_list_destructors_ex(NULL, NULL, "SWFFontCharacter", module_number);
	le_swfbuttonrecordp = zend_register_list_destructors_ex(NULL, NULL, "SWFButtonRecord", module_number);
	le_swfsoundinstancep = zend_register_list_destructors_ex(NULL, NULL, "SWFSoundInstance", module_number);
	le_swfvideostreamp = zend_register_list_destructors_ex(destroy_SWFVideoStream_resource, NULL, "SWFVideoStream", module_number);
	le_swfbinarydatap = zend_register_list_destructors_ex(destroy_SWFBinaryData_resource, NULL, "SWFBinaryData", module_number);
	le_swfinitactionp = zend_register_list_destructors_ex(NULL, NULL, "SWFInitAction", module_number);
	le_swfprebuiltclipp = zend_register_list_destructors_ex(destroy_SWFPrebuiltClip_resource, NULL, "SWFPrebuiltClip", module_number);
	le_swfsoundstreamp = zend_register_list_destructors_ex(destroy_SWFSoundStream_resource, NULL, "SWFSoundStream", module_number);
	le_swffilterp = zend_register_list_destructors_ex(destroy_SWFFilter_resource, NULL, "SWFFilter", module_number);
	le_swfblurp = zend_register_list_destructors_ex(destroy_SWFBlur_resource, NULL, "SWFBlur", module_number);
	le_swfshadowp = zend_register_list_destructors_ex(destroy_SWFShadow_resource, NULL, "SWFShadow", module_number);
	le_swffiltermatrixp = zend_register_list_destructors_ex(destroy_SWFFilterMatrix_resource, NULL, "SWFFilterMatrix", module_number);
	le_swfcharacterp = zend_register_list_destructors_ex(NULL, NULL, "SWFCharacter", module_number);
#endif

	INIT_CLASS_ENTRY(shape_class_entry, "SWFShape", swfshape_functions);
	INIT_CLASS_ENTRY(fill_class_entry, "SWFFill", swffill_functions);
	INIT_CLASS_ENTRY(gradient_class_entry, "SWFGradient", swfgradient_functions);
	INIT_CLASS_ENTRY(bitmap_class_entry, "SWFBitmap", swfbitmap_functions);
	INIT_CLASS_ENTRY(text_class_entry, "SWFText", swftext_functions);
	INIT_CLASS_ENTRY(textfield_class_entry, "SWFTextField", swftextfield_functions);
	INIT_CLASS_ENTRY(font_class_entry, "SWFFont", swffont_functions);
	INIT_CLASS_ENTRY(displayitem_class_entry, "SWFDisplayItem", swfdisplayitem_functions);
	INIT_CLASS_ENTRY(movie_class_entry, "SWFMovie", swfmovie_functions);
	INIT_CLASS_ENTRY(button_class_entry, "SWFButton", swfbutton_functions);
	INIT_CLASS_ENTRY(action_class_entry, "SWFAction", swfaction_functions);
	INIT_CLASS_ENTRY(morph_class_entry, "SWFMorph", swfmorph_functions);
	INIT_CLASS_ENTRY(movieclip_class_entry, "SWFMovieClip", swfmovieclip_functions);
	INIT_CLASS_ENTRY(sprite_class_entry, "SWFSprite", swfsprite_functions);
	INIT_CLASS_ENTRY(sound_class_entry, "SWFSound", swfsound_functions);
#ifdef HAVE_NEW_MING
	INIT_CLASS_ENTRY(fontchar_class_entry, "SWFFontChar", swffontchar_functions);
	INIT_CLASS_ENTRY(buttonrecord_class_entry, "SWFButtonRecord", swfbuttonrecord_functions);
	INIT_CLASS_ENTRY(soundinstance_class_entry, "SWFSoundInstance", swfsoundinstance_functions);
	INIT_CLASS_ENTRY(videostream_class_entry, "SWFVideoStream", swfvideostream_functions);
	INIT_CLASS_ENTRY(binarydata_class_entry, "SWFBinaryData", swfbinarydata_functions);
	INIT_CLASS_ENTRY(initaction_class_entry, "SWFInitAction", swfinitaction_functions);
	INIT_CLASS_ENTRY(prebuiltclip_class_entry, "SWFPrebuiltClip", swfprebuiltclip_functions);
	INIT_CLASS_ENTRY(soundstream_class_entry, "SWFSoundStream", swfsoundstream_functions);
	INIT_CLASS_ENTRY(filter_class_entry, "SWFFilter", swffilter_functions);
	INIT_CLASS_ENTRY(filtermatrix_class_entry, "SWFFilterMatrix", swffiltermatrix_functions);
	INIT_CLASS_ENTRY(shadow_class_entry, "SWFShadow", swfshadow_functions);
	INIT_CLASS_ENTRY(blur_class_entry, "SWFBlur", swfblur_functions);
#endif
	INIT_CLASS_ENTRY(character_class_entry, "SWFCharacter", swfcharacter_functions);


	shape_class_entry_ptr = zend_register_internal_class(&shape_class_entry TSRMLS_CC);
	fill_class_entry_ptr = zend_register_internal_class(&fill_class_entry TSRMLS_CC);
	gradient_class_entry_ptr = zend_register_internal_class(&gradient_class_entry TSRMLS_CC);
	bitmap_class_entry_ptr = zend_register_internal_class(&bitmap_class_entry TSRMLS_CC);
	text_class_entry_ptr = zend_register_internal_class(&text_class_entry TSRMLS_CC);
	textfield_class_entry_ptr = zend_register_internal_class(&textfield_class_entry TSRMLS_CC);
	font_class_entry_ptr = zend_register_internal_class(&font_class_entry TSRMLS_CC);
	displayitem_class_entry_ptr = zend_register_internal_class(&displayitem_class_entry TSRMLS_CC);
	movie_class_entry_ptr = zend_register_internal_class(&movie_class_entry TSRMLS_CC);
	button_class_entry_ptr = zend_register_internal_class(&button_class_entry TSRMLS_CC);
	action_class_entry_ptr = zend_register_internal_class(&action_class_entry TSRMLS_CC);
	morph_class_entry_ptr = zend_register_internal_class(&morph_class_entry TSRMLS_CC);
	movieclip_class_entry_ptr = zend_register_internal_class(&movieclip_class_entry TSRMLS_CC);
	sprite_class_entry_ptr = zend_register_internal_class(&sprite_class_entry TSRMLS_CC);
	sound_class_entry_ptr = zend_register_internal_class(&sound_class_entry TSRMLS_CC);
#ifdef HAVE_NEW_MING
	fontchar_class_entry_ptr = zend_register_internal_class(&fontchar_class_entry TSRMLS_CC);
	buttonrecord_class_entry_ptr = zend_register_internal_class(&buttonrecord_class_entry TSRMLS_CC);
	soundinstance_class_entry_ptr = zend_register_internal_class(&soundinstance_class_entry TSRMLS_CC);
	videostream_class_entry_ptr = zend_register_internal_class(&videostream_class_entry TSRMLS_CC);
	binarydata_class_entry_ptr = zend_register_internal_class(&binarydata_class_entry TSRMLS_CC);
	initaction_class_entry_ptr = zend_register_internal_class(&initaction_class_entry TSRMLS_CC);
	prebuiltclip_class_entry_ptr = zend_register_internal_class(&prebuiltclip_class_entry TSRMLS_CC);
	soundstream_class_entry_ptr = zend_register_internal_class(&soundstream_class_entry TSRMLS_CC);
	filter_class_entry_ptr = zend_register_internal_class(&filter_class_entry TSRMLS_CC);
	filtermatrix_class_entry_ptr = zend_register_internal_class(&filtermatrix_class_entry TSRMLS_CC);
	shadow_class_entry_ptr = zend_register_internal_class(&shadow_class_entry TSRMLS_CC);
	blur_class_entry_ptr = zend_register_internal_class(&blur_class_entry TSRMLS_CC);
#endif
	character_class_entry_ptr = zend_register_internal_class(&character_class_entry TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
