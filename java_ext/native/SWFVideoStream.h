#include <jni.h>


#ifndef _Included_SWFVideoStream
#define _Included_SWFVideoStream
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jint JNICALL Java_SWFVideoStream_nNew
	(JNIEnv *, jobject);

JNIEXPORT jint JNICALL Java_SWFVideoStream_nNewFile
	(JNIEnv *env, jobject, jstring);

JNIEXPORT void JNICALL Java_SWFVideoStream_nSetDimension
	(JNIEnv *, jobject, jint, jint, jint);

JNIEXPORT jint JNICALL Java_SWFVideoStream_nGetNumFrames
	(JNIEnv *, object, jint);

#ifdef __cplusplus
}
#endif
#endif
