//
// Description:
//    SWFNative Implementation
//
// Authors:
//    Jonathan Shore <jshore@e-shuppan.com>
//
// Copyright:
//    Copyright 2001 E-Publishing Group Inc.  Permission is granted to use or
//    modify this code provided that the original copyright notice is included.
//
//    This software is distributed with no warrantee of liability, merchantability,
//    or fitness for a specific purpose.
//



#include <SWFNative.h>
#include <SWFUtilities.h>
#include <SWFFill.h>
#include <SWFShape.h>
#include <SWFDisplayItem.h>
#include <SWFMovie.h>
#include <SWFMovieClip.h>
#include <SWFMorph.h>
#include <SWFFont.h>
#include <SWFBitmap.h>
#include <SWFText.h>
#include <SWFTextField.h>
#include <SWFSound.h>
#include <SWFSoundStream.h>
#include <SWFGradient.h>
#include <SWFButton.h>
#include <SWFAction.h>




//
//  SWFFill Methods
//


JNIEXPORT void JNICALL Java_SWFFill_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFFill ((SWFFill)handle);
}


JNIEXPORT void JNICALL Java_SWFFill_nSkewX (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_skewX ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nSkewXTo (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_skewXTo ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nSkewY (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_skewY ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nSkewYTo (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_skewYTo ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nScaleX (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_scaleX ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nScaleXTo (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_scaleXTo ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nScaleY (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_scaleY ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nScaleYTo (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_scaleYTo ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nScale (JNIEnv *, jobject, jint handle, jfloat xv, jfloat yv)
{
    SWFFill_scaleXY ((SWFFill)handle, xv, yv);
}


JNIEXPORT void JNICALL Java_SWFFill_nRotate (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_rotate ((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nRotateTo (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFFill_rotateTo((SWFFill)handle, v);
}


JNIEXPORT void JNICALL Java_SWFFill_nMove (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFFill_move ((SWFFill)handle, x,y);
}


JNIEXPORT void JNICALL Java_SWFFill_nMoveTo (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFFill_moveTo ((SWFFill)handle, x,y);
}




//
//  SWFShape Methods
//


JNIEXPORT jint JNICALL Java_SWFShape_nNew (JNIEnv *, jobject)
{
    return (jint)newSWFShape();
}


JNIEXPORT void JNICALL Java_SWFShape_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFShape ((SWFShape)handle);
}


JNIEXPORT void JNICALL Java_SWFShape_nMovePen (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFShape_movePen ((SWFShape)handle, x, y);
}


JNIEXPORT void JNICALL Java_SWFShape_nMovePenTo (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFShape_movePenTo ((SWFShape)handle, x, y);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawLine (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFShape_drawLine ((SWFShape)handle, x, y);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawLineTo (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFShape_drawLineTo ((SWFShape)handle, x, y);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawLineToRelative (JNIEnv *, jobject, jint handle, jfloat dx, jfloat dy)
{
    // might be wrong (no equiv call)
    SWFShape_drawLineTo ((SWFShape)handle, dx, dy);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawCurve (JNIEnv *, jobject, jint handle, jfloat bx, jfloat by, jfloat cx, jfloat cy)
{
    SWFShape_drawCurve ((SWFShape)handle, bx, by, cx, cy);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawCurveTo (JNIEnv *, jobject, jint handle, jfloat bx, jfloat by, jfloat cx, jfloat cy)
{
    SWFShape_drawCurveTo ((SWFShape)handle, bx, by, cx, cy);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawCubic (JNIEnv *, jobject, jint handle, jfloat bx, jfloat by, jfloat cx, jfloat cy, jfloat dx, jfloat dy)
{
    SWFShape_drawCubic ((SWFShape)handle, bx, by, cx, cy, dx, dy);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawCubicTo (JNIEnv *, jobject, jint handle, jfloat bx, jfloat by, jfloat cx, jfloat cy, jfloat dx, jfloat dy)
{
    SWFShape_drawCubicTo ((SWFShape)handle, bx, by, cx, cy, dx, dy);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawArc (JNIEnv *, jobject, jint handle, jfloat r, jfloat from, jfloat to)
{
    SWFShape_drawArc ((SWFShape)handle, r, from, to);
}


JNIEXPORT void JNICALL Java_SWFShape_nDrawCircle (JNIEnv *, jobject, jint handle, jfloat r)
{
    SWFShape_drawCircle ((SWFShape)handle, r);
}

JNIEXPORT jfloat JNICALL Java_SWFShape_nGetPenX (JNIEnv *, jobject, jint handle)
{
	return SWFShape_getPenX( (SWFShape)handle );
}

JNIEXPORT jfloat JNICALL Java_SWFShape_nGetPenY (JNIEnv *, jobject, jint handle)
{
	return SWFShape_getPenY( (SWFShape)handle );
}

JNIEXPORT void JNICALL Java_SWFShape_nDrawGlyph (JNIEnv *, jobject, jint handle, jint Hfont, jint c)
{
    SWFShape_drawGlyph ((SWFShape)handle, (SWFFont)Hfont, c);
}


JNIEXPORT void JNICALL Java_SWFShape_nEnd (JNIEnv *, jobject, jint handle)
{
    SWFShape_end ((SWFShape)handle);
}


JNIEXPORT void JNICALL Java_SWFShape_nSetLine (JNIEnv *, jobject, jint handle, jshort width, jint r, jint g, jint b, jint alpha)
{
    SWFShape_setLine ((SWFShape)handle, width, r, g, b, alpha);
}


JNIEXPORT jint JNICALL Java_SWFShape_nAddBitmapFill (JNIEnv *, jobject, jint handle, jint Hbitmap, jshort flags)
{
    return (jint)SWFShape_addBitmapFill ((SWFShape)handle, (SWFBitmap)Hbitmap, flags);
}


JNIEXPORT jint JNICALL Java_SWFShape_nAddGradientFill (JNIEnv *, jobject, jint handle, jint Hgradient, jshort flags)
{
    return (jint)SWFShape_addGradientFill ((SWFShape)handle, (SWFGradient)Hgradient, flags);
}


JNIEXPORT jint JNICALL Java_SWFShape_nAddSolidFill (JNIEnv *, jobject, jint handle, jint r, jint g, jint b, jint alpha)
{
    return (jint)SWFShape_addSolidFill ((SWFShape)handle, r,g,b, alpha);
}


JNIEXPORT void JNICALL Java_SWFShape_nSetLeftFill (JNIEnv *, jobject, jint handle, jint Hfill)
{
    SWFShape_setLeftFill ((SWFShape)handle, (SWFFill)Hfill);
}


JNIEXPORT void JNICALL Java_SWFShape_nSetRightFill (JNIEnv *, jobject, jint handle, jint Hfill)
{
    SWFShape_setRightFill ((SWFShape)handle, (SWFFill)Hfill);
}



//
//  SWFDisplayItem Methods
//



JNIEXPORT void JNICALL Java_SWFDisplayItem_nDestroy (JNIEnv *, jobject, jint handle)
{
    // do nothing
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nRotate (JNIEnv *, jobject, jint handle, jfloat r)
{
    SWFDisplayItem_rotate ((SWFDisplayItem)handle, r);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nRotateTo (JNIEnv *, jobject, jint handle, jfloat r)
{
    SWFDisplayItem_rotateTo ((SWFDisplayItem)handle, r);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nMove (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFDisplayItem_move ((SWFDisplayItem)handle, x, y);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nMoveTo (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFDisplayItem_moveTo ((SWFDisplayItem)handle, x, y);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nScale (JNIEnv *, jobject, jint handle, jfloat xs, jfloat ys)
{
    SWFDisplayItem_scale ((SWFDisplayItem)handle, xs, ys);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nScaleTo (JNIEnv *, jobject, jint handle, jfloat xs, jfloat ys)
{
    SWFDisplayItem_scaleTo ((SWFDisplayItem)handle, xs, ys);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nSkewX (JNIEnv *, jobject, jint handle, jfloat s)
{
    SWFDisplayItem_skewX ((SWFDisplayItem)handle, s);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nSkewXTo (JNIEnv *, jobject, jint handle, jfloat s)
{
    SWFDisplayItem_skewXTo ((SWFDisplayItem)handle, s);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nSkewY (JNIEnv *, jobject, jint handle, jfloat s)
{
    SWFDisplayItem_skewY ((SWFDisplayItem)handle, s);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nSkewYTo (JNIEnv *, jobject, jint handle, jfloat s)
{
    SWFDisplayItem_skewYTo ((SWFDisplayItem)handle, s);
}


JNIEXPORT jint JNICALL Java_SWFDisplayItem_nGetDepth (JNIEnv *, jobject, jint handle)
{
    return (jint)SWFDisplayItem_getDepth ((SWFDisplayItem)handle);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nSetDepth (JNIEnv *, jobject, jint handle, jint depth)
{
    SWFDisplayItem_setDepth ((SWFDisplayItem)handle, depth);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nRemove (JNIEnv *, jobject, jint handle)
{
    SWFDisplayItem_remove ((SWFDisplayItem)handle);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nSetName (JNIEnv* env, jobject, jint handle, jstring name)
{
    const char* sname = env->GetStringUTFChars(name, NULL);
    SWFDisplayItem_setName ((SWFDisplayItem)handle, sname);
    env->ReleaseStringUTFChars (name, sname);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nSetRatio (JNIEnv *, jobject, jint handle, jfloat r)
{
    SWFDisplayItem_setRatio ((SWFDisplayItem)handle, r);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nAddColor (JNIEnv *, jobject, jint handle, jint r, jint g, jint b, jint alpha)
{
    SWFDisplayItem_addColor ((SWFDisplayItem)handle, r,g,b, alpha);
}


JNIEXPORT void JNICALL Java_SWFDisplayItem_nMultColor (JNIEnv *, jobject, jint handle, jfloat r, jfloat g, jfloat b, jfloat alpha)
{
    SWFDisplayItem_multColor ((SWFDisplayItem)handle, r,g,b, alpha);
}



//
//  SWFMovie Methods
//


JNIEXPORT jint JNICALL Java_SWFMovie_nNew (JNIEnv *, jobject)
{
    return (jint)newSWFMovie();
}


JNIEXPORT void JNICALL Java_SWFMovie_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFMovie ((SWFMovie)handle);
}


JNIEXPORT void JNICALL Java_SWFMovie_nSetScale (JNIEnv *, jobject, jfloat scale)
{
    Ming_setScale (scale);
}


JNIEXPORT void JNICALL Java_SWFMovie_nSetCubicThreshold (JNIEnv *, jobject, jint v)
{
    Ming_setCubicThreshold (v);
}


JNIEXPORT void JNICALL Java_SWFMovie_nSetRate (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFMovie_setRate ((SWFMovie)handle, v);
}


JNIEXPORT void JNICALL Java_SWFMovie_nSetDimension (JNIEnv *, jobject, jint handle, jint width, jint height)
{
    SWFMovie_setDimension ((SWFMovie)handle, width, height);
}


JNIEXPORT void JNICALL Java_SWFMovie_nSetNumberOfFrames (JNIEnv *, jobject, jint handle, jint n)
{
    SWFMovie_setNumberOfFrames ((SWFMovie)handle, n);
}

JNIEXPORT void JNICALL Java_SWFMovie_nUseSWFVersion (JNIEnv *, jobject, jint version)
{
	Ming_useSWFVersion(version);
}

JNIEXPORT void JNICALL Java_SWFMovie_nSetBackground (JNIEnv *, jobject, jint handle, jint r, jint g, jint b)
{
    SWFMovie_setBackground ((SWFMovie)handle, r,g,b);
}


JNIEXPORT void JNICALL Java_SWFMovie_nSetSoundStream (JNIEnv *, jobject, jint handle, jint Hsound_stream)
{
    SWFMovie_setSoundStream ((SWFMovie)handle, (SWFSoundStream)Hsound_stream);
}


JNIEXPORT jint JNICALL Java_SWFMovie_nAdd (JNIEnv *, jobject, jint handle, jint Hobject)
{
    return (jint)SWFMovie_add ((SWFMovie)handle, (SWFBlock)Hobject);
}


JNIEXPORT void JNICALL Java_SWFMovie_nRemove (JNIEnv *, jobject, jint handle, jint Hobject)
{
    SWFMovie_remove ((SWFMovie)handle, (SWFDisplayItem)Hobject);
}


JNIEXPORT void JNICALL Java_SWFMovie_nNextFrame (JNIEnv *, jobject, jint handle)
{
    SWFMovie_nextFrame ((SWFMovie)handle);
}


JNIEXPORT void JNICALL Java_SWFMovie_nLabelFrame (JNIEnv* env, jobject, jint handle, jstring label)
{
    const char* slabel = env->GetStringUTFChars (label, NULL);
    SWFMovie_labelFrame ((SWFMovie)handle, (char*)slabel);
    env->ReleaseStringUTFChars (label, slabel);
}


JNIEXPORT jint JNICALL Java_SWFMovie_nSetCompression(JNIEnv *, jobject, jint handle, jint level)
{
	return Ming_setSWFCompression(level);
}


JNIEXPORT void JNICALL Java_SWFMovie_nSave (JNIEnv* env, jobject, jint handle, jstring filename)
{
    // should add error checking
    const char* sfilename = env->GetStringUTFChars (filename, NULL);
    SWFMovie_save ((SWFMovie)handle, (char*)sfilename);
    env->ReleaseStringUTFChars (filename, sfilename);
}


JNIEXPORT jbyteArray JNICALL Java_SWFMovie_nOutput (JNIEnv* env, jobject, jint handle)
{
    // get output into stream
    StringStream mystream;
    SWFMovie_output ((SWFMovie)handle, StringStream::hook, (void*)&mystream);

    jbyteArray narray = env->NewByteArray (mystream.length());
    env->SetByteArrayRegion (narray, 0, mystream.length(), (jbyte*)mystream.getBytes());
    return narray;
}



//
//  SWFMovieClip Methods
//


JNIEXPORT jint JNICALL Java_SWFMovieClip_nNew (JNIEnv *, jobject)
{
    return (jint)newSWFMovieClip();
}


JNIEXPORT void JNICALL Java_SWFMovieClip_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFMovieClip ((SWFMovieClip)handle);
}


JNIEXPORT void JNICALL Java_SWFMovieClip_nSetFrames (JNIEnv *, jobject, jint handle, jint v)
{
    SWFMovieClip_setNumberOfFrames ((SWFMovieClip)handle, v);
}


JNIEXPORT jint JNICALL Java_SWFMovieClip_nAdd (JNIEnv *, jobject, jint handle, jint Hobject)
{
    return (jint)SWFMovieClip_add ((SWFMovieClip)handle, (SWFBlock)Hobject);
}


JNIEXPORT void JNICALL Java_SWFMovieClip_nRemove (JNIEnv *, jobject, jint handle, jint Hobject)
{
    SWFMovieClip_remove ((SWFMovieClip)handle, (SWFDisplayItem)Hobject);
}


JNIEXPORT void JNICALL Java_SWFMovieClip_nNextFrame (JNIEnv *, jobject, jint handle)
{
    SWFMovieClip_nextFrame ((SWFMovieClip)handle);
}


JNIEXPORT void JNICALL Java_SWFMovieClip_nLabelFrame (JNIEnv* env, jobject, jint handle, jstring name)
{
    const char* sname = env->GetStringUTFChars (name, NULL);
    SWFMovieClip_labelFrame ((SWFMovieClip)handle, (char*)sname);
    env->ReleaseStringUTFChars (name, sname);
}



//
//  SWFMorph Methods
//


JNIEXPORT jint JNICALL Java_SWFMorph_nNew (JNIEnv *, jobject)
{
    return (jint)newSWFMorphShape();
}


JNIEXPORT void JNICALL Java_SWFMorph_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFMorph ((SWFMorph)handle);
}


JNIEXPORT jint JNICALL Java_SWFMorph_nGetShape1 (JNIEnv *, jobject, jint handle)
{
    return (jint)SWFMorph_getShape1 ((SWFMorph)handle);
}


JNIEXPORT jint JNICALL Java_SWFMorph_nGetShape2 (JNIEnv *, jobject, jint handle)
{
    return (jint)SWFMorph_getShape2 ((SWFMorph)handle);
}




//
//  SWFFont Methods
//


JNIEXPORT jint JNICALL Java_SWFFont_nNewFileFont (JNIEnv* env, jobject, jstring font)
{
    const char* sfont = env->GetStringUTFChars (font, NULL);
    FILE* file = fopen (sfont, "rb");

    jint obj = (jint)loadSWFFontFromFile (file);

    env->ReleaseStringUTFChars (font, sfont);

    return obj;
}


JNIEXPORT jint JNICALL Java_SWFFont_nNewBrowserFont (JNIEnv* env, jobject, jstring font)
{
    const char* sfont = env->GetStringUTFChars (font, NULL);
    jint obj = (jint)newSWFBrowserFont ((char*)sfont);
    env->ReleaseStringUTFChars (font, sfont);

    return obj;
}



JNIEXPORT void JNICALL Java_SWFFont_nDestroyFileFont (JNIEnv *, jobject, jint handle)
{
    destroySWFFont ((SWFFont)handle);
}


JNIEXPORT void JNICALL Java_SWFFont_nDestroyBrowserFont (JNIEnv *, jobject, jint handle)
{
    destroySWFBrowserFont ((SWFBrowserFont)handle);
}


JNIEXPORT jfloat JNICALL Java_SWFFont_nGetStringWidth (JNIEnv* env, jobject, jint handle, jstring str)
{
    const char* sstr = env->GetStringUTFChars (str, NULL);
    jfloat w = SWFFont_getStringWidth ((SWFFont)handle, (const unsigned char*) sstr);
    env->ReleaseStringUTFChars (str, sstr);

    return w;
}


JNIEXPORT jfloat JNICALL Java_SWFFont_nGetAscent (JNIEnv *, jobject, jint handle)
{
    return SWFFont_getAscent ((SWFFont)handle);
}


JNIEXPORT jfloat JNICALL Java_SWFFont_nGetDescent (JNIEnv *, jobject, jint handle)
{
    return SWFFont_getDescent ((SWFFont)handle);
}


JNIEXPORT jfloat JNICALL Java_SWFFont_nGetLeading (JNIEnv *, jobject, jint handle)
{
    return SWFFont_getLeading ((SWFFont)handle);
}




//
//  SWFBitmap Methods
//


JNIEXPORT jint JNICALL Java_SWFBitmap_nNewDblBitmap (JNIEnv* env, jobject, jstring filename)
{
    const char* sfilename = env->GetStringUTFChars (filename, NULL);
    FILE* file = fopen (sfilename, "rb");

    jint obj = (jint)newSWFDBLBitmap (file);

    env->ReleaseStringUTFChars (filename, sfilename);

    return obj;
}


JNIEXPORT jint JNICALL Java_SWFBitmap_nNewJpegBitmap (JNIEnv* env, jobject, jstring filename)
{
    const char* sfilename = env->GetStringUTFChars (filename, NULL);
    FILE* file = fopen (sfilename, "rb");

    jint obj = (jint)newSWFJpegBitmap (file);

    env->ReleaseStringUTFChars (filename, sfilename);

    return obj;
}


JNIEXPORT jint JNICALL Java_SWFBitmap_nNewDataBitmap (JNIEnv* env, jobject, jbyteArray array)
{
    jboolean copy = true;
    int len = 
	env->GetArrayLength(array);
    jbyte* data = 
	env->GetByteArrayElements(array, &copy);

    // create input for image
    SWFInput input = 
	newSWFInput_allocedBuffer ((unsigned char*)data, len);
    
    // create image
    jint obj = (jint)newSWFJpegBitmap_fromInput (input);
    return obj;
}



JNIEXPORT jint JNICALL Java_SWFBitmap_nNewJpegWithAlpha (JNIEnv* env, jobject, jstring bitmap, jstring alpha)
{
    const char* sbitmap = env->GetStringUTFChars (bitmap, NULL);
    const char* salpha = env->GetStringUTFChars (alpha, NULL);
    FILE* Bfile = fopen (sbitmap, "rb");
    FILE* Afile = fopen (salpha, "rb");

    jint obj = (jint)newSWFJpegWithAlpha (Bfile, Afile);

    env->ReleaseStringUTFChars (bitmap, sbitmap);
    env->ReleaseStringUTFChars (alpha, salpha);

    return obj;
}


JNIEXPORT void JNICALL Java_SWFBitmap_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFBitmap ((SWFBitmap)handle);
}


JNIEXPORT jint JNICALL Java_SWFBitmap_nGetWidth (JNIEnv *, jobject, jint handle)
{
    return SWFBitmap_getWidth ((SWFBitmap)handle);
}


JNIEXPORT jint JNICALL Java_SWFBitmap_nGetHeight (JNIEnv *, jobject, jint handle)
{
    return SWFBitmap_getHeight ((SWFBitmap)handle);
}

JNIEXPORT void JNICALL Java_SWFTextField_nSetHeight (JNIEnv *, jobject, jint handle, jfloat height)
{
	 SWFTextField_setHeight ((SWFTextField)handle, height);
}

//
//  SWFText Methods
//


JNIEXPORT jint JNICALL Java_SWFText_nNew (JNIEnv *, jobject)
{
    return (jint)newSWFText2();
}


JNIEXPORT void JNICALL Java_SWFText_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFText ((SWFText)handle);
}


JNIEXPORT void JNICALL Java_SWFText_nSetFont (JNIEnv *, jobject, jint handle, jint Hfont)
{
    SWFText_setFont ((SWFText)handle, (SWFFont)Hfont);
}


JNIEXPORT void JNICALL Java_SWFText_nSetColor (JNIEnv *, jobject, jint handle, jint r, jint g, jint b, jint alpha)
{
    SWFText_setColor ((SWFText)handle, r,g,b, alpha);
}


JNIEXPORT void JNICALL Java_SWFText_nAddString (JNIEnv* env, jobject, jint handle, jstring text)
{
    const char* stext = env->GetStringUTFChars (text, NULL);
    SWFText_addString ((SWFText)handle, stext, NULL);
    env->ReleaseStringUTFChars (text, stext);
}


JNIEXPORT void JNICALL Java_SWFText_nSetHeight (JNIEnv *, jobject, jint handle, jfloat height)
{
    SWFText_setHeight ((SWFText)handle, height);
}


JNIEXPORT void JNICALL Java_SWFText_nSetSpacing (JNIEnv *, jobject, jint handle, jfloat spacing)
{
    SWFText_setSpacing ((SWFText)handle, spacing);
}


JNIEXPORT jfloat JNICALL Java_SWFText_nGetAscent (JNIEnv *, jobject, jint handle)
{
    return SWFText_getAscent ((SWFText)handle);
}


JNIEXPORT jfloat JNICALL Java_SWFText_nGetDescent (JNIEnv *, jobject, jint handle)
{
    return SWFText_getDescent ((SWFText)handle);
}


JNIEXPORT jfloat JNICALL Java_SWFText_nGetLeading (JNIEnv *, jobject, jint handle)
{
    return SWFText_getLeading ((SWFText)handle);
}


JNIEXPORT void JNICALL Java_SWFText_nMoveTo (JNIEnv *, jobject, jint handle, jfloat x, jfloat y)
{
    SWFText_moveTo ((SWFText)handle, x, y);
}



//
//  SWFTextField Methods
//


JNIEXPORT jint JNICALL Java_SWFTextField_nNew (JNIEnv *, jobject)
{
    return (jint)newSWFTextField();
}


JNIEXPORT void JNICALL Java_SWFTextField_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFTextField ((SWFTextField)handle);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetFont (JNIEnv *, jobject, jint handle, jint Hfont)
{
    SWFTextField_setFont ((SWFTextField)handle, (SWFBlock)Hfont);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetColor (JNIEnv *, jobject, jint handle, jint r, jint g, jint b, jint alpha)
{
    SWFTextField_setColor ((SWFTextField)handle, r,g,b, alpha);
}


JNIEXPORT void JNICALL Java_SWFTextField_nAddString (JNIEnv* env, jobject, jint handle, jstring text)
{
    const char* stext = env->GetStringUTFChars (text, NULL);
    SWFTextField_addString ((SWFTextField)handle, (char*)stext);
    env->ReleaseStringUTFChars (text, stext);
}



JNIEXPORT void JNICALL Java_SWFTextField_nSetBounds (JNIEnv *, jobject, jint handle, jfloat width, jfloat height)
{
    SWFTextField_setBounds ((SWFTextField)handle, width, height);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetFlags (JNIEnv *, jobject, jint handle, jlong flags)
{
    SWFTextField_setFlags ((SWFTextField)handle, flags);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetAlignment (JNIEnv *, jobject, jint handle, jint align)
{
    SWFTextField_setAlignment ((SWFTextField)handle, (SWFTextFieldAlignment)align);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetVariableName (JNIEnv* env, jobject, jint handle, jstring name)
{
    const char* sname = env->GetStringUTFChars (name, NULL);
    SWFTextField_setVariableName ((SWFTextField)handle, (char*)sname);
    env->ReleaseStringUTFChars (name, sname);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetLeftMargin (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFTextField_setLeftMargin ((SWFTextField)handle, v);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetRightMargin (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFTextField_setRightMargin ((SWFTextField)handle, v);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetIndentation (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFTextField_setIndentation ((SWFTextField)handle, v);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetLineSpacing (JNIEnv *, jobject, jint handle, jfloat v)
{
    SWFTextField_setLineSpacing ((SWFTextField)handle, v);
}


JNIEXPORT void JNICALL Java_SWFTextField_nSetLength (JNIEnv *, jobject, jint handle, jint len)
{
    SWFTextField_setLength ((SWFTextField)handle, len);
}




//
//  SWFSound Methods
//


JNIEXPORT jint JNICALL Java_SWFSound_nNew (JNIEnv* env, jobject, jstring filename, jbyte flags)
{
    const char* sfilename = env->GetStringUTFChars (filename, NULL);
    FILE* file = fopen (sfilename, "rb");

    jint obj = (jint)newSWFSound (file, flags);

    env->ReleaseStringUTFChars (filename, sfilename);

    return obj;
}

//
// SWFSoundStream
//
JNIEXPORT jint JNICALL Java_SWFSoundStream_nNew (JNIEnv* env, jobject, jstring filename)
	
{
    const char* sfilename = env->GetStringUTFChars (filename, NULL);
    FILE* file = fopen (sfilename, "rb");
    jint obj = (jint)newSWFSoundStream (file);
    env->ReleaseStringUTFChars (filename, sfilename);                                                                                                                                                                                          
    return obj;
}


//                                                                                                                                                  
// SWFSoundStream                                                                                                                                            //
JNIEXPORT jint JNICALL Java_SWFVideoStream_nNewFile (JNIEnv* env, jobject, jstring filename)                                                                                                                                                                                       
{                                                                                                                                                                                             
    const char* sfilename = env->GetStringUTFChars (filename, NULL);
    FILE* file = fopen (sfilename, "rb"); 
    jint obj = (jint)newSWFVideoStream_fromFile (file);
    env->ReleaseStringUTFChars (filename, sfilename);                                                                                                                                                                                          
    return obj;
}

JNIEXPORT jint JNICALL Java_SWFVideoStream_nNew (JNIEnv * , jobject, jstring filename)
	
{ 
	jint obj = (jint)newSWFVideoStream ();
	return obj;
}


JNIEXPORT void JNICALL Java_SWFVideoStream_nSetDimension(JNIEnv *, jobject, jint handle, jint width, jint height)
{
	SWFVideoStream_setDimension((SWFVideoStream)handle, width, height);
}

JNIEXPORT jint JNICALL Java_SWFVideoStream_nGetNumFrames(JNIEnv *, jobject, jint handle) 
{
	return SWFVideoStream_getNumFrames((SWFVideoStream)handle);
}
//
//  SWFGradient Methods
//


JNIEXPORT jint JNICALL Java_SWFGradient_nNew (JNIEnv *, jobject)
{
    return (jint)newSWFGradient();
}


JNIEXPORT void JNICALL Java_SWFGradient_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFGradient ((SWFGradient)handle);
}



JNIEXPORT void JNICALL Java_SWFGradient_nAddEntry (JNIEnv *, jobject, jint handle, jfloat ratio, jint r, jint g, jint b, jint alpha)
{
    SWFGradient_addEntry ((SWFGradient)handle, ratio, r,g,b, alpha);
}


//
//  SWFButton Methods
//


JNIEXPORT jint JNICALL Java_SWFButton_nNew (JNIEnv *, jobject)
{
    return (jint)newSWFButton();
}


JNIEXPORT void JNICALL Java_SWFButton_nDestroy (JNIEnv *, jobject, jint handle)
{
    destroySWFButton ((SWFButton)handle);
}


JNIEXPORT void JNICALL Java_SWFButton_nAddShape (JNIEnv *, jobject, jint handle, jint Hshape, jint flags)
{
    SWFButton_addShape ((SWFButton)handle, (SWFCharacter)Hshape, flags);
}


JNIEXPORT void JNICALL Java_SWFButton_nAddAction (JNIEnv *, jobject, jint handle, jint Haction, jint flags)
{
    SWFButton_addAction ((SWFButton)handle, (SWFAction)Haction, flags);
}




//
//  SWFAction Methods
//


JNIEXPORT jint JNICALL Java_SWFAction_nNew (JNIEnv* env, jobject, jstring script)
{
    const char* sscript = env->GetStringUTFChars (script, NULL);
    jint obj = (jint)compileSWFActionCode ((char*)sscript);
    env->ReleaseStringUTFChars (script, sscript);

    return obj;
}
