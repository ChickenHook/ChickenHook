#include <jni.h>

#include "TransactionCreator.h"

#define  SHA256_DIGEST_LENGTH 32

#include <stdio.h>


static void *sha256Addr;
static void *registerNatives;

/**
 * Function to log messages back to the java call back interface
 * @param logtext String to be logged
 */
void logCallback(const std::string logtext) {

    __android_log_write(ANDROID_LOG_DEBUG, "logCallback", logtext.c_str());

}


/**
 * Original function
 */
void doIt() {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "original functioncalled");
}

/**
 * Hook function
 */
void myDoIt() {
    // this myDoIt will be called instead of doIt
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function called myDoIt");
    // yeah we're inside! But sometimes you want to call the original function also.
    // For this purpose we try to retrieve the corresponding trampoline.
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &doIt, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");
        void (*_doit)() =(void (*)()) trampoline.getRealCallAddr();
        if (_doit == nullptr) {
            // Now we copy the original function code into the original function
            trampoline.copyOriginal();
            // We call the function
            doIt();
            // afterwards we install our trampoline again
            trampoline.reinstall();
            // thats it!
        } else {
            _doit();
        }

    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
}

FILE *my_fopen(const char *__path, const char *__mode) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "fopen called [-] %s", __path);
    FILE *f;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &fopen, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");
        auto _fopen = (FILE *(*)(const char *__path,
                                 const char *__mode)) trampoline.getRealCallAddr();
        if (_fopen == nullptr) {
            trampoline.copyOriginal();
            f = fopen(__path, __mode);
            trampoline.reinstall();
        } else {
            f = _fopen(__path, __mode);
        }

        return f;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return NULL;
}

/*
int my_open(const char *__path, int __flags, ...) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "open called [-] %s", __path);
    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &open, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");

        auto _open = (int (*)(const char *, int,
                              ...)) trampoline.getRealCallAddr();
        if (_open == nullptr) {
            trampoline.copyOriginal();
            res = open(__path, __flags);
            trampoline.reinstall();
        } else {
            res = _open(__path, __flags);
        }
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return -1;
}*/

int my_SHA256_Final(unsigned char *md, void *c) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "sha256 called [-] <%p>", md);

    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(sha256Addr, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");
        auto _sha256_Final = (int
        (*)(unsigned char *md, void *c)) trampoline.getRealCallAddr();

        auto my_SHA256_Final_FUN = (int (*)(unsigned char *,
                                            void *)) (sha256Addr);

        if (_sha256_Final == nullptr) {
            trampoline.copyOriginal();
        } else {
            my_SHA256_Final_FUN = _sha256_Final;
        }

        res = my_SHA256_Final_FUN(md, c);
        md[0] = 'H';
        md[1] = 'O';
        md[2] = 'O';
        md[3] = 'K';
        if (_sha256_Final == nullptr) {
            trampoline.reinstall();
        }
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return res;
}

//////////////////// open library hooking ///////////////

void *my_dlsym(void *__handle, const char *__symbol) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "my_dlsym called [-] <%s>", __symbol);

    void *res = nullptr;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &dlsym, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");
        auto _dlsym = (void *(*)(void *__handle,
                                 const char *__symbol)) trampoline.getRealCallAddr();
        if (_dlsym == nullptr) {
            trampoline.copyOriginal();
            res = dlsym(__handle, __symbol);
            trampoline.reinstall();
        } else {
            res = _dlsym(__handle, __symbol);
        }

        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return res;
}

void *my_dlopen(const char *__filename, int __flag) {

    if (__filename != nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "stringFromJNI", "my_dlopen called [-] <%s>", __filename);
    }


    void *res = nullptr;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &dlopen, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "stringFromJNI",
                            "hooked function call original function");

        trampoline.copyOriginal();

        res = dlopen(__filename, __flag);
        trampoline.reinstall();

        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return res;
}

bool isAlphabet(char ch) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
        return true;
    }
    return false;
}

//////////////////// READ HOOOKING //////////////////////
/**
 * Used for printing read(..)'s binary output
 * @param buffer
 * @param size
 * @param title
 * @return
 */
