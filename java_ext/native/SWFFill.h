//
// Description:
//    SWFFill Native Interface
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


#ifndef _Included_SWFFill
#define _Included_SWFFill
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFFill
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFFill
 * Method:    nSkewX
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nSkewX
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nSkewXTo
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nSkewXTo
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nSkewY
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nSkewY
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nSkewYTo
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nSkewYTo
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nScaleX
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nScaleX
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nScaleXTo
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nScaleXTo
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nScaleY
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nScaleY
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nScaleYTo
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nScaleYTo
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nScale
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nScale
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFFill
 * Method:    nRotate
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nRotate
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nRotateTo
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nRotateTo
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFFill
 * Method:    nMove
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nMove
  (JNIEnv *, jobject, jint, jfloat, jfloat);

/*
 * Class:     SWFFill
 * Method:    nMoveTo
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFFill_nMoveTo
  (JNIEnv *, jobject, jint, jfloat, jfloat);

#ifdef __cplusplus
}
#endif
#endif
