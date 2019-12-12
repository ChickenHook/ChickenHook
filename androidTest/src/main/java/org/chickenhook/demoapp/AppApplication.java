package org.chickenhook.demoapp;

import android.app.ActivityManager;
import android.app.Application;
import android.content.Context;
import android.os.Build;
import android.webkit.WebView;


public class AppApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
        {
            try
            {
                WebView.setDataDirectorySuffix(Application.getProcessName());
            }
            catch (Exception ignored)
            {

            }
        }
    }

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
    }
}
