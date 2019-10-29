package com.self.vmcracker

import android.accessibilityservice.AccessibilityService
import android.accessibilityservice.AccessibilityServiceInfo
import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.pm.ServiceInfo
import android.hardware.Sensor
import android.hardware.SensorManager
import android.hardware.input.InputManager
import androidx.appcompat.app.AppCompatActivity
import android.util.Log
import android.widget.Button
import kotlinx.android.synthetic.main.activity_main.*
import java.io.BufferedReader
import java.io.FileReader
import kotlin.concurrent.thread
import com.self.vmcracker.hook.ActivityHooker
import com.swift.sandhook.SandHook
import androidx.core.app.ComponentActivity
import androidx.core.app.ComponentActivity.ExtraData
import androidx.core.content.ContextCompat.getSystemService
import android.icu.lang.UCharacter.GraphemeClusterBreak.T
import android.os.*
import android.view.accessibility.AccessibilityEvent
import android.view.accessibility.AccessibilityManager
import androidx.core.view.accessibility.AccessibilityManagerCompat
import dalvik.system.DexClassLoader
import java.io.File
import java.io.FileDescriptor
import java.lang.Exception
import java.lang.reflect.Field
import java.nio.charset.StandardCharsets
import java.security.MessageDigest
import kotlin.experimental.and


class MainActivity : Activity() {


    fun readFile(file: File): ByteArray {
        return file.readBytes()
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val button_test = findViewById<Button>(R.id.test_button)
        button_test.setOnClickListener {
            Log.d("TEST", "This is a test callback")
            Log.d("TEST", "This is a test callback2")

            true
        }
        val sensorManager = this.getSystemService(Context.SENSOR_SERVICE) as SensorManager
        val sensor: Sensor


        /*val vmRuntime = Class.forName("dalvik.system.VMRuntime")
        val address = vmRuntime.getDeclaredMethod("addressOf", Object::class.java)
        val runtime = vmRuntime.getDeclaredMethod("getRuntime")
        val newNonMovableArray =
            vmRuntime.getDeclaredMethod("newNonMovableArray", Class::class.java, Int::class.java)
        address.isAccessible = true
        val newMovableArray = newNonMovableArray.invoke(runtime.invoke(null), Test::class.java, 1)
        (newMovableArray as Array<Test>)[0] = Test()
        val addr = address.invoke(runtime.invoke(null), newMovableArray) as Long*/
        sample_text.text = "" + "test"
        //stringFromJNI(addr)
        // /proc/28290/fd

        Looper.getMainLooper().setMessageLogging { string ->
            Log.d("LOOPER HOOK", string)
        }
        Log.d("Accessibiilty watcher", "retrieve services")
        //logAccServices()


        //startService(Intent(this, MyAcc::class.java))
        //logAccServices()

        digest("HALLO")
        stringFromJNI(0)
        digest("HALLO")
        readFile(File("/proc/self/maps"))
    }


    fun ByteArray.toHexString() = joinToString("") { "%02x".format(it) }

    fun digest(str: String) {

        val messageDigest = MessageDigest.getInstance("SHA-256") as MessageDigest
        val encodedhash = messageDigest.digest(
            str.toByteArray()
        )
        Log.d("DIGEST", "got digest ${encodedhash.toHexString()}")
    }

    fun logAccServices() {
        AccessibilityManagerCompat.getInstalledAccessibilityServiceList(
            this.getSystemService(
                Context.ACCESSIBILITY_SERVICE
            ) as AccessibilityManager?
        ).forEach {
            Log.d("Accessibiilty watcher", it.settingsActivityName)
            it.packageNames.forEach {
                Log.d("Accessibility watcher", "package: $it")
            }
            Log.d("Accessibility matcher", "package: " + it.resolveInfo.resolvePackageName)
        }
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(address: Long): String

    //external fun addressOf(obj: Object): Long

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }


    fun createNonMovableArray() {

    }

    class Test {
        val testValue = 13443
        val testString = "Hallo Freunde"
    }


    class MyAcc : AccessibilityService() {
        override fun onInterrupt() {

        }

        override fun onAccessibilityEvent(event: AccessibilityEvent?) {

        }

        override fun onServiceConnected() {
            val info = AccessibilityServiceInfo()
            info.apply {
                // Set the type of events that this service wants to listen to. Others
                // won't be passed to this service.
                eventTypes =
                    AccessibilityEvent.TYPE_VIEW_CLICKED or AccessibilityEvent.TYPE_VIEW_FOCUSED

                // If you only want this service to work with specific applications, set their
                // package names here. Otherwise, when the service is activated, it will listen
                // to events from all applications.
                packageNames =
                    arrayOf("com.example.android.myFirstApp", "com.example.android.mySecondApp")

                // Set the type of feedback your service will provide.
                feedbackType = AccessibilityServiceInfo.FEEDBACK_SPOKEN

                // Default services are invoked only if no package-specific ones are present
                // for the type of AccessibilityEvent generated. This service *is*
                // application-specific, so the flag isn't necessary. If this was a
                // general-purpose service, it would be worth considering setting the
                // DEFAULT flag.

                // flags = AccessibilityServiceInfo.DEFAULT;

                notificationTimeout = 100
            }

            this.serviceInfo = info
        }

    }
}
