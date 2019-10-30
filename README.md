# ChickenHook

## General

Supported platforms: x86, arm64

## Usage
1. Global variable (will be replaced with singelton in future)

```c
    static ChickenHook chickenHook;
```

2. Init chickenhook (only once!)

```c
    chickenHook.init();
```



3. Create the hook function (the function that should be called instead of the original function)

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
            trampoline.install();
            // that's it!
            return res;
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
        }
        return -1;
    }
```

4. Inject the trampoline  (enable the hook)

```c
    chickenHook.inject((void *) &open, (void *) &my_open);
```


## Build

Linux ant and Android gradle...

## CI
```
https://dev.azure.com/sascharoth/sascharoth/_build?definitionId=2
```
