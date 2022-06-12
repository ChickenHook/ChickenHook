package org.chickenhook.demoapp

import android.annotation.SuppressLint
import android.app.Activity
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.content.pm.PackageManager
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*
import android.os.*
import android.security.keystore.KeyGenParameterSpec
import android.security.keystore.KeyProperties
import android.view.View
import androidx.annotation.RequiresApi
import java.io.File
import java.lang.RuntimeException
import java.security.KeyStore
import java.security.MessageDigest
import javax.crypto.Cipher
import javax.crypto.KeyGenerator
import javax.crypto.SecretKey
import javax.crypto.spec.IvParameterSpec


class MainActivity : Activity() {


    fun readFile(file: File): ByteArray {
        return file.readBytes()
    }
    fun TriggerPackageManagerCrash() {
        val view = findViewById<View>(android.R.id.content)
        Log.d("TriggerClipboardCrash", "TriggerClipboardCrash View <$view>")
        val ActivityThreadClass = Class.forName("android.app.ActivityThread")
        val GetPackageManagerMethod = ActivityThreadClass.getDeclaredMethod("getPackageManager")
        GetPackageManagerMethod.isAccessible = true
        val IPackageManagerProxy = GetPackageManagerMethod.invoke(null)

        val mRemoteField = IPackageManagerProxy::class.java.getDeclaredField("mRemote")
        mRemoteField.isAccessible = true
        val mRemote = mRemoteField.get(IPackageManagerProxy) as IBinder

        val data = Parcel.obtain();
        data.writeInterfaceToken("android.content.pm.IPackageManager")
//        data.writeLong(0)
//        data.writeLong(0)
//
//        val bundle = Bundle()
//        bundle.putString("TEXT", "From SomeTest")
//        data.writeBundle(bundle);
        val l1 = intArrayOf(
            0xff00000, // 0x73622a85
            0x00000113,
            0xff00000,
            0xff00000,
            0xff00000,
            0xff00000,
            0xff00000,
            -1,
            -1
        )
        l1.forEach {
            data.writeInt(it)
        }

        val reply = Parcel.obtain()

        val res = mRemote.transact(37, data, reply, 17)



        SystemClock.sleep(2000)
    }
    fun TriggerClipboardCrash() {
        val view = findViewById<View>(android.R.id.content)
        Log.d("TriggerClipboardCrash", "TriggerClipboardCrash View <$view>")
        val mAttachInfoField = View::class.java.getDeclaredField("mAttachInfo")
        mAttachInfoField.isAccessible = true
        val attachInfo = mAttachInfoField.get(view)

        val mWindowSessionField = attachInfo::class.java.getDeclaredField("mSession")
        mWindowSessionField.isAccessible = true
        val mWindowSession = mWindowSessionField.get(attachInfo)

        val mRemoteField = mWindowSession::class.java.getDeclaredField("mRemote")
        mRemoteField.isAccessible = true
        val mWindowSessionBinder = mRemoteField.get(mWindowSession) as IBinder

        val data = Parcel.obtain();
        data.writeInterfaceToken("android.view.IWindowSession")
        data.writeLong(0)
        data.writeLong(0)

        val bundle = Bundle()
        bundle.putString("TEXT", "From SomeTest")
        data.writeBundle(bundle);
        val l1 = intArrayOf(
            0xff00000, // 0x73622a85
            0x00000113,
            0xff00000,
            0xff00000,
            0xff00000,
            0xff00000,
            0xff00000,
            -1,
            -1
        )
        l1.forEach {
            data.writeInt(it)
        }

        val reply = Parcel.obtain()
        SystemClock.sleep(2000)
//        for(i in 0..30) {
//            Log.d("MainActivity", "TriggerClipboardCrash [+] it <$i>")
//            val res = mWindowSessionBinder.transact(i, data, reply, 17)
//            Log.d("MainActivity", "TriggerClipboardCrash [+] res <$res>")
//        }

        val res = mWindowSessionBinder.transact(13, data, reply, 17)
        Log.d("MainActivity", "TriggerClipboardCrash [+] res <$res>")


        SystemClock.sleep(2000)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        test_button?.setOnClickListener {
            /*Log.d("TEST", "This is a test callback")
            startActivity(Intent(this, WebViewActivity::class.java))*/
            Thread() {
                //SystemClock.sleep(5000)
                //encryptTest()
                //Log.d("MainActivity", "BINDER TEST <"+mService.getString()+">")
                //sayHello("From Button");
                SomeTest()
            }.start();
            true
        }
        clipboard_crash?.setOnClickListener {
            //TriggerClipboardCrash()
            TriggerPackageManagerCrash()
        }


//        Looper.getMainLooper().setMessageLogging { string ->
//            // TODO try to use native hook to create java hooks and hook this class!
//            Log.d("LOOPER HOOK", string)
//        }

        digest("HALLO")
        sample_text?.text = NativeLib.installHooks()
//        digest("HALLO")
//        readFile(File("/proc/self/maps"))

        //SomeTest();
        Thread() {
            sayHello("From Startup");
        }.start()
    }

