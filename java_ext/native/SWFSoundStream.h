#include <jni.h>


#ifndef _Included_SWFSoundStream
#define _Included_SWFSoundStream
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     SWFSoundStream
 * Method:    nNew
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_SWFSoundStream_nNew
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
