//
// Description:
//    SWFGradient Native Interface
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
