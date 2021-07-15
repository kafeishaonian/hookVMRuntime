package com.example.baa;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.example.baa.databinding.ActivityMainBinding;

import java.lang.reflect.Method;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

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
            hook();
        });
    }


    public void hook(){
        try {
            Class work = Class.forName("android.app.QueuedWork");
            Method[] methods = work.getMethods();
            Method[] methods1 = work.getDeclaredMethods();

            for (int i = 0; i < methods.length; i++){
                Log.e("TAG", "hook: ======" + methods[i]);
            }

            Log.e("TAG", "===================================");

            for (int i = 0; i < methods1.length; i++){
                Log.e("TAG", "hook: ======" + methods1[i]);
            }

//            Method workCurrent = work.getMethod("waitToFinish", null);
//            Constructor constructor = work.getConstructor();
//            Object object = constructor.newInstance();
        } catch (Exception e){

        }
    }


}