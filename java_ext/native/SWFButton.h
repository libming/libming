//
// Description:
//    SWFButton Native Interface
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


#ifndef _Included_SWFButton
#define _Included_SWFButton
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFButton
 * Method:    nNew
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SWFButton_nNew
  (JNIEnv *, jobject);

/*
 * Class:     SWFButton
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFButton_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFButton
 * Method:    nAddShape
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_SWFButton_nAddShape
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     SWFButton
 * Method:    nAddAction
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_SWFButton_nAddAction
  (JNIEnv *, jobject, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
