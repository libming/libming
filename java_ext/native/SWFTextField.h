//
// Description:
//    SWFTextField Native Interface
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


#ifndef _Included_SWFTextField
#define _Included_SWFTextField
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFTextField
 * Method:    nNew
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SWFTextField_nNew
  (JNIEnv *, jobject);

/*
 * Class:     SWFTextField
 * Method:    nDestroy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nDestroy
  (JNIEnv *, jobject, jint);

/*
 * Class:     SWFTextField
 * Method:    nSetFont
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetFont
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFTextField
 * Method:    nSetColor
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetColor
  (JNIEnv *, jobject, jint, jint, jint, jint, jint);

/*
 * Class:     SWFTextField
 * Method:    nAddString
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nAddString
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     SWFTextField
 * Method:    nSetBounds
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetBounds
  (JNIEnv *, jobject, jint, jfloat, jfloat);
JNIEXPORT void JNICALL Java_SWFTextField_nSetHeight
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFTextField
 * Method:    nSetFlags
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetFlags
  (JNIEnv *, jobject, jint, jlong);

/*
 * Class:     SWFTextField
 * Method:    nSetAlignment
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetAlignment
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     SWFTextField
 * Method:    nSetVariableName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetVariableName
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     SWFTextField
 * Method:    nSetLeftMargin
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetLeftMargin
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFTextField
 * Method:    nSetRightMargin
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetRightMargin
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFTextField
 * Method:    nSetIndentation
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetIndentation
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFTextField
 * Method:    nSetLineSpacing
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetLineSpacing
  (JNIEnv *, jobject, jint, jfloat);

/*
 * Class:     SWFTextField
 * Method:    nSetLength
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_SWFTextField_nSetLength
  (JNIEnv *, jobject, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
