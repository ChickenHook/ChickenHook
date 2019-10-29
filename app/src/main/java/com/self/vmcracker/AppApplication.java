package com.self.vmcracker;

import android.app.ActivityManager;
import android.app.Application;
import android.app.KeyguardManager;
import android.app.admin.SystemUpdateInfo;
import android.app.slice.SliceManager;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.pm.PermissionInfo;
import android.hardware.biometrics.BiometricPrompt;
import android.hardware.input.InputManager;
import android.os.Build;
import android.os.HardwarePropertiesManager;
import android.os.health.HealthStats;
import android.os.health.PackageHealthStats;
import android.os.health.SystemHealthManager;
import android.util.Log;
import android.view.WindowManager;
import android.view.accessibility.AccessibilityManager;
import android.view.inputmethod.InputMethodManager;

import com.self.vmcracker.hook.ActivityHooker;
import com.swift.sandhook.SandHook;
import com.swift.sandhook.wrapper.HookErrorException;

import static android.Manifest.permission.BIND_ACCESSIBILITY_SERVICE;
import static android.content.pm.PermissionInfo.PROTECTION_SIGNATURE_OR_SYSTEM;

public class AppApplication extends Application {

    static {
/*        try {
            SandHook.passApiCheck();
            SandHook.disableVMInline();
            SandHook.skipAllSafeCheck(true);
            SandHook.addHookClass(
                    ActivityHooker.class
                );
        } catch (Exception e) {
            e.printStackTrace();
        }*/

    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        /*PermissionInfo pi = new PermissionInfo();
        pi.name = "android.permission.BIND_ACCESSIBILITY_SERVICE";
        pi.nonLocalizedLabel = "ACCESSIBILITY";
        pi.protectionLevel = PermissionInfo.PROTECTION_NORMAL;
        base.getPackageManager().addPermission(pi);
        SecurityManager s;
        AccessibilityManager a;
        SystemUpdateInfo s2;
        PackageHealthStats s3;
        PackageManager pa;*/
        /*ActivityManager ac = (ActivityManager) base.getSystemService(ACTIVITY_SERVICE);
        for (ActivityManager.RunningAppProcessInfo o : ac.getRunningAppProcesses()) {
            Log.d("RUNNING APP LISTENER", "found" + o.processName);
            for (String p : o.pkgList) {
                Log.d("RUNNING APP LISTENER", "sub package" + p);

            }

        }
        
        //ac.clearApplicationUserData();
        KeyguardManager k = (KeyguardManager) base.getSystemService(KEYGUARD_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            k.createConfirmDeviceCredentialIntent("Malicious behaviour detected", "KOBIL detected an attack, please unlock your device to continue");
        }*/



    }
}