    @SuppressLint("WrongConstant")
    fun SomeTest() {
//        val p=Parcel.obtain();
//        p.writeStrongBinder()
//        p.marshall();

        // resizeTask
        // removeTask InvocationTargetException
//        val atm = this.getSystemService("activity_task")
//
//        val i = Intent()
//        i.setClassName("com.android.keychain","com.android.keychain.KeyChainActivity")
//        startActivity(i)
//        val getTasksMethod = atm::class.java.getMethod("removeTask", Integer.TYPE)
//        getTasksMethod.isAccessible = true
//        val tasks = getTasksMethod.invoke(atm, 101)
//        Log.d("MainActivity", "SomeTest [+] stacks = <$tasks>")
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            Thread() {
                SystemClock.sleep(5000)
//                val atm = this.getSystemService("activity_task")
//                val getTasksMethod = atm::class.java.getMethod("removeTask", Integer.TYPE)
//                getTasksMethod.isAccessible = true
//                val tasks = getTasksMethod.invoke(atm, 101)
//                Log.d("MainActivity", "SomeTest [+] stacks = <$tasks>")

                //encryptTest()
//                Log.d("MainActivity", "BINDER TEST <"+mService.getString()+">")
                for(i in 0..440) {
                    sayHello("From SomeTest");
                }

            }.start();
        }
    }


    fun ByteArray.toHexString() = joinToString("") { "%02x".format(it) }

    fun digest(str: String) {

        val messageDigest = MessageDigest.getInstance("SHA-256") as MessageDigest
        val encodedhash = messageDigest.digest(
            str.toByteArray()
        )
        Log.d("MainActivity", "got digest ${encodedhash.toHexString()}")
    }









    @RequiresApi(Build.VERSION_CODES.M)
    fun encryptTest() {
        val keyGenerator = KeyGenerator.getInstance(KeyProperties.KEY_ALGORITHM_AES, "AndroidKeyStore")
        val keyGenParameterSpec = KeyGenParameterSpec.Builder("MyKeyAlias",
            KeyProperties.PURPOSE_ENCRYPT or KeyProperties.PURPOSE_DECRYPT)
            .setBlockModes(KeyProperties.BLOCK_MODE_CBC)
            .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_NONE)
            .build()

        keyGenerator.init(keyGenParameterSpec)
        keyGenerator.generateKey()

        val pair = encryptData("Test this encryption")

        val decryptedData = decryptData(pair.first, pair.second)

        val encrypted = pair.second.toString(Charsets.UTF_8)
        println("Encrypted data: $encrypted")
        println("Decrypted data: $decryptedData")
    }

    fun decryptData(ivBytes: ByteArray, data: ByteArray): String{
        val cipher = Cipher.getInstance("AES/CBC/NoPadding")
        val spec = IvParameterSpec(ivBytes)

        cipher.init(Cipher.DECRYPT_MODE, getKey(), spec)
        return cipher.doFinal(data).toString(Charsets.UTF_8).trim()
    }

    fun getKey(): SecretKey {
        val keystore = KeyStore.getInstance("AndroidKeyStore")
        keystore.load(null)

        val secretKeyEntry = keystore.getEntry("MyKeyAlias", null) as KeyStore.SecretKeyEntry
        return secretKeyEntry.secretKey
    }

    fun encryptData(data: String): Pair<ByteArray, ByteArray> {
        val cipher = Cipher.getInstance("AES/CBC/NoPadding")

        var temp = data
        while (temp.toByteArray().size % 16 != 0)
            temp += "\u0020"

        cipher.init(Cipher.ENCRYPT_MODE, getKey())

        val ivBytes = cipher.iv
        val encryptedBytes = cipher.doFinal(temp.toByteArray(Charsets.UTF_8))

        return Pair(ivBytes, encryptedBytes)
    }

    private var mService: Messenger? = null
    private var mCustomBinder: MyBinderProxy? = null

    /** Flag indicating whether we have called bind on the service.  */
    private var bound: Boolean = false



    private val connection = object : ServiceConnection {

        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
//            mService = Messenger(service)
            mCustomBinder = MyBinderProxy(service)
            bound = true

        }

        override fun onServiceDisconnected(arg0: ComponentName) {
            mService = null
            bound = false
        }
    }

    fun sayHello(text : String) {
        SystemClock.sleep(2000)
        if (!bound) return
        // Create and send a message to the service, using a supported 'what' value
//        val msg: Message = Message.obtain(null, MSG_SAY_HELLO, 0, 0)
//        val data = Bundle()
//        data.putString("TEXT", text)
//        msg.data = data
//        try {
//            mService?.send(msg)
//        } catch (e: RemoteException) {
//            e.printStackTrace()
//        }

        mCustomBinder?.sendString(text)
    }


    override fun onStart() {
        super.onStart()
        // Bind to LocalService
        Intent(this, RemoteService::class.java).also { intent ->
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }
    }

}
