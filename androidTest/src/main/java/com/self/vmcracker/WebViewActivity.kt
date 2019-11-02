package com.self.vmcracker

import android.app.Activity
import android.os.Bundle
import android.webkit.WebView

class WebViewActivity : Activity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_web_view)

        findViewById<WebView>(R.id.webview).let {
            it.loadUrl("https://webtest.labnet.kobil.com/webview-test/kobilSpecialCommands.html")
        }
    }

}
