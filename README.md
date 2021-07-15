# hookVMRuntime
JNI Native崩溃日志分析

在build/intermediates/cmake/debug/obj/arm-xxx目录下创建crash.log

将LogCat的日志copy到crash.log

cd到build\intermediates\cmake\debug目录

然后执行ndk-stack -sym obj/armeabi-v7a -dump obj/armeabi-v7a/crash.log命令
