//
// Description:
//    SWFMorph Native Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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
