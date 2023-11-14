#include <jni.h>
#include <android/log.h>
#include "jvmti.h"
#include <string>

jvmtiEnv *mJvmtiEnv;
jlong tag = 0;

static void JNICALL objectAlloc(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread,
                                jobject object, jclass object_klass, jlong size) {
    //给对象打tag，后续在objectFree()内可以通过该tag来判断是否成对出现释放
//    tag += 1;
//    jvmti_env->SetTag(object, tag);
//
//    //获取线程信息
//    jvmtiThreadInfo threadInfo;
//    jvmti_env->GetThreadInfo(thread, &threadInfo);
//
//    //获得 创建的对象的类签名
//    char *classSignature;
//    jvmti_env->GetClassSignature(object_klass, &classSignature, nullptr);
//
//
//    __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "objectAlloc --------> ");
//
//    jvmti_env->Deallocate((unsigned char *) classSignature);
}


static void Java_AttachInit(JNIEnv *env, jclass interface, jstring path) {
//
//
//    jvmtiEventCallbacks callbacks;
//    memset(&callbacks, 0, sizeof(callbacks));
//    callbacks.VMObjectAlloc = &objectAlloc;
//
//    int error = mJvmtiEnv->SetEventCallbacks(&callbacks, sizeof(callbacks));
//    __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "返回错误：= %d\n", error);
//
//    mJvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_OBJECT_ALLOC, nullptr);
}

static void Java_AttachRelease(JNIEnv *env, jclass interface) {

}

static void Java_AttachWFilters(JNIEnv *env, jclass interface, jstring packageName) {

}



extern "C" JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options, void *reserved) {
    //准备JVMTI环境
    int error;
    jint result = vm->GetEnv((void **) &mJvmtiEnv, JVMTI_VERSION_1_2);

    if (result != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "create jvmtiEnv is NULL ===== %d\n", result);
    }

    jvmtiCapabilities caps;
    mJvmtiEnv->GetPotentialCapabilities(&caps);
    mJvmtiEnv->AddCapabilities(&caps);

    __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "create jvmtiEnv done");
    return JNI_OK;
}



static const JNINativeMethod gMethod[] = {
        {"attachInit", "(Ljava/lang/String;)V",     (void *) Java_AttachInit},
        {"attachRelease", "()V",                    (void *)Java_AttachRelease},
        {"attachWFilters", "(Ljava/lang/String;)V", (void *)Java_AttachWFilters}
};



extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved){
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) & env, JNI_VERSION_1_4) != JNI_OK){
        return -1;
    }

    jclass clazz = env->FindClass("com/example/baa/Monitor");
    if (!clazz){
        return -1;
    }

    if (env->RegisterNatives(clazz, gMethod, sizeof(gMethod) / sizeof(gMethod[0]))){
        return -1;
    }
    return JNI_VERSION_1_4;
}
