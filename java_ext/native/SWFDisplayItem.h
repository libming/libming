//
// Description:
//    SWFDisplayItem Native Interface
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


#ifndef _Included_SWFDisplayItem
#define _Included_SWFDisplayItem
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFDisplayItem
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFDisplayItem
 * Method:    nRotate
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nRotate
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nRotateTo
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nRotateTo
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nMove
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nMove
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nMoveTo
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nMoveTo
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nScale
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nScale
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nScaleTo
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nScaleTo
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nSkewX
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nSkewX
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nSkewXTo
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nSkewXTo
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nSkewY
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nSkewY
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nSkewYTo
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nSkewYTo
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nGetDepth
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_SWFDisplayItem_nGetDepth
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFDisplayItem
 * Method:    nSetDepth
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nSetDepth
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFDisplayItem
 * Method:    nRemove
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nRemove
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFDisplayItem
 * Method:    nSetName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nSetName
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     SWFDisplayItem
 * Method:    nSetRatio
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nSetRatio
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFDisplayItem
 * Method:    nAddColor
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nAddColor
  (JNIEnv *, jobject, jint, jint, jint, jint, jint);

/*
 * Class:     SWFDisplayItem
 * Method:    nMultColor
 * Signature: (IFFFF)V
 */
JNIEXPORT void JNICALL Java_SWFDisplayItem_nMultColor
  (JNIEnv *, jobject, jint, jfloat, jfloat, jfloat, jfloat);

#ifdef __cplusplus
}
#endif
#endif
