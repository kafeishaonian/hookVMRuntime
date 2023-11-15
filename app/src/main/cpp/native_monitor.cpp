#include <jni.h>
#include <android/log.h>
#include "jvmti.h"
#include <string>
#include <chrono>
#include <cstdint>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <mutex>

jvmtiEnv *mJvmtiEnv;
jlong tag = 0;

using namespace std;
string mPackageName;


jboolean findFilter(const char *name) {
    string tempstr = name;
    int idx;
    idx = tempstr.find(mPackageName);
    if (idx == string::npos) {
        idx = tempstr.find("OutOfMemoryError");
        if (idx == string::npos) {
            return JNI_FALSE;
        } else {
            return JNI_TRUE;
        }
    } else {
        return JNI_TRUE;
    }
}

string GetCurrentSystemTime() {
    auto now = chrono::system_clock::now();
    ::uint64_t dis_millseconds = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count()
            - chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count() * 1000;
    ::time_t tt = chrono::system_clock::to_time_t(now);
    struct tm *ptm = ::localtime(&tt);
    char date[60] = {0};
    ::sprintf(date, "%d-%02d-%02d %02d:%02d:%02d.%03d",
              (int) ptm->tm_year + 1900, (int) ptm->tm_mon + 1, (int) ptm->tm_mday,
              (int) ptm->tm_hour, (int) ptm->tm_min, (int) ptm->tm_sec, (int) dis_millseconds);
    return move(string(date));
}

void JNICALL objectAlloc(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread,
                                jobject object, jclass object_klass, jlong size) {
    //给对象打tag，后续在objectFree()内可以通过该tag来判断是否成对出现释放
    tag += 1;
    jvmti_env->SetTag(object, tag);

    //获取线程信息
    jvmtiThreadInfo threadInfo;
    jvmti_env->GetThreadInfo(thread, &threadInfo);

    //获得 创建的对象的类签名
    char *classSignature;
    jvmti_env->GetClassSignature(object_klass, &classSignature, nullptr);

    if (mPackageName.empty() || findFilter(classSignature)) {
        char str[500];
        char *format = "%s:= object alloc (Thread:= %s Class:= %s Size:= %lld Tag:= %lld) \r\n";
        ::sprintf(str, format, GetCurrentSystemTime().c_str(), threadInfo.name, classSignature, size, tag);
        __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "%s", str);
    }

    jvmti_env->Deallocate((unsigned char *) classSignature);
}

void JNICALL methodEntry(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jmethodID method) {
    jclass clazz;
    char *signature;
    char *methodName;

    //获取方法对应的类
    jvmti_env->GetMethodDeclaringClass(method, &clazz);
    //获取类的签名
    jvmti_env->GetClassSignature(clazz, &signature, nullptr);
    //获取方法名字
    jvmti_env->GetMethodName(method, &methodName, nullptr, nullptr);

    if (mPackageName.empty() || findFilter(signature)) {
        char str[500];
        char *format = "%s:= methodEntry {%s  %s} \r\n";
        ::sprintf(str, format, GetCurrentSystemTime().c_str(), signature, methodName);
        __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "%s", str);
    }

    jvmti_env->Deallocate((unsigned char *) methodName);
    jvmti_env->Deallocate((unsigned char *) signature);
}


void Java_AttachInit(JNIEnv *env, jclass interface, jstring path) {

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.VMObjectAlloc = &objectAlloc;
    callbacks.MethodEntry = &methodEntry;

    __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "SetEventCallbacks");

    int error = mJvmtiEnv->SetEventCallbacks(&callbacks, sizeof(callbacks));
    __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "返回错误：= %d\n", error);

    mJvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_OBJECT_ALLOC, nullptr);
    mJvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY, nullptr);

    __android_log_print(ANDROID_LOG_ERROR, "LogLogLog", "attachInit is done");
}

void Java_AttachRelease(JNIEnv *env, jclass interface) {
    mJvmtiEnv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_VM_OBJECT_ALLOC, NULL);
    mJvmtiEnv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_METHOD_ENTRY, NULL);
}

void Java_AttachWFilters(JNIEnv *env, jclass interface, jstring packageName) {
    const char *_packageName = env->GetStringUTFChars(packageName, NULL);
    mPackageName = string(_packageName);
    env->ReleaseStringUTFChars(packageName, _packageName);
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



const JNINativeMethod gMethod[] = {
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
