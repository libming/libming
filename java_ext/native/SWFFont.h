//
// Description:
//    SWFFont Native Interface
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


#ifndef _Included_SWFFont
#define _Included_SWFFont
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFFont
 * Method:    nNewFileFont
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_SWFFont_nNewFileFont
  (JNIEnv *, jobject, jstring);

/*
 * Class:     SWFFont
 * Method:    nNewBrowserFont
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_SWFFont_nNewBrowserFont
  (JNIEnv *, jobject, jstring);

/*
 * Class:     SWFFont
 * Method:    nDestroyFileFont
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFFont_nDestroyFileFont
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFFont
 * Method:    nDestroyBrowserFont
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFFont_nDestroyBrowserFont
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFFont
 * Method:    nGetStringWidth
 * Signature: (ILjava/lang/String;)F
 */
JNIEXPORT jfloat JNICALL Java_SWFFont_nGetStringWidth
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     SWFFont
 * Method:    nGetAscent
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_SWFFont_nGetAscent
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFFont
 * Method:    nGetDescent
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_SWFFont_nGetDescent
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFFont
 * Method:    nGetLeading
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_SWFFont_nGetLeading
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
