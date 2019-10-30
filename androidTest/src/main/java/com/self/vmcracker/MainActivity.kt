package com.self.vmcracker

import android.app.Activity
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*
import android.os.*
import java.io.File
import java.security.MessageDigest


class MainActivity : Activity() {


    fun readFile(file: File): ByteArray {
        return file.readBytes()
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        test_button?.setOnClickListener {
            Log.d("TEST", "This is a test callback")
            true
        }
        sample_text?.text = "" + "test"


        Looper.getMainLooper().setMessageLogging { string -> // TODO try to use native hook to create java hooks and hook this class!
            Log.d("LOOPER HOOK", string)
        }

        digest("HALLO")
        installHooks()
        digest("HALLO")
        readFile(File("/proc/self/maps"))
    }


    fun ByteArray.toHexString() = joinToString("") { "%02x".format(it) }

    fun digest(str: String) {

        val messageDigest = MessageDigest.getInstance("SHA-256") as MessageDigest
        val encodedhash = messageDigest.digest(
            str.toByteArray()
        )
        Log.d("MainActivity", "got digest ${encodedhash.toHexString()}")
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun installHooks(): String

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
