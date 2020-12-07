//
// Created by ahmad.hassan on 11/16/2020.
//

#include <jni.h>
#include <cstdio>
#include "../copa/copa_receiver.h"

extern "C" JNIEXPORT void JNICALL
Java_edu_umn_congestion_MainActivity_runCopaReceiver(JNIEnv __unused *javaEnvironment, jobject __unused obj, jint port) {
    auto *copa = new COPA_RECEIVER(port);
    copa->echo_packets(copa->sender_socket);
}