std::string hexdump(const uint8_t *buffer, std::size_t size, const std::string &title = "HEXDUMP") {
    using namespace std;
    ostringstream ost;
    ost << title << '\n';
    const auto bytes = vector<uint8_t>(buffer, buffer + size);
    std::string chars;
    std::string ASCII;
    for (auto p = cbegin(bytes); p < cend(bytes);) {
        constexpr auto ptrSize = sizeof(void *);
        constexpr auto byteSize = sizeof(uint8_t);
        constexpr auto wordsPerRow = 2;
        const auto bytesToGo = static_cast<unsigned>(std::distance(p, cend(bytes)));
        const auto nBytes = std::min((unsigned) ptrSize, bytesToGo);
        const auto bytesPrinted = static_cast<unsigned>(std::distance(cbegin(bytes), p));
        const auto isFirstRow = bytesPrinted == 0;
        const auto isNewRow = (bytesPrinted % (ptrSize * wordsPerRow)) == 0;
        const auto isLastRow = (p + nBytes) == cend(bytes);

        if (isNewRow && !isLastRow) {
            if (!isFirstRow) {
                ost << "    " << chars << '\n';
                chars.clear();
            }
            ost << hex << setw(ptrSize * 2) << setfill('0') << bytesPrinted << "   ";
        }

        {
            for_each(p, p + nBytes, [&ost, &chars](uint8_t byte) {
                ost << ' ' << hex << setw(byteSize * 2) << setfill('0')
                    << static_cast<unsigned>(byte);
                if (isAlphabet((char) byte)) {
                    chars += (char) (byte);
                } else {
                    chars += '.';
                }
            });
            ost << "  ";
        }

        if (isLastRow) {
            ost << chars << '\n';
        }

        p += nBytes;
    }
    return ost.str();
}


void printLines(std::string content, const std::string &title = "HEXDUMP") {
    std::istringstream streamTable1(content);
    while (!streamTable1.eof()) {
        std::string s1;
        getline(streamTable1, s1);
        while (s1.size() < 9)
            s1 += " ";
        __android_log_print(ANDROID_LOG_DEBUG, title.c_str(), "%s", s1.c_str());

    }
}

void printHexDump(const uint8_t *buffer, std::size_t size, const std::string &title = "HEXDUMP") {
    printLines(hexdump(buffer, size, title), title);
}

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
        //printLines(hexdump(static_cast<const uint8_t *>(__buf), __count, "read"));

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

static jstring my_installHooks(
        JNIEnv *env,
        jobject thiz);


// register natives hook
jint my_RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods,
                        jint size) {
    __android_log_print(ANDROID_LOG_DEBUG, "my_RegisterNatives",
                        "RegisterNatives called with size <%d>", size);

    JNINativeMethod newMethods[size];
    for (int i = 0; i < size; i++) {
        JNINativeMethod jniNativeMethod = methods[i];
        __android_log_print(ANDROID_LOG_DEBUG, "my_RegisterNatives",
                            "Found function: %s with addr <%p>", jniNativeMethod.name,
                            jniNativeMethod.fnPtr);
        newMethods[i].name = jniNativeMethod.name;
        newMethods[i].fnPtr = (void *) &my_installHooks;
        newMethods[i].signature = jniNativeMethod.signature;
    }

    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(registerNatives, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "my_RegisterNatives",
                            "hooked function call original function");

        jint
        (*fun)(JNIEnv *, jclass, const JNINativeMethod *, jint) = (jint (*)(JNIEnv *env, jclass,
                                                                            const JNINativeMethod *,
                                                                            jint)) trampoline.getRealCallAddr();
        if (fun == nullptr) {
            fun = (jint (*)(JNIEnv *, jclass, const JNINativeMethod *, jint)) registerNatives;
            trampoline.copyOriginal();
            res = fun(env, clazz, newMethods, size);
            //trampoline.reinstall(); // doN't reinstall the hook! we're done
        } else {
            res = fun(env, clazz, newMethods, size);
            trampoline.copyOriginal(); // copy back the original function! we're done..
        }
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "my_RegisterNatives",
                            "hooked function cannot call original function");
    }
    return res;
}


std::string BinderReturnCommandToString(uint32_t cmd) {
    std::string res("UNKNOWN");
    switch (cmd) {
        case BR_ERROR:
            return "BR_ERROR";
        case BR_OK:
            return "BR_OK";
        case BR_TRANSACTION_SEC_CTX:
            return "BR_TRANSACTION_SEC_CTX";
        case BR_TRANSACTION:
            return "BR_TRANSACTION";
        case BR_REPLY:
            return "BR_REPLY";
        case BR_ACQUIRE_RESULT:
            return "BR_ACQUIRE_RESULT";
        case BR_DEAD_REPLY:
            return "BR_DEAD_REPLY";
        case BR_TRANSACTION_COMPLETE:
            return "BR_TRANSACTION_COMPLETE";
        case BR_INCREFS:
            return "BR_INCREFS";
        case BR_ACQUIRE:
            return "BR_ACQUIRE";
        case BR_RELEASE:
            return "BR_RELEASE";
        case BR_DECREFS:
            return "BR_DECREFS";
        case BR_ATTEMPT_ACQUIRE:
            return "BR_ATTEMPT_ACQUIRE";
        case BR_NOOP:
            return "BR_NOOP";
        case BR_SPAWN_LOOPER:
            return "BR_SPAWN_LOOPER";
        case BR_FINISHED:
            return "BR_FINISHED";
        case BR_DEAD_BINDER:
            return "BR_DEAD_BINDER";
        case BR_CLEAR_DEATH_NOTIFICATION_DONE:
            return "BR_CLEAR_DEATH_NOTIFICATION_DONE";
        case BR_FAILED_REPLY:
            return "BR_FAILED_REPLY";
        case BR_FROZEN_REPLY:
            return "BR_FROZEN_REPLY";
        case BR_ONEWAY_SPAM_SUSPECT:
            return "BR_ONEWAY_SPAM_SUSPECT";

        default:
            break;
    }

    return res;
}


