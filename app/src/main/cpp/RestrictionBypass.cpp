//
// Created by ys on 2021/7/15.
//

#include <jni.h>
#include <android/log.h>
#include <thread>
#include <future>

JavaVM *_vm;

JNIEnv *attachCurrentThread(){
    JNIEnv *env;

    int res = _vm->AttachCurrentThread(&env, nullptr);
    __android_log_print(ANDROID_LOG_ERROR, "native", "Found attached %d", res);
    return env;
}

void detachCurrentThread(){
    _vm->DetachCurrentThread();
}

void printClassName(jobject obj, JNIEnv *env){
    jclass cls = env -> GetObjectClass(obj);
    //获取类对象
    jmethodID mid = env->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
    jobject clsObj = env->CallObjectMethod(obj, mid);
    //获取类描述符
    cls = env->GetObjectClass(clsObj);
    //找到类的getName()方法
    mid = env->GetMethodID(cls, "getName", "()Ljava/lang/String;");
    //通过getName获取jstring对象
    jstring strObj = static_cast<jstring>(env->CallObjectMethod(clsObj, mid));
    //通过jstring 获取C对象
    const char *str = env->GetStringUTFChars(strObj, NULL);
    __android_log_print(ANDROID_LOG_ERROR, "native", "Calling class is: %s", str);
    env->ReleaseStringUTFChars(strObj, str);
}

static jobject getDeclaredMethod_internal(
        jobject clazz,
        jstring method_name,
        jobjectArray params) {
    JNIEnv *env = attachCurrentThread();
    printClassName(clazz, env);
    jclass clazz_class = env->GetObjectClass(clazz);
    jmethodID get_declared_method_id = env->GetMethodID(clazz_class, "getDeclaredMethod",
                                                        "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");

    jobject res = env->CallObjectMethod(clazz, get_declared_method_id,
                                        method_name, params);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    jobject global_res = nullptr;
    if (res != nullptr) {
        global_res = env->NewGlobalRef(res);
    }
    detachCurrentThread();
    return global_res;
}

static jobject Java_getDeclaredMethod(
        JNIEnv *env,
        jclass interface,
        jobject clazz,
        jstring method_name,
        jobjectArray params) {
    auto global_clazz = env->NewGlobalRef(clazz);
    jstring global_method_name = (jstring) env->NewGlobalRef(method_name);
    int arg_length = env->GetArrayLength(params);
    jobjectArray global_params = nullptr;
    if (params != nullptr) {
        for (int i = 0; i < arg_length; i++) {
            jobject element = (jobject) env->GetObjectArrayElement(params, i);
            jobject global_element = env->NewGlobalRef(element);
            env->SetObjectArrayElement(params, i, global_element);
        }
        global_params = (jobjectArray) env->NewGlobalRef(params);
    }

    auto future = std::async(&getDeclaredMethod_internal, global_clazz,
                             global_method_name,
                             global_params);
    auto result = future.get();
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    return result;
}

static jobject getMethod_internal(
        jobject clazz,
        jstring method_name,
        jobjectArray params) {
    JNIEnv *env = attachCurrentThread();
    printClassName(clazz, env);
    jclass clazz_class = env->GetObjectClass(clazz);
    jmethodID get_declared_method_id = env->GetMethodID(clazz_class, "getDeclaredMethod",
                                                        "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");
    jobject res = env->CallObjectMethod(clazz, get_declared_method_id,
                                        method_name, params);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    jobject global_res = nullptr;
    if (res != nullptr) {
        global_res = env->NewGlobalRef(res);
    }

    detachCurrentThread();
    return global_res;
}

static jobject Java_getMethod(
        JNIEnv *env,
        jclass interface,
        jobject clazz,
        jstring method_name,
        jobjectArray params
        ){
    auto global_clazz = env->NewGlobalRef(clazz);
    jstring global_method_name = static_cast<jstring>(env->NewGlobalRef(method_name));
    int arg_length = env->GetArrayLength(params);
    jobjectArray global_params = nullptr;
    if (params != nullptr) {
        for (int i = 0; i < arg_length; i++) {
            jobject element = (jobject) env->GetObjectArrayElement(params, i);
            jobject global_element = env->NewGlobalRef(element);
            env->SetObjectArrayElement(params, i, global_element);
        }
        global_params = (jobjectArray) env->NewGlobalRef(params);
    }
    auto future = std::async(&getMethod_internal, global_clazz,
                             global_method_name,
                             global_params);
    auto result = future.get();
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    return result;
}

static jobject getDeclaredField_internal(
        jobject object,
        jstring field_name) {

    JNIEnv *env = attachCurrentThread();


    printClassName(object, env);
    jclass clazz_class = env->GetObjectClass(object);
    jmethodID methodId = env->GetMethodID(clazz_class, "getDeclaredField",
                                          "(Ljava/lang/String;)Ljava/lang/reflect/Field;");
    jobject res = env->CallObjectMethod(object, methodId, field_name);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    jobject global_res = nullptr;
    if (res != nullptr) {
        global_res = env->NewGlobalRef(res);
    }

    detachCurrentThread();
    return global_res;
}

static jobject Java_getDeclaredField(JNIEnv *env,
                                     jclass interface,
                                     jobject object,
                                     jstring field_name){
    auto global_object = env->NewGlobalRef(object);
    jstring global_field_name = (jstring) env->NewGlobalRef(field_name);
    auto future = std::async(&getDeclaredField_internal, global_object,
                             global_field_name);
    auto result = future.get();
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    return result;
}


static const JNINativeMethod gMethod[] = {
        {"getDeclaredMethod", "(Ljava/lang/Object;Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;", (void *) Java_getDeclaredMethod},
        {"getMethod", "(Ljava/lang/Object;Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;", (void *) Java_getMethod},
        {"getDeclaredField", "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/reflect/Field;", (void *) Java_getDeclaredField}
};

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved){
    _vm = vm;
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) & env, JNI_VERSION_1_4) != JNI_OK){
        return -1;
    }

    jclass clazz = env->FindClass("com/example/baa/NativeRestrictionBypass");
    if (!clazz){
        return -1;
    }

    if (env->RegisterNatives(clazz, gMethod, sizeof(gMethod) / sizeof(gMethod[0]))){
        return -1;
    }
    return JNI_VERSION_1_4;
}

