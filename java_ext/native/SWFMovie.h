//
// Description:
//    SWFMovie Native Interface
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


#ifndef _Included_SWFMovie
#define _Included_SWFMovie
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFMovie
 * Method:    nNew
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SWFMovie_nNew
  (JNIEnv *, jobject);

/*
 * Class:     SWFMovie
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFMovie
 * Method:    nSetScale
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nSetScale
  (JNIEnv *, jobject, jfloat);

/*
 * Class:     SWFMovie
 * Method:    nSetCubicThreshold
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nSetCubicThreshold
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFMovie
 * Method:    nUseSWFVersion
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nUseSWFVersion
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFMovie
 * Method:    nSetRate
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nSetRate
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFMovie
 * Method:    nSetDimension
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nSetDimension
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     SWFMovie
 * Method:    nSetFrames
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nSetFrames
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFMovie
 * Method:    nSetBackground
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nSetBackground
  (JNIEnv *, jobject, jint, jint, jint, jint);

/*
 * Class:     SWFMovie
 * Method:    nSetSoundStream
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nSetSoundStream
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFMovie
 * Method:    nAdd
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_SWFMovie_nAdd
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFMovie
 * Method:    nRemove
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nRemove
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFMovie
 * Method:    nNextFrame
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nNextFrame
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFMovie
 * Method:    nLabelFrame
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nLabelFrame
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     SWFMovie
 * Method:    nSetCompression
 */

JNIEXPORT jint JNICALL Java_SWFMovie_nSetCompression
  (JNIEnv *, jobject, jint handle, jint level);

/*
 * Class:     SWFMovie
 * Method:    nSave
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_SWFMovie_nSave
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     SWFMovie
 * Method:    nOutput
 * Signature: (I)[B
 */
JNIEXPORT jbyteArray JNICALL Java_SWFMovie_nOutput
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