std::string BinderCommandToString(uint32_t cmd) {
    std::string res("UNKNOWN");
    switch (cmd) {
        case BC_TRANSACTION:
            return "BC_TRANSACTION";
        case BC_REPLY:
            return "BC_REPLY";
        case BC_ACQUIRE_RESULT:
            return "BC_ACQUIRE_RESULT";
        case BC_FREE_BUFFER:
            return "BC_FREE_BUFFER";
        case BC_INCREFS:
            return "BC_INCREFS";
        case BC_ACQUIRE:
            return "BC_ACQUIRE";
        case BC_RELEASE:
            return "BC_RELEASE";
        case BC_DECREFS:
            return "BC_DECREFS";
        case BC_INCREFS_DONE:
            return "BC_INCREFS_DONE";
        case BC_ACQUIRE_DONE:
            return "BC_ACQUIRE_DONE";
        case BC_ATTEMPT_ACQUIRE:
            return "BC_ATTEMPT_ACQUIRE";
        case BC_REGISTER_LOOPER:
            return "BC_REGISTER_LOOPER";
        case BC_ENTER_LOOPER:
            return "BC_ENTER_LOOPER";
        case BC_EXIT_LOOPER:
            return "BC_EXIT_LOOPER";
        case BC_REQUEST_DEATH_NOTIFICATION:
            return "BC_REQUEST_DEATH_NOTIFICATION";
        case BC_CLEAR_DEATH_NOTIFICATION:
            return "BC_CLEAR_DEATH_NOTIFICATION";
        case BC_DEAD_BINDER_DONE:
            return "BC_DEAD_BINDER_DONE";
        case BC_TRANSACTION_SG:
            return "BC_TRANSACTION_SG";
        case BC_REPLY_SG:
            return "BC_REPLY_SG";
        default:
            break;
    }

    return res;
}

int counter = 0;

/*
 2022-05-23 23:58:51.591 22976-23333/org.chickenhook.demoapp D/modifyBwrPost - WRITE:
2022-05-23 23:58:51.591 22976-23333/org.chickenhook.demoapp D/modifyBwrPost - READ: CMD <BR_TRANSACTION_COMPLETE, 29190, 256, 72>
2022-05-23 23:58:51.591 963-3827/? W/Parcel: **** enforceInterface() expected 'android.app.IActivityManager' but read 'android.os.IMessenger'
 */

/*
 2022-05-23 23:58:14.286 22230-22816/org.chickenhook.demoapp D/modifyBwrPost - WRITE: btd <188, 0, 0, 1, 10, 0x10>
2022-05-23 23:58:14.286 963-3791/? W/Parcel: **** enforceInterface() expected 'android.app.IAlarmManager' but read 'android.os.IMessenger'
 */

/*
 2022-05-23 23:57:43.443 22230-22804/org.chickenhook.demoapp D/modifyBwrPost - WRITE: btd <188, 0, 0, 1, f, 0xf>
2022-05-23 23:57:43.443 963-4490/? W/Parcel: **** enforceInterface() expected 'android.os.IUserManager' but read 'android.os.IMessenger'
 */

/*
 2022-05-23 23:54:08.456 22230-22425/org.chickenhook.demoapp D/modifyBwrPost - WRITE: btd <188, 0, 0, 1, 4, 0x4>
2022-05-23 23:54:08.456 963-3889/? W/Parcel: **** enforceInterface() expected 'android.permission.IPermissionManager' but read 'android.os.IMessenger'
 */

/*
2022-05-24 00:04:18.828 418-1117/? W/Parcel: **** enforceInterface() expected 'android.os.IServiceManager' but read 'android.os.IMessenger'


 2022-05-23 23:56:22.803 22230-22703/org.chickenhook.demoapp D/modifyBwrPost - WRITE: btd <188, 0, 0, 1, b, 0xb>
2022-05-23 23:56:22.804 963-1761/? W/Parcel: **** enforceInterface() expected 'android.content.pm.IPackageManager' but read 'android.os.IMessenger'
 */

std::string parseDescriptor(binder_transaction_data *btd) {
    std::string Descriptor;
    uint8_t *ptr = (uint8_t *) btd->data.ptr.buffer;
    ptr += sizeof(uint64_t);
    if (ptr[0] == 'T' &&
        ptr[1] == 'S' &&
        ptr[2] == 'Y' &&
        ptr[3] == 'S'
            ) {
        ptr += sizeof(uint64_t);
        char c = *ptr;
        while (c != 0) {
            Descriptor += c;
            ptr += 2;
            c = *ptr;
        }
//        __android_log_print(ANDROID_LOG_VERBOSE, "parseDescriptor", "DESCRIPTOR <%s>",
//                            Descriptor.c_str());
    } else {
        ptr += sizeof(uint32_t);
        char c = *ptr;
        while (c != 0) {
            Descriptor += c;
            ptr += 2;
            c = *ptr;
        }
//        __android_log_print(ANDROID_LOG_VERBOSE, "parseDescriptor",
//                            "!!!!! NO DESCRIPTOR FOUND !!!!!!");
    }
    return Descriptor;
}

