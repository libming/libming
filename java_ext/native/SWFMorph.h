//
// Description:
//    SWFMorph Native Interface
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


#ifndef _Included_SWFMorph
#define _Included_SWFMorph
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFMorph
 * Method:    nNew
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SWFMorph_nNew
  (JNIEnv *, jobject);

/*
 * Class:     SWFMorph
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFMorph_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFMorph
 * Method:    nGetShape1
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_SWFMorph_nGetShape1
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFMorph
 * Method:    nGetShape2
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_SWFMorph_nGetShape2
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
