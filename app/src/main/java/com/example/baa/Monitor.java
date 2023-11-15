package com.example.baa;

import android.content.Context;
import android.os.Build;
import android.os.Debug;
import android.util.Log;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * author: hongming.wei
 * data: 2023/11/9
 */
public class Monitor {
    public static void init(Context context) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                File libDir = new File(context.getFilesDir(), "lib");
                if (!libDir.exists()) {
                    libDir.mkdirs();
                }

                File libSo = new File(libDir, "libmemorymonitor.so");
                if (libSo.exists()) {
                    libSo.delete();
                }

                Method findLibrary = ClassLoader.class.getDeclaredMethod("findLibrary", String.class);
                String libFilePath = (String) findLibrary.invoke(context.getClassLoader(), "memorymonitor");
                int isCopy = NativeRestrictionBypass.SoFileCopy(libFilePath, libSo.getAbsolutePath());
                if (isCopy == 1) {
                    System.load(libSo.getAbsolutePath());
                    hookVMDebug(libSo.getAbsolutePath(), context);
                }
                attachInit(libSo.getAbsolutePath());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    private static void hookVMDebug(String agentPath, Context context) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                Debug.attachJvmtiAgent(agentPath, null, context.getClassLoader());
            } else {
                //android 9.0以下版本使用反射方式加载
                Class vmDebugClazz = Class.forName("dalvik.system.VMDebug");
                Method attachAgentMethod = vmDebugClazz.getMethod("attachAgent", String.class, ClassLoader.class);
                attachAgentMethod.setAccessible(true);
                attachAgentMethod.invoke(null, agentPath, null);
            }
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            Throwable t = e.getTargetException();
            t.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    public static native void attachInit(String path);
    public static native void attachRelease();

    public static native void attachWFilters(String packagename);
}