bool overwriteDescriptor(binder_transaction_data *btd,
                         const std::string &NewValue
) {
    std::string Descriptor;
    uint8_t *ptr = (uint8_t *) btd->data.ptr.buffer;
    ptr += sizeof(uint64_t);
    if (ptr[0] == 'T' &&
        ptr[1] == 'S' &&
        ptr[2] == 'Y' &&
        ptr[3] == 'S'
            ) {
        ptr[4] = NewValue.

                length();

        *((uint8_t *) btd->data.ptr.buffer) = 04;
        *((uint8_t *) btd->data.ptr.buffer + 3) = 0xc2;
        ptr += sizeof(uint64_t);
        for (
            auto &c
                : NewValue) {
            *
                    ptr = c;
            *(ptr + 1) = 0;
            ptr += 2;
        }
        return true;
//        __android_log_print(ANDROID_LOG_VERBOSE, "parseDescriptor", "DESCRIPTOR <%s>",
//                            Descriptor.c_str());
    } else {
        ptr[0] = NewValue.

                length();

        ptr += sizeof(uint32_t);
        for (
            auto &c
                : NewValue) {
            *
                    ptr = c;
            *(ptr + 1) = 0;
            ptr += 2;
        }
        return true;
//        __android_log_print(ANDROID_LOG_VERBOSE, "parseDescriptor",
//                            "!!!!! NO DESCRIPTOR FOUND !!!!!!");
    }
    return false;
}

/*
 05-24 11:21:40.648   973  1142 W Binder  : Caught a RuntimeException from the binder stub implementation.
05-24 11:21:40.648   973  1142 W Binder  : java.lang.NullPointerException: Attempt to read from field 'int android.media.PlayerBase$PlayerIdCard.mPlayerType' on a null object reference
05-24 11:21:40.648   973  1142 W Binder  : 	at android.media.AudioPlaybackConfiguration.<init>(AudioPlaybackConfiguration.java:235)
05-24 11:21:40.648   973  1142 W Binder  : 	at com.android.server.audio.PlaybackActivityMonitor.trackPlayer(PlaybackActivityMonitor.java:172)
05-24 11:21:40.648   973  1142 W Binder  : 	at com.android.server.audio.AudioService.trackPlayer(AudioService.java:10852)
05-24 11:21:40.648   973  1142 W Binder  : 	at android.media.IAudioService$Stub.onTransact(IAudioService.java:1632)
05-24 11:21:40.648   973  1142 W Binder  : 	at android.os.Binder.execTransactInternal(Binder.java:1220)
05-24 11:21:40.648   973  1142 W Binder  : 	at android.os.Binder.execTransact(Binder.java:1179)
 */
/*
 NO ERROR


 05-24 11:21:40.653 27066 27098 D modifyBwrPost - WRITE: Attacking Service <android.os.IServiceManager>


 05-24 11:21:40.656   620   642 E SurfaceFlinger: Permission Denial: can't access SurfaceFlinger pid=0, uid=10040


 05-24 11:21:40.662   973  1142 W Binder  : Caught a RuntimeException from the binder stub implementation.
05-24 11:21:40.662   973  1142 W Binder  : java.lang.SecurityException: Package  does not belong to 10040
05-24 11:21:40.662   973  1142 W Binder  : 	at android.app.AppOpsManager.checkPackage(AppOpsManager.java:8803)
05-24 11:21:40.662   973  1142 W Binder  : 	at android.graphics.GraphicsStatsService.requestBufferForProcess(GraphicsStatsService.java:180)
05-24 11:21:40.662   973  1142 W Binder  : 	at android.view.IGraphicsStats$Stub.onTransact(IGraphicsStats.java:89)
05-24 11:21:40.662   973  1142 W Binder  : 	at android.os.Binder.execTransactInternal(Binder.java:1220)
05-24 11:21:40.662   973  1142 W Binder  : 	at android.os.Binder.execTransact(Binder.java:1179)


 05-24 11:21:40.667   973  1142 W Parcel  : Attempt to read object from Parcel 0x70b10978d0 at offset 72 that is not in the object list


 05-24 11:21:40.680   973  1142 W Parcel  : Attempt to read object from Parcel 0x70b10978d0 at offset 92 that is not in the object list

 05-24 11:21:40.683   973  1142 D ActivityManager: Failed to get provider for authority 'null'

 05-24 11:21:40.685   973  1142 E DatabaseUtils: Writing exception to parcel
05-24 11:21:40.685   973  1142 E DatabaseUtils: java.lang.SecurityException: Calling uid: 10040 doesn't match source uid: -1
05-24 11:21:40.685   973  1142 E DatabaseUtils: 	at android.content.AttributionSource.enforceCallingUid(AttributionSource.java:252)
05-24 11:21:40.685   973  1142 E DatabaseUtils: 	at android.content.AttributionSource.enforceCallingUidAndPid(AttributionSource.java:238)
05-24 11:21:40.685   973  1142 E DatabaseUtils: 	at android.content.AttributionSource.<init>(AttributionSource.java:160)
05-24 11:21:40.685   973  1142 E DatabaseUtils: 	at android.content.AttributionSource$1.createFromParcel(AttributionSource.java:494)
05-24 11:21:40.685   973  1142 E DatabaseUtils: 	at android.content.AttributionSource$1.createFromParcel(AttributionSource.java:486)
05-24 11:21:40.685   973  1142 E DatabaseUtils: 	at android.content.ContentProviderNative.onTransact(ContentProviderNative.java:88)
05-24 11:21:40.685   973  1142 E DatabaseUtils: 	at android.os.Binder.execTransactInternal(Binder.java:1220)
05-24 11:21:40.685   973  1142 E DatabaseUtils: 	at android.os.Binder.execTransact(Binder.java:1179)

 05-24 11:21:40.698 27066 27098 D modifyBwrPre - WRITE: Attacking Service <android.view.IGraphicsStats>


 */

