//
// Description:
//    SWFAction Native Interface
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

#ifndef _Included_SWFAction
#define _Included_SWFAction
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFAction
 * Method:    nNew
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_SWFAction_nNew
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
