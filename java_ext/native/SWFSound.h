//
// Description:
//    SWFSound Native Interface
//
// Authors:
//    krechert
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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
