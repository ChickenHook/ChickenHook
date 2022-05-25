package org.chickenhook.demoapp;

import android.app.Service
import android.content.Context
import android.content.Intent
import android.os.*
import android.util.Log
import android.widget.Toast
import java.lang.RuntimeException
import java.util.*

const val MSG_SAY_HELLO = 0x123456a

class RemoteService : Service() {
    /**
     * Target we publish for clients to send messages to IncomingHandler.
     */
    private lateinit var mMessenger: Messenger


    companion object {
        init {
            NativeLib.installHooks()
        }
    }



    /**
     * Handler of incoming messages from clients.
     */
    internal class IncomingHandler(
        context: Context,
        private val applicationContext: Context = context.applicationContext
    ) : Handler() {
        override fun handleMessage(msg: Message) {
            when (msg.what) {
                MSG_SAY_HELLO -> {
                    val title = msg.data.getString("TEXT")
                    //Toast.makeText(applicationContext, title, Toast.LENGTH_SHORT).show()
                    Log.d("RemoteService", "Received message <$title>")
                }
                else -> super.handleMessage(msg)
            }
        }
    }

    /**
     * When binding to the service, we return an interface to our messenger
     * for sending messages to the service.
     */
    override fun onBind(intent: Intent): IBinder? {
        Toast.makeText(applicationContext, "binding", Toast.LENGTH_SHORT).show()
        mMessenger = Messenger(IncomingHandler(this))
        return mMessenger.binder
    }

}
