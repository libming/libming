//
// Description:
//    SWFBitmap Native Interface
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


#ifndef _Included_SWFBitmap
#define _Included_SWFBitmap
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: initialized */
/*
 * Class:     SWFBitmap
 * Method:    nNewDblBitmap
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_SWFBitmap_nNewDblBitmap
  (JNIEnv *, jobject, jstring);

/*
 * Class:     SWFBitmap
 * Method:    nNewJpegBitmap
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_SWFBitmap_nNewJpegBitmap
  (JNIEnv *, jobject, jstring);

/*
 * Class:     SWFBitmap
 * Method:    nNewJpegWithAlpha
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_SWFBitmap_nNewJpegWithAlpha
  (JNIEnv *, jobject, jstring, jstring);


/*
 * Class:     swf_api_SWFBitmap
 * Method:    nNewDataBitmap
 * Signature: ([B)I
 */
JNIEXPORT jint JNICALL Java_SWFBitmap_nNewDataBitmap
  (JNIEnv *, jobject, jbyteArray);


/*
 * Class:     SWFBitmap
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFBitmap_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFBitmap
 * Method:    nGetWidth
 * Signature: (I)F
 */
JNIEXPORT jint JNICALL Java_SWFBitmap_nGetWidth
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFBitmap
 * Method:    nGetHeight
 * Signature: (I)F
 */
JNIEXPORT jint JNICALL Java_SWFBitmap_nGetHeight
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
