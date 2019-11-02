#include <jni.h>
#include <string>
#include <android/log.h>
#include <dlfcn.h>
#include <vector>
#include "chickenHook/hooking.h"
#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <unistd.h>

#define  SHA256_DIGEST_LENGTH 32

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
    ChickenHook::Hooking::getInstance().setLoggingCallback(&logCallback);
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
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function call original function");
        // Now we copy the original function code into the original function
        trampoline.copyOriginal();
        // We call the function
        doIt();
        // afterwards we install our trampoline again
        trampoline.reinstall();
        // thats it!
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function cannot call original function");
    }
}

FILE *my_fopen(const char *__path, const char *__mode) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "fopen called [-] %s", __path);
    FILE *f;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &fopen, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function call original function");

        trampoline.copyOriginal();
        f = fopen(__path, __mode);
        trampoline.reinstall();
        return f;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function cannot call original function");
    }
    return NULL;
}

int my_open(const char *__path, int __flags, ...) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "open called [-] %s", __path);

    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &open, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function call original function");

        trampoline.copyOriginal();
        res = open(__path, __flags);
        trampoline.reinstall();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function cannot call original function");
    }
    return -1;
}

int my_SHA256_Final(unsigned char *md, void *c) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "sha256 called [-] <%p>", md);

    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(sha256Addr, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function call original function");

        trampoline.copyOriginal();
        int ((*my_SHA256_Final_FUN)(unsigned char *, void *)) =(int (*)(unsigned char *, void *)) (sha256Addr);
        res = my_SHA256_Final_FUN(md, c);
        md[0] = 'H';
        md[1] = 'O';
        md[2] = 'O';
        md[3] = 'K';
        trampoline.reinstall();
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
    Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &dlsym, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");

        trampoline.copyOriginal();
        res = dlsym(__handle, __symbol);
        trampoline.reinstall();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function cannot call original function");
    }
    return res;
}

void *my_dlopen(const char *__filename, int __flag) {

    if (__filename != nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "stringFromJNI", "my_dlopen called [-] <%s>", __filename);
    }


    void *res = nullptr;
    Trampoline trampoline;
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

    for (auto p = cbegin(bytes); p < cend(bytes);) {
        constexpr auto ptrSize = sizeof(void *);
        constexpr auto byteSize = sizeof(uint8_t);
        constexpr auto wordsPerRow = 4;
        const auto bytesToGo = static_cast<unsigned>(std::distance(p, cend(bytes)));
        const auto nBytes = std::min((unsigned) ptrSize, bytesToGo);
        const auto bytesPrinted = static_cast<unsigned>(std::distance(cbegin(bytes), p));
        const auto isFirstRow = bytesPrinted == 0;
        const auto isNewRow = (bytesPrinted % (ptrSize * wordsPerRow)) == 0;
        const auto isLastRow = (p + nBytes) == cend(bytes);

        if (isNewRow && !isLastRow) {
            if (!isFirstRow) {
                ost << '\n';
            }
            ost << hex << setw(ptrSize * 2) << setfill('0') << bytesPrinted << "   ";
        }

        {
            for_each(p, p + nBytes, [&ost, &chars](uint8_t byte) {
                ost << ' ' << hex << setw(byteSize * 2) << setfill('0')
                    << static_cast<unsigned>(byte);
                chars += (char) (byte);
            });
            ost << "   ";
        }

        if (isLastRow) {
            ost << chars << '\n';
        }

        p += nBytes;
    }
    return ost.str();
}

void printLines(std::string content) {
    std::istringstream streamTable1(content);
    while (!streamTable1.eof()) {
        std::string s1;
        getline(streamTable1, s1);
        while (s1.size() < 9)
            s1 += " ";
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "%s", s1.c_str());

    }
}

ssize_t my_read(int __fd, void *__buf, size_t __count) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "read called [-] %d", __fd);

    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &read, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function call original function");
       // printLines(hexdump(static_cast<const uint8_t *>(__buf), __count, "read"));
        trampoline.copyOriginal();
        res = read(__fd, __buf, __count);
        trampoline.reinstall();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function cannot call original function");
    }
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
    Trampoline trampoline;
    auto fp = &_JNIEnv::RegisterNatives;
    if (ChickenHook::getInstance().getTrampolineByAddr(*((void **) &fp), trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "my_RegisterNatives",
                            "hooked function call original function");
        trampoline.copyOriginal();

        jint
        (*fun)(JNIEnv *, jclass, const JNINativeMethod *, jint) = (jint (*)(JNIEnv *env, jclass,
                                                                            const JNINativeMethod *,
                                                                            jint)) registerNatives;
        res = fun(env, clazz, newMethods, size);
        trampoline.reinstall();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "my_RegisterNatives",
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

    // doIt
    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "install hook");
    ChickenHook::Hooking::getInstance().hook((void *) &doIt, (void *) &myDoIt);
    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "hook installed");
    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "call function");
    doIt();

    // fopen
    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "hook fopen");
    ChickenHook::Hooking::getInstance().hook((void *) &fopen, (void *) &my_fopen);
    if (fopen("/proc/self/maps", "r") == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "!! FILE DESCRIPTOR NULL !!");
    }

    // open
    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "hook open");
    ChickenHook::Hooking::getInstance().hook((void *) &open, (void *) &my_open);
    if (open("/proc/self/maps", O_RDONLY | O_CLOEXEC) <= 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "!! FILE DESCRIPTOR <=0 !!");
    }

    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "call fopen");

    if (fopen("/proc/self/maps", "r") == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "!! FILE DESCRIPTOR NULL !!");
    } // we should see open and fopen hook was triggered while calling fopen!

    // read
    ChickenHook::Hooking::getInstance().hook((void *) &read, (void *) &my_read);
    // we will see some android read actions ;)

    // try to hook function our library doesn't link against...
    // SHA256_Final
    sha256Addr = dlsym(RTLD_DEFAULT, "SHA256_Final");
    __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "sha256Addr ADDR %p", sha256Addr);
    if (sha256Addr != nullptr) {
        ChickenHook::Hooking::getInstance().hook(sha256Addr, (void *) &my_SHA256_Final);
    }


    // dl hooking
    ChickenHook::Hooking::getInstance().hook((void *) &dlsym, (void *) &my_dlsym);
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


////////// JNI STUFF

static const JNINativeMethod gMethods[] = {
        {"installHooks", "()Ljava/lang/String;", (void *) installHooks},
};
static const char *classPathName = "com/self/vmcracker/MainActivity";

static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
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
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    // hook register natives
    registerNatives = dlsym(
            RTLD_DEFAULT, // load other library with dlopen RTLD_NOW and set the handle here...
            "_ZN7_JNIEnv15RegisterNativesEP7_jclassPK15JNINativeMethodi"); // you can also try to hook jniRegisterNativeMethods of libandroid_runtime.so
    if (registerNatives != nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "installHooks", "registerNatives ADDR %p",
                            registerNatives);
        ChickenHook::getInstance().hook(registerNatives,
                                        (void *) &my_RegisterNatives);
    }


    if (!registerNativeMethods(env, classPathName,
                               (JNINativeMethod *) gMethods,
                               sizeof(gMethods) / sizeof(gMethods[0]))) {
        return -1;
    }

    return JNI_VERSION_1_4;
}