/*


    ClipData(Parcel in) {
        mClipDescription = new ClipDescription(in);
        if (in.readInt() != 0) {
            mIcon = Bitmap.CREATOR.createFromParcel(in);
        } else {
            mIcon = null;
        }
        mItems = new ArrayList<>();
        final int N = in.readInt();
        for (int i=0; i<N; i++) {
            CharSequence text = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(in);
            String htmlText = in.readString8();
            Intent intent = in.readTypedObject(Intent.CREATOR);
            Uri uri = in.readTypedObject(Uri.CREATOR);
            ActivityInfo info = in.readTypedObject(ActivityInfo.CREATOR);
            TextLinks textLinks = in.readTypedObject(TextLinks.CREATOR);
            Item item = new Item(text, htmlText, intent, uri);
            item.setActivityInfo(info);
            item.setTextLinks(textLinks);
            mItems.add(item);
        }
    }

 */

/*
 static jstring android_os_Parcel_readString16(JNIEnv* env, jclass clazz, jlong nativePtr)
{
    Parcel* parcel = reinterpret_cast<Parcel*>(nativePtr);
    if (parcel != NULL) {
        size_t len;
        const char16_t* str = parcel->readString16Inplace(&len);
        if (str) {
            return env->NewString(reinterpret_cast<const jchar*>(str), len);
        }
        return NULL;
    }
    return NULL;
}

 */

struct TargetStruct {
    binder_write_read bwr;
    binder_transaction_data btd;
};
TargetStruct *Target_ = nullptr;

std::map<std::string, uint64_t> ServiceMap;

int _code = 0;

#define BINDER_WRITE_READ _IOWR('b', 1, struct binder_write_read)

