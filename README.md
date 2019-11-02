|   | Build & Test |
|---|:-----:|
|![Win](docs/res/win_med.png) **Windows**|[![Build & Test][win-build-badge]][win-build]| 
|![macOS](docs/res/apple_med.png) **macOS**|[![Build & Test][macOS-build-badge]][macOS-build]| 
|![Linux](docs/res/linux_med.png) **Linux**|[![Build & Test][linux-build-badge]][linux-build]|

[win-build-badge]: https://dev.azure.com/sascharoth/sascharoth/_apis/build/status/ChickenHook.chickenhook
[win-build]:https://dev.azure.com/sascharoth/sascharoth/_build/latest?definitionId=1

[macOS-build-badge]: https://dev.azure.com/sascharoth/sascharoth/_apis/build/status/ChickenHook.chickenhook
[macOS-build]: https://dev.azure.com/sascharoth/sascharoth/_build/latest?definitionId=1

[linux-build-badge]: https://dev.azure.com/sascharoth/sascharoth/_apis/build/status/ChickenHook.chickenhook
[linux-build]: https://dev.azure.com/sascharoth/sascharoth/_build/latest?definitionId=1

# ChickenHook 
<img src="./logo.png" alt="ChickenHook logo" height="200" width="200" />

## General

Supported architectures: x86, arm64, x86_64 (experimental)
Supported platforms: Android, Linux
## Usage

1. Create the hook function (the function that should be called instead of the original function)

example here shows a hook function for libc's open

```c
    int my_open(const char *__path, int __flags, ...) {
        // this my_open will be called instead of doIt

        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "open called [-] %s", __path);
        // yeah we're inside! But sometimes you want to call the original function also.
        // For this purpose we try to retrieve the corresponding trampoline.
        int res = -1;
        Trampoline trampoline;
        if (chickenHook.getTrampolineByAddr((void *) &open, trampoline)) {
            __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");
            // Now we copy the original function code into the original function
            trampoline.copyOriginal();
            // We call the function
            res = open(__path, __flags);
            // afterwards we install our trampoline again
            trampolinecode;
            // that's it!
            return res;
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
        }
        return -1;
    }
```

2. Inject the trampoline  (enable the hook)

```c
    ChickenHook::getInstance().hook((void *) &open, (void *) &my_open);
```


## Build

Linux ant and Android gradle...

### Linux
```
ant configure-linux compile-linux
```
artifacts will be in build/libs/

### Android
Use as an Android Studio project or:
```
gradle assemble
```
