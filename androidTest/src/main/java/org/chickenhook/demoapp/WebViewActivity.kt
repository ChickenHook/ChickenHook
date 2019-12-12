package org.chickenhook.demoapp

import android.app.Activity
import android.os.Bundle
import android.webkit.WebView
import org.chickenhook.demoapp.R

class WebViewActivity : Activity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_web_view)

        findViewById<WebView>(R.id.chicken_web_view).let {
            it.loadUrl("https://www.google.de")
        }
    }

}
