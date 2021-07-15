package com.example.baa;

import android.app.Application;
import android.content.Context;
import android.util.Log;

public class MyApplication extends Application {

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        try {
            UnsealUtils.unseal();
        } catch (Exception e) {
            Log.e("BypassProvider", "Unable to unseal hidden api access", e);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }
}
