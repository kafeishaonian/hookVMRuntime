package com.example.baa;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.TextView;

import com.example.baa.databinding.ActivityMainBinding;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText("哈哈哈哈哈");

        tv.setOnClickListener(v -> {
//            hookActivityThread();
//            hookQueuedWork1();

            new ByteTest();
        });
    }

    public void hookActivityThread(){
        try {
            Class cThread = Class.forName("android.app.ActivityThread");
            Field currentThread = cThread.getDeclaredField("sCurrentActivityThread");
            currentThread.setAccessible(true);
            Object currThread = currentThread.get(null);
            Field field = cThread.getDeclaredField("mH");
            field.setAccessible(true);
            Handler handler = (Handler) field.get(currThread);
            if (handler != null) {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        hookQueuedWork();
                    }
                });
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void hookQueuedWork(){
        try {
            Class work = Class.forName("android.app.QueuedWork");
            Method workCurrent = work.getMethod("waitToFinish", null);
            Constructor constructor = work.getConstructor();
            Object object = constructor.newInstance();
            workCurrent.invoke(object, new Class[]{});
        } catch (Exception e){

        }
    }

    public void hookQueuedWork1(){
        try {
            Method getRuntime = RestrictionBypass.getMethod(
                    Class.forName("android.app.QueuedWork"),
                    "waitToFinish"
            );
            getRuntime.setAccessible(true);
            getRuntime.invoke(null);
        } catch (Exception e){
            e.printStackTrace();
        }
    }


}