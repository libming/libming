//
// Description:
//    SWFGradient Native Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
//


#include <jni.h>


#ifndef _Included_SWFGradient
#define _Included_SWFGradient
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFGradient
 * Method:    nNew
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SWFGradient_nNew
  (JNIEnv *, jobject);

/*
 * Class:     SWFGradient
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFGradient_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFGradient
 * Method:    nAddEntry
 * Signature: (IFIIII)V
 */
JNIEXPORT void JNICALL Java_SWFGradient_nAddEntry
  (JNIEnv *, jobject, jint, jfloat, jint, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
