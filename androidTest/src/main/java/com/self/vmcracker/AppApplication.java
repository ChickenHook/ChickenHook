package com.self.vmcracker;

import android.app.Application;
import android.content.Context;


public class AppApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
    }
}
