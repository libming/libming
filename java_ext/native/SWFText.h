//
// Description:
//    SWFText Native Interface
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


#ifndef _Included_SWFText
#define _Included_SWFText
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFText
 * Method:    nNew
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SWFText_nNew
  (JNIEnv *, jobject);

/*
 * Class:     SWFText
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFText_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFText
 * Method:    nSetFont
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFText_nSetFont
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFText
 * Method:    nSetColor
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL Java_SWFText_nSetColor
  (JNIEnv *, jobject, jint, jint, jint, jint, jint);

/*
 * Class:     SWFText
 * Method:    nAddString
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_SWFText_nAddString
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     SWFText
 * Method:    nSetHeight
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFText_nSetHeight
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFText
 * Method:    nSetSpacing
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFText_nSetSpacing
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFText
 * Method:    nGetAscent
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_SWFText_nGetAscent
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFText
 * Method:    nGetDescent
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_SWFText_nGetDescent
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFText
 * Method:    nGetLeading
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_SWFText_nGetLeading
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFText
 * Method:    nMoveTo
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFText_nMoveTo
  (JNIEnv *, jobject, jint, jfloat, jfloat);

#ifdef __cplusplus
}
#endif
#endif
