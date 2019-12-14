|   | Build & Test |
|---|:-----:|
|![macOS](docs/res/apple_med.png) **macOS**|[![Build & Test][macOS-build-badge]][macOS-build]| 
|![Linux](docs/res/linux_med.png) **Linux**|[![Build & Test][linux-build-badge]][linux-build]|

<!--- [Win](docs/res/win_med.png) **Windows**|[![Build & Test][win-build-badge]][win-build] --->

<!--- [win-build-badge]: https://dev.azure.com/sascharoth/sascharoth/_apis/build/status/ChickenHook.chickenhook --->
<!--- [win-build]: https://dev.azure.com/ChickenHook/ChickenHook/_build?definitionId=1 --->

[macOS-build-badge]: https://dev.azure.com/sascharoth/sascharoth/_apis/build/status/ChickenHook.chickenhook
[macOS-build]: https://dev.azure.com/ChickenHook/ChickenHook/_build?definitionId=1

[linux-build-badge]: https://dev.azure.com/sascharoth/sascharoth/_apis/build/status/ChickenHook.chickenhook
[linux-build]: https://dev.azure.com/ChickenHook/ChickenHook/_build?definitionId=1

# ChickenHook 
<img src="./logo.png" alt="ChickenHook logo" height="200" width="200" />

## General

ChickenHook is a multi architecture hooking framework.

Supported architectures: x86, arm64, x86_64 (experimental)
Supported platforms: Android, Linux

### Example usage

#### Linux

Hack some applications using ChickenHook + StaticInjector (Linux Wrapper)

See more at: [StaticInjector](https://github.com/ChickenHook/StaticInjector)

Here are some examples hacks using StaticInjector

##### Firefox

Check this video (Please enable subtitles):

[![](http://img.youtube.com/vi/_4K2d7FFHqo/0.jpg)](http://www.youtube.com/watch?v=_4K2d7FFHqo "Linux attack Firefox")

##### Skype


Check this video (Please enable subtitles):

[![](doc/skypeAttack.gif)](http://www.youtube.com/watch?v=kbrenIx8OrI "How to hack Skype on Linux")

http://img.youtube.com/vi/kbrenIx8OrI/0.jpg

Read more in our wiki:
[How to create a linux attack (skype example)](https://github.com/ChickenHook/StaticInjector/wiki/How-to-create-a-linux-attack-(step-by-step-guide) "How to create a linux attack (step by step guide)")

#### Android

Hook AndroidRuntime (ART)

See more at: [ChickenTime](https://github.com/ChickenHook/AndroidChickenTime)

## Requirements

* ant

### Linux and MacOS
* cmake
* make

### Android
* Android SDK
* Android NDK 
* Android Studio (Optional)

## Usage

1. Create the hook function (the function that should be called instead of the original function)

example here shows a hook function for libc's open

```c
ssize_t my_read(int __fd, void *__buf, size_t __count) {
    __android_log_print(ANDROID_LOG_DEBUG, "my_read", "read called [-] %d", __fd);

    // <== add your code before real call here

    // yeah we're inside! But sometimes you want to call the original function also.
    // For this purpose we try to retrieve the corresponding trampoline.
    // So let's retrieve our trampoline in order to call the original function "read"
    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &read, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "my_read",
                            "hooked function call original function");
        printLines(hexdump(static_cast<const uint8_t *>(__buf), __count, "read"));

        // retrieve the real read call address
        ssize_t (*_read)(int, void *, size_t) =(ssize_t (*)(int, void *,
                                                            size_t)) trampoline.getRealCallAddr();
        // if read != nullptr we have a valid address and call it
        // if read ==nullptr we have to copy the original code of read.
        if (_read == nullptr) {
            // !! WARNING !! This is a very risky workaround.
            // * Race condition can lead to crashes
            // * Multithreading and semaphores in target function or it's callee's can lead to deadlocks
            trampoline.copyOriginal();
            res = read(__fd, __buf, __count);
            trampoline.reinstall();
        } else {
            // Very save method. Available for most of all functions
            res = _read(__fd, __buf, __count);
        }
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "my_read",
                            "hooked function cannot call original function");
    }

    // <== manipulate results here

    return res;
}
```

2. Inject the trampoline  (enable the hook)

```c
    ChickenHook::Hooking::getInstance().hook((void *) &read, (void *) &my_read);
```


## Build

Currently ChickenHook can be build for Linux and Android and MacOs.

### Linux
```
ant configure-linux compile-linux test-linux
```
artifacts will be in build/libs/

### Android
Use as an Android Studio project or:
```
ant configure-android compile-android test-android
```

### MacOS
```
ant configure-mac compile-mac test-mac install-mac
```
artifacts will be in ./artifactsOut

## Include in your Project

WIP
