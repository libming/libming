//
// Description:
//    SWFAction Native Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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
