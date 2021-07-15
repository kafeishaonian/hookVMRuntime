package com.example.baa;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class NativeRestrictionBypass {

    static {
        System.loadLibrary("native-lib");
    }

    public static native Method getDeclaredMethod(Object recv, String name, Class<?>[] parameterTypes);
    public static native Method getMethod(Object recv, String name, Class<?>[] parameterTypes);
    public static native Field getDeclaredField(Object recv, String name);
}
