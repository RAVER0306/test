#include "com_sdk_jni_kestrel_JNISayHello.h"
#include <stdio.h>

JNIEXPORT jstring JNICALL Java_com_sdk_jni_kestrel_JNISayHello_sayHello
  (JNIEnv *, jobject, jstring);
{
    printf("Hello world ! \n");
    return;
}
