package com.chickenhook.vmcracker

import android.os.SystemClock
import androidx.test.espresso.Espresso
import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.action.ViewActions.click
import androidx.test.espresso.assertion.ViewAssertions.matches
import androidx.test.espresso.matcher.ViewMatchers.withId
import androidx.test.espresso.matcher.ViewMatchers.withText
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.rule.ActivityTestRule
import org.chickenhook.demoapp.MainActivity
import org.chickenhook.demoapp.R

import org.junit.Test
import org.junit.runner.RunWith
import org.junit.Rule

/**
 * Instrumented test, which will execute on an Android device.
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
@RunWith(AndroidJUnit4::class)
class RegressionTest {
    @get:Rule
    var mActivityRule: ActivityTestRule<MainActivity> = ActivityTestRule(
        MainActivity::class.java)

    @Test
    fun startMainActivity() {
        onView(withId(R.id.sample_text)).check(matches(withText("Hello from our Hook ;)")))
    }

    @Test
    fun openWebView() {
        onView(withId(R.id.test_button)).perform(click())
        SystemClock.sleep(1000)
        Espresso.pressBack();

    }
}
