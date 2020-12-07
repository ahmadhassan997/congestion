//
// Created by ahmad.hassan on 11/30/2020.
//

#include <jni.h>
#include <cstdio>
#include <android/log.h>
#include "../rtcp/rtcp.c"

extern "C" JNIEXPORT void JNICALL
Java_edu_umn_congestion_MainActivity_runRtcpReceiver(JNIEnv *javaEnvironment, jobject obj, jstring j_src_ip,
jstring j_tgt_ip, jstring j_src_port, jstring j_tgt_port, jstring j_f_name, jdouble j_duration) {
__android_log_print(ANDROID_LOG_DEBUG, "rtcp", "start of rtcp ");
const char *src_ip = javaEnvironment->GetStringUTFChars(j_src_ip, nullptr);
const char *tgt_ip = javaEnvironment->GetStringUTFChars(j_tgt_ip, nullptr);
const char *src_port = javaEnvironment->GetStringUTFChars(j_src_port, nullptr);
const char *tgt_port = javaEnvironment->GetStringUTFChars(j_tgt_port, nullptr);
const char *file_name = javaEnvironment->GetStringUTFChars(j_f_name, nullptr);
run(src_ip, tgt_ip, src_port, tgt_port, file_name, j_duration);
javaEnvironment->ReleaseStringUTFChars(j_src_ip, src_ip);
javaEnvironment->ReleaseStringUTFChars(j_tgt_ip, tgt_ip);
javaEnvironment->ReleaseStringUTFChars(j_src_port, src_port);
javaEnvironment->ReleaseStringUTFChars(j_tgt_port, tgt_port);
javaEnvironment->ReleaseStringUTFChars(j_f_name, file_name);
}