void DumpBwr(binder_write_read *bwr, const std::string &tag) {
    if (bwr->write_size > 0) {
        std::string write_tag = tag + " - WRITE";

//        allocate_random_heap_buffer((char *)bwr->write_buffer, bwr->write_size);
        uint32_t *cmd = (uint32_t *) bwr->write_buffer;
        __android_log_print(ANDROID_LOG_DEBUG, write_tag.c_str(), "CMD <%s, %u>",
                            BinderCommandToString(*cmd).c_str(), *cmd);
        if (*cmd == BC_TRANSACTION) {
            binder_transaction_data *btd = (binder_transaction_data *) (bwr->write_buffer +
                                                                        sizeof(uint32_t));
            __android_log_print(ANDROID_LOG_DEBUG, write_tag.c_str(),
                                "btd <%lld, %d, %llx, %d, %llu, %x, %p, %d>", btd->data_size,
                                btd->sender_euid, btd->cookie, btd->code, btd->cookie,
                                btd->target.handle,
                                btd->target.ptr, btd->flags);
            if (btd->data_size > 0 && btd->data.ptr.buffer > 0) {
                printHexDump((const uint8_t *) btd->data.ptr.buffer, btd->data_size, write_tag);
            }


            std::string Descriptor = parseDescriptor(btd);
            if (!Descriptor.empty()) {
                if (ServiceMap.count(Descriptor) == 0) {
                    ServiceMap.insert({Descriptor, btd->target.ptr});
                }
            }
            if (Descriptor == "android.os.IMessenger") {
                __android_log_print(ANDROID_LOG_DEBUG, write_tag.c_str(),
                                    "FOUND IMessenger <%s>", Descriptor.c_str());
//                if (Target_ != nullptr) {
//                    __android_log_print(ANDROID_LOG_DEBUG, write_tag.c_str(),
//                                        "COPY data from doTransaction");
//                    memcpy((char *) bwr->write_buffer, (char*)Target_->bwr.write_buffer, bwr->write_size);
//                    __android_log_print(ANDROID_LOG_DEBUG, write_tag.c_str(),
//                                        "Target <%p>",(void*) btd->target.ptr);
//                    Target_ = nullptr;
//                } else {
                    uint64_t TargetId = 0;
                    std::string TargetName;
                    uint32_t id = counter % ServiceMap.size();
                    uint32_t k = 0;
                    for (const auto &it : ServiceMap) {
                        if (k == id) {
                            TargetId = it.second;
                            TargetName = it.first;
                        }
                        k++;
                    }
                    __android_log_print(ANDROID_LOG_DEBUG, write_tag.c_str(),
                                        "Attacking Service <%s, %d>", TargetName.c_str(), counter);
                    if (overwriteDescriptor(btd, TargetName)) {
                        if (parseDescriptor(btd) == Descriptor) {
                            __android_log_print(ANDROID_LOG_ERROR, write_tag.c_str(),
                                                "Unable to replace Descriptor <%s>",
                                                Descriptor.c_str());
                        }
                    }
                    if (counter % ServiceMap.size() == 0) {
                        _code += 1;
                    }
                    btd->code = _code;
                    btd->target.handle = TargetId;
                    btd->target.ptr = TargetId;
                    counter += 1;
                }

//            }

        } //else { temoporary
        printHexDump((const uint8_t *) bwr->write_buffer, bwr->write_size, write_tag);
        //}



//        if(btd->cmd == 1077961472) {
//                printHexDump((const uint8_t *)btd->data.ptr.buffer, btd->data_size, write_tag);
//        }

        //auto dump = hexdump((const uint8_t *)bwr->write_buffer, bwr->write_size);
        //__android_log_print(ANDROID_LOG_DEBUG, "modifyBwrPre", "DUMP <%s>", dump.c_str());
    }
    if (bwr->read_size > 0) {
        std::string read_tag = tag + " - READ";
        uint8_t *ptr = (uint8_t *) bwr->read_buffer;
        uint32_t *cmd = (uint32_t *) ptr;
        if (*cmd == BR_NOOP) {
            ptr += sizeof(uint32_t);
            cmd = (uint32_t *) ptr;
        }
        __android_log_print(ANDROID_LOG_DEBUG, read_tag.c_str(), "CMD <%s, %u, %lld, %d>",
                            BinderReturnCommandToString(*cmd).c_str(), *cmd, bwr->read_size,
                            _IOC_SIZE(BR_TRANSACTION_SEC_CTX));
        if (*cmd == BR_TRANSACTION || *cmd == BR_REPLY) {
            binder_transaction_data *btd = (binder_transaction_data *) (ptr + sizeof(uint32_t));
            // btd->sender_euid
            __android_log_print(ANDROID_LOG_DEBUG, read_tag.c_str(),
                                "btd <%lld, %d, %llx, %d, %llu, %x, %p, %d>", btd->data_size,
                                btd->sender_euid, btd->cookie, btd->code, btd->cookie,
                                btd->target.handle,
                                btd->target.ptr, btd->flags);
            printHexDump((const uint8_t *) btd->data.ptr.buffer, btd->data_size, read_tag);

        } else {
            printHexDump((const uint8_t *) bwr->read_buffer, bwr->read_size, read_tag);
        }
        //std::string write_tag = tag + " - READ";
        //printHexDump((const uint8_t *)bwr->read_buffer, bwr->read_size, write_tag);
//        if(btd->sender_euid == 1000 || btd->sender_euid == 0) {
//            __android_log_print(ANDROID_LOG_DEBUG, tag.c_str(), "REPLY FROM SYSTEM SERVICE FOUND <%d>", btd->sender_euid );
//            if(btd->data.ptr.buffer != 0 && btd->data_size > 0) {
//                //std::string TestOut(btd->data.ptr.buffer, btd->data_size);
//                std::string read_tag = tag + " - READ";
//
//                printHexDump((const uint8_t *)btd->data.ptr.buffer, btd->data_size, read_tag);
////                printHexDump((const uint8_t *)btd->data.ptr.offsets, btd->offsets_size, tag);
//                //__android_log_print(ANDROID_LOG_DEBUG, "modifyBwrPost", "DUMP <%s>", dump.c_str());
//            }
//
//
//
//        }
//        auto dump = hexdump((const uint8_t *)bwr->read_buffer, bwr->read_size);
//        allocate_random_heap_buffer((char *)bwr->read_buffer, bwr->read_size);

//        __android_log_print(ANDROID_LOG_DEBUG, "modifyBwrPost", "DUMP <%s>", dump.c_str());
    }

}

void modifyBwrPre(binder_write_read *bwr) {
    //bwr->write_buffer = bwr->write_buffer +1;
    DumpBwr(bwr, "modifyBwrPre");
}

void modifyBwrPost(binder_write_read *bwr) {
    //bwr->write_buffer = bwr->write_buffer +1;
    DumpBwr(bwr, "modifyBwrPost");

}

int BinderFd = -1;

