//
// Description:
//    SWFSound Native Interface
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


#ifndef _Included_SWFSound
#define _Included_SWFSound
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFSound
 * Method:    nNew
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_SWFSound_nNew
  (JNIEnv *, jobject, jstring, jbyte);

#ifdef __cplusplus
}
#endif
#endif
