//
// Description:
//    SWFShape Native Interface
//
// Authors:
//    Jonathan Shore <jshore@e-shuppan.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 E-Publishing Group Inc.  Permission is granted to use or
//    modify this code provided that the original copyright notice is included.
//
//    This software is distributed with no warrantee of liability, merchantability,
//    or fitness for a specific purpose.
//


#include <jni.h>


#ifndef _Included_SWFShape
#define _Included_SWFShape
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFShape
 * Method:    nNew
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SWFShape_nNew
  (JNIEnv *, jobject);

/*
 * Class:     SWFShape
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFShape
 * Method:    nMovePen
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nMovePen
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nMovePenTo
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nMovePenTo
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawLine
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawLine
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawLineTo
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawLineTo
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawLineToRelative
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawLineToRelative
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawCurve
 * Signature: (IFFFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawCurve
  (JNIEnv *, jobject, jint, jfloat, jfloat, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawCurveTo
 * Signature: (IFFFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawCurveTo
  (JNIEnv *, jobject, jint, jfloat, jfloat, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawCubic
 * Signature: (IFFFFFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawCubic
  (JNIEnv *, jobject, jint, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawCubicTo
 * Signature: (IFFFFFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawCubicTo
  (JNIEnv *, jobject, jint, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawArc
 * Signature: (IFFF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawArc
  (JNIEnv *, jobject, jint, jfloat, jfloat, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawCircle
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawCircle
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFShape
 * Method:    nDrawGlyph
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nDrawGlyph
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     SWFShape
 * Method:    nEnd
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nEnd
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFShape
 * Method:    nSetLine
 * Signature: (ISIIII)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nSetLine
  (JNIEnv *, jobject, jint, jshort, jint, jint, jint, jint);

/*
 * Class:     SWFShape
 * Method:    nAddBitmapFill
 * Signature: (IIS)I
 */
JNIEXPORT jint JNICALL Java_SWFShape_nAddBitmapFill
  (JNIEnv *, jobject, jint, jint, jshort);

/*
 * Class:     SWFShape
 * Method:    nAddGradientFill
 * Signature: (IIS)I
 */
JNIEXPORT jint JNICALL Java_SWFShape_nAddGradientFill
  (JNIEnv *, jobject, jint, jint, jshort);

/*
 * Class:     SWFShape
 * Method:    nAddSolidFill
 * Signature: (IIIII)I
 */
JNIEXPORT jint JNICALL Java_SWFShape_nAddSolidFill
  (JNIEnv *, jobject, jint, jint, jint, jint, jint);

/*
 * Class:     SWFShape
 * Method:    nSetLeftFill
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nSetLeftFill
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFShape
 * Method:    nSetRightFill
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFShape_nSetRightFill
  (JNIEnv *, jobject, jint, jint);

JNIEXPORT jfloat JNICALL Java_SWFShape_nGetPenX
  (JNIEnv *, jobject, jint);

JNIEXPORT jfloat JNICALL Java_SWFShape_nGetPenY
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
