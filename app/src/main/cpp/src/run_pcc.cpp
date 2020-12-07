//
// Created by ahmad.hassan on 11/17/2020.
//
#include <jni.h>
#include <cstdio>
#include "../pcc/app/appserver.h"

extern "C" JNIEXPORT void JNICALL
Java_edu_umn_congestion_MainActivity_runPccReceiver(JNIEnv __unused *javaEnvironment, jobject __unused obj, jint port) {
    new AppServer(port);
}
