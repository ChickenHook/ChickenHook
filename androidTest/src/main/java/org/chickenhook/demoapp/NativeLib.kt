package org.chickenhook.demoapp

object NativeLib {
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    @JvmStatic
    external fun installHooks(): String

    @JvmStatic
    external fun doTransaction(
        target: String,
        code: Int,
        marshalledParcel: ByteArray,
        offets: ByteArray
    ): Int

    // Used to load the 'native-lib' library on application startup.
    init {
        System.loadLibrary("native-lib")
    }
}