// int (*)(int, int, ...)
int my_ioctl(int fd, int cmd, unsigned long arg) {
    char fd_path[100];
    snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%d", getpid(), fd);
    char linkb[100];

    int len = readlink(fd_path, linkb, sizeof(linkb));
    std::string link(linkb, len);
    __android_log_print(ANDROID_LOG_DEBUG, "my_ioctl", "my_ioctl called [-] %d, %u, %p, %s", fd,
                        cmd, arg, link.c_str());

    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(
            (void *) (int (*)(int, int, ...)) &ioctl, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "my_ioctl",
                            "hooked function call original function");


        /// do some magic here
        bool isBinder = false;

        if (link.find("/dev/binder") != std::string::npos) {
            BinderFd = fd;
            void *ubuf = (void *) arg;

            isBinder = true;
            __android_log_print(ANDROID_LOG_DEBUG, "my_ioctl",
                                "Binder [+] Found Binder call!! on addr <%p> with cmd <%d> <%d>",
                                ubuf, cmd, BINDER_WRITE_READ);
            if (ubuf != nullptr) {
                //if(cmd == BINDER_WRITE_READ) {
                __android_log_print(ANDROID_LOG_DEBUG, "my_ioctl",
                                    "Binder [+] Found BINDER_WRITE_READ call!!");
                binder_write_read *bwr = (binder_write_read *) ubuf;

                __android_log_print(ANDROID_LOG_DEBUG, "my_ioctl",
                                    "Binder [+] bwr <%llu, %llu>", bwr->read_size, bwr->write_size);
                modifyBwrPre(bwr);
                __android_log_print(ANDROID_LOG_DEBUG, "my_ioctl",
                                    "Binder [+] bwr <%llu, %llu>", bwr->read_size, bwr->write_size);
                //}

            }


        }

        int (*_ioctl)(int fd, int cmd, ...) =(int (*)(int fd, int cmd,
                                                      ...)) trampoline.getRealCallAddr();
        if (_ioctl == nullptr) {
            // !! WARNING !! This is a very risky workaround.
            // * Race condition can lead to crashes
            // * Multithreading and semaphores in target function or it's callee's can lead to deadlocks
//            trampoline.copyOriginal();
//            res = _ioctl(fd, cmd, arg);
//            trampoline.reinstall();
        } else {
            // Very save method. Available for most of all functions
            res = _ioctl(fd, cmd, arg);
        }
        if (isBinder && arg != 0) {
            binder_write_read *bwr = (binder_write_read *) arg;
            modifyBwrPost(bwr);

        }
        //printLines(hexdump(static_cast<const uint8_t *>(__buf), __count, "read"));
//        trampoline.copyOriginal();
//        ioctl(fd, cmd, arg);

        /*
         va_list args;
    va_start(args, b);

    va_end(args);
         */

//        res = read(__fd, __buf, __count);
        //trampoline.install();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return res;
}

/**
 * Some hook examples
 */

static jstring installHooks(
        JNIEnv *env,
        jobject /* this */) {

    // read
    //ChickenHook::Hooking::getInstance().hook((void *) &read, (void *) &my_read);
    // we will see some android read actions ;)

    // open
    /*__android_log_print(ANDROID_LOG_DEBUG, "installHooks", "hook open");
    if (open("/proc/self/maps", O_RDONLY | O_CLOEXEC) <= 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "!!!! FILE DESCRIPTOR <=0 !!!!");
    }
    ChickenHook::Hooking::getInstance().hook((void *) &open, (void *) &my_open);
    if (open("/proc/self/maps", O_RDONLY | O_CLOEXEC) <= 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "!! FILE DESCRIPTOR <=0 !!");
    }*/

//
//    // doIt
//    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "install hook");
//    //ChickenHook::Hooking::getInstance().hook((void *) &doIt, (void *) &myDoIt);
//    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "hook installed");
//    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "call function");
//    //doIt();
//
//    // fopen
//    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "hook fopen");
//    ChickenHook::Hooking::getInstance().hook((void *) &fopen, (void *) &my_fopen);
//    if (fopen("/proc/self/maps", "r") == nullptr) {
//        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "!! FILE DESCRIPTOR NULL !!");
//    }
//
//
//    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "call fopen");
//
//    if (fopen("/proc/self/maps", "r") == nullptr) {
//        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "!! FILE DESCRIPTOR NULL !!");
//    } // we should see open and fopen hook was triggered while calling fopen!
//
//
//
//
//    // try to hook function our library doesn't link against...
//    // SHA256_Final
//    sha256Addr = dlsym(RTLD_DEFAULT, "SHA256_Final");
//    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "sha256Addr ADDR %p", sha256Addr);
//    if (sha256Addr != nullptr) {
//        ChickenHook::Hooking::getInstance().hook(sha256Addr, (void *) &my_SHA256_Final);
//    }

    if (!ChickenHook::Hooking::getInstance().hook((void *) (int (*)(int, int, ...)) &ioctl,
                                                  (void *) (int (*)(int, int,
                                                                    unsigned long)) (&my_ioctl))) {
        __android_log_print(ANDROID_LOG_ERROR, "stringFromJNI", "Unable to install ioctl hook %p",
                            (void *) (int (*)(int, int, ...)) &ioctl);
    }


    // dl hooking
    //ChickenHook::Hooking::getInstance().hook((void *) &dlsym, (void *) &my_dlsym);
    //ChickenHook::getInstance().hook((void *) &dlopen, (void *) &my_dlopen);


    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}



