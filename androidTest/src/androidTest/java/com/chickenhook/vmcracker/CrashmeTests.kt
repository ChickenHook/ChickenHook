package com.chickenhook.vmcracker

import android.R.attr
import android.os.Parcel
import android.os.SystemClock
import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.assertion.ViewAssertions.matches
import androidx.test.espresso.matcher.ViewMatchers.withId
import androidx.test.espresso.matcher.ViewMatchers.withText
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.rule.ActivityTestRule
import org.chickenhook.demoapp.MainActivity
import org.chickenhook.demoapp.NativeLib
import org.chickenhook.demoapp.R
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import android.R.attr.data
import android.util.Log
import androidx.test.espresso.Espresso
import androidx.test.espresso.action.ViewActions
import org.junit.Ignore
import java.nio.ByteBuffer
import java.nio.IntBuffer


/**
 * Instrumented test, which will execute on an Android device.
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
@RunWith(AndroidJUnit4::class)
class CrashmeTests {
    @get:Rule
    var mActivityRule: ActivityTestRule<MainActivity> = ActivityTestRule(
        MainActivity::class.java
    )

    @Test
    fun crashIt() {
        SystemClock.sleep(2000)
        onView(withId(R.id.test_button)).perform(ViewActions.click())
        SystemClock.sleep(15000)
        Espresso.pressBack();
    }


}
