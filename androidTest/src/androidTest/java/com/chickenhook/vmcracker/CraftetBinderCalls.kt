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
import android.content.Context
import android.util.Log
import android.view.WindowManager
import androidx.test.espresso.action.ViewActions
import androidx.test.platform.app.InstrumentationRegistry
import java.nio.ByteBuffer
import java.nio.IntBuffer


/**
 * Instrumented test, which will execute on an Android device.
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
@RunWith(AndroidJUnit4::class)
class CraftetBinderCalls {
    @get:Rule
    var mActivityRule: ActivityTestRule<MainActivity> = ActivityTestRule(
        MainActivity::class.java
    )

    @Test
    fun startMainActivity() {
        //onView(withId(R.id.sample_text)).check(matches(withText("Hello from our Hook ;)")))
        SystemClock.sleep(5000)

        val target = "android.app.IActivityManager";
        val byteBuffer: ByteBuffer = ByteBuffer.allocate(512)
        val intBuffer: IntBuffer = byteBuffer.asIntBuffer()
        val p = Parcel.obtain()
        intBuffer.put(0)
        p.writeString(target)

        //intBuffer.put(p.dataPosition())

        NativeLib.doTransaction(target, 7, p.marshall(), byteBuffer.array())

        SystemClock.sleep(2000)

    }
    @Test
    fun startIMessenger() {
        //onView(withId(R.id.sample_text)).check(matches(withText("Hello from our Hook ;)")))
        SystemClock.sleep(5000)

        val target = "android.media.IAudioService";
        val byteBuffer: ByteBuffer = ByteBuffer.allocate(512)
        val intBuffer: IntBuffer = byteBuffer.asIntBuffer()
        val p = Parcel.obtain()
        intBuffer.put(0)
        p.writeInterfaceToken(target)

        intBuffer.put(p.dataPosition())
        p.writeString("Crafted Binder object!!")
        Log.d("CraftedBinderCalls", "Call doTransaction")
        NativeLib.doTransaction(target, 1, p.marshall(), byteBuffer.array())
        onView(withId(R.id.test_button)).perform(ViewActions.click())

        SystemClock.sleep(2000)

    }
    @Test
    fun crashWindowSession() {

        onView(withId(R.id.clipboard_crash)).perform(ViewActions.click())
        SystemClock.sleep(2000)

    }
}