///////// JNI HOOKING

// lets approve hook register natives works!!
static jstring my_installHooks(
        JNIEnv *env,
        jobject thiz) {
    __android_log_print(ANDROID_LOG_DEBUG, "my_installHooks", "JNI hook called!");

    // let's manpulate the string that will be returned to the VM
    std::string hookStr = "Hello from our Hook ;)";
    jstring hookJavaString = env->NewStringUTF(hookStr.c_str());

    // this hook works without trampoline!
    installHooks(env, thiz);

    return hookJavaString;
}

static jint doTransaction(
        JNIEnv *env,
        jobject thiz,
        jstring Target,
        jint code,
        jbyteArray marshalledData,
        jbyteArray joffsets) {
    __android_log_print(ANDROID_LOG_DEBUG, "doTransaction", "Do transaction called!");
    jboolean copy = false;
    const char *chars = env->GetStringUTFChars(Target, &copy);
    jboolean copyBytes = false;
    uint8_t *data = (uint8_t *) env->GetByteArrayElements(marshalledData, &copyBytes);
    uint32_t dataSize = env->GetArrayLength(marshalledData);
    jboolean copyOffsetBytes = false;
    binder_size_t *offsets = (binder_size_t *) env->GetByteArrayElements(joffsets,
                                                                         &copyOffsetBytes);
    uint32_t offsetsSize = env->GetArrayLength(marshalledData);
    int res;

    if (ServiceMap.count(chars) == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "doTransaction", "Did not find service <%s>", chars);
        return 0;
    }
    int TargetHandle = ServiceMap[chars];

    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(
            (void *) (int (*)(int, int, ...)) &ioctl, trampoline)) {
        int (*_ioctl)(int fd, int cmd, ...) =(int (*)(int fd, int cmd,
                                                      ...)) trampoline.getRealCallAddr();
        if (_ioctl == nullptr) {
            __android_log_print(ANDROID_LOG_ERROR, "doTransaction", "Error while download files");

            return 1;
        } else {
            // Very save method. Available for most of all functions
            Target_ = static_cast<TargetStruct *>(malloc(sizeof(struct TargetStruct)));
            TransactionCreator::BuildBwr(TargetHandle, code, data, dataSize, offsets, offsetsSize,
                                         Target_->bwr);

//            res = my_ioctl(BinderFd, BINDER_WRITE_READ, (unsigned long) (uint64_t) &Target_->bwr);
//            __android_log_print(ANDROID_LOG_INFO, "doTransaction", "DONE <%d>", res);
        }
    }

    if (copyOffsetBytes) {
        env->ReleaseByteArrayElements(joffsets, (jbyte *) offsets, 0);
    }
    if (copyBytes) {
        env->ReleaseByteArrayElements(marshalledData, (jbyte *) data, 0);
    }
    if (copy) {
        env->ReleaseStringUTFChars(Target, chars);
    }
    return 1;
}


////////// JNI STUFF

static const JNINativeMethod gMethods[] = {
        {"installHooks",  "()Ljava/lang/String;",       (void *) installHooks},
        {"doTransaction", "(Ljava/lang/String;I[B[B)I", (void *) doTransaction},
};
static const char *classPathName = "org/chickenhook/demoapp/NativeLib";

static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "registerNativeMethods",
                            "Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "registerNativeMethods",
                            "Native registration unable to register natives...");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM *vm, void * /*reserved*/) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    for (char16_t c = 0; c < 65535; c++) {
        env->NewString((jchar *) &c, 2);
    }
    /*
         for(unsigned char c = 0;c<255;c++) {
        env->NewStringUTF((char*)&c);
    }
     */

    ChickenHook::Hooking::getInstance().setLoggingCallback(&logCallback);

//    // hook register natives
//    registerNatives = (void *) env->functions->RegisterNatives;/*dlsym(
//            RTLD_DEFAULT, // load other library with dlopen RTLD_NOW and set the handle here...
//            "_ZN7_JNIEnv15RegisterNativesEP7_jclassPK15JNINativeMethodi"); // you can also try to hook jniRegisterNativeMethods of libandroid_runtime.so*/
//    if (registerNatives != nullptr) {
//        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "registerNatives ADDR %p",
//                            registerNatives);
//        ChickenHook::Hooking::getInstance().hook(registerNatives,
//                                                 (void *) &my_RegisterNatives);
//    }


    if (!registerNativeMethods(env, classPathName,
                               (JNINativeMethod *) gMethods,
                               sizeof(gMethods) / sizeof(gMethods[0]))) {
        return -1;
    }

    return JNI_VERSION_1_4;
}