#include <jni.h>
#include <string>
#include <android/log.h>
#include <dlfcn.h>
#include <vector>
#include "chickenHook/chickenhook.h"
#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <unistd.h>


jlong (*addressOf)(JNIEnv *, jobject, jobject);


uint8_t bytes[] = {
        0x55, 0x89, 0xe5, 0x53, 0x57,
        0x56, 0x83, 0xe4, 0xf0, 0x83,
        0xec, 0x30, 0xe8
};

static ChickenHook chickenHook;
static void *sha256Addr;

# define SHA256_DIGEST_LENGTH    32


void doIt() {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "original functioncalled");
}

void myDoIt() {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hooked function called myDoIt");
    Trampoline trampoline;
    if (chickenHook.getTrampolineByAddr((void *) &doIt, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");

        trampoline.copyOriginal();
        doIt();
        trampoline.install();

    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
}

FILE *my_fopen(const char *__path, const char *__mode) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "fopen called [-] %s", __path);
    FILE *f;
    Trampoline trampoline;
    if (chickenHook.getTrampolineByAddr((void *) &fopen, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");

        trampoline.copyOriginal();
        f = fopen(__path, __mode);
        trampoline.install();
        return f;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return NULL;
}

int my_open(const char *__path, int __flags, ...) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "open called [-] %s", __path);

    int res = -1;
    Trampoline trampoline;
    if (chickenHook.getTrampolineByAddr((void *) &open, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");

        trampoline.copyOriginal();
        res = open(__path, __flags);
        trampoline.install();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return -1;
}

int my_SHA256_Final(unsigned char *md, void *c) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "sha256 called [-] <%p>", md);

    int res = -1;
    Trampoline trampoline;
    if (chickenHook.getTrampolineByAddr((void *) &sha256Addr, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");

        trampoline.copyOriginal();
        int ((*my_SHA256_Final_FUN)(unsigned char *, void *)) =(int (*)(unsigned char *,
                                                                        void *)) (sha256Addr);
        res = my_SHA256_Final_FUN(md, c);
        trampoline.install();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return res;
}


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
    Trampoline trampoline;
    if (chickenHook.getTrampolineByAddr((void *) &read, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");
        printLines(hexdump(static_cast<const uint8_t *>(__buf), __count, "read"));
        trampoline.copyOriginal();
        res = read(__fd, __buf, __count);
        trampoline.install();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return res;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_self_vmcracker_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */,
        jlong address) {

    /*std::vector<uint8_t> byte_vector(&bytes[0], &bytes[12]);

    JniAddressFinder nativeLibFinder("native-lib.so");

    JniAddressFinder jniAddressFinder("libart.so");
    if (jniAddressFinder.load() && nativeLibFinder.load()) {
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib", "Found android runtime");

        jniAddressFinder.getAddressOfJavaFunction(byte_vector);

        std::string functionName = "_ZN3artL19VMRuntime_addressOfEP7_JNIEnvP8_jobjectS3_";
        //Dl_info *handle = (Dl_info *) dlopen(nativeLibFinder.getPath().c_str(), RTLD_LAZY);
        //handle->dli_saddr = jniAddressFinder.getLibraryAddress();
        //handle->dli_fname = jniAddressFinder.getPath().c_str();

        //addressOf = (jlong (*)(JNIEnv, jobject)) dlsym(handle, functionName.c_str());
        addressOf = (jlong (*)(JNIEnv *, jobject, jobject)) ((unsigned long) jniAddressFinder.getLibraryAddress() +
                                                             0x49cc50);
        if (!addressOf) {
            __android_log_print(ANDROID_LOG_DEBUG, "native-lib", "Error: %s\n", dlerror());
            //dlclose(handle);
            //return EXIT_FAILURE;
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "native-lib", "found addressOf");
            addressOf(nullptr, nullptr, nullptr);
        }


    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib", "Not found android runtime");
    }
    /*jniAddressFinder.getAddressOfJavaFunction("addressOf","This is a teststring");*/
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "install hook");
    chickenHook.init();
    chickenHook.inject((void *) &doIt, (void *) &myDoIt);
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "hook installed");
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "call function");
    doIt();
    //doIt();
    //doIt();
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "inject fopen");
    chickenHook.inject((void *) &fopen, (void *) &my_fopen);
    if (fopen("/proc/self/maps", "r") == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "!! FILE DESCRIPTOR NULL !!");
    }

    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "inject open");
    chickenHook.inject((void *) &open, (void *) &my_open);
    if (open("/proc/self/maps", O_RDONLY | O_CLOEXEC) <= 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "!! FILE DESCRIPTOR <=0 !!");
    }
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "address was <%p>",
                        reinterpret_cast<void *>(address));
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "call fopen");

    if (fopen("/proc/self/maps", "r") == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "!! FILE DESCRIPTOR NULL !!");
    }
    chickenHook.inject((void *) &read, (void *) &my_read);
    void *dlopenAddr = dlsym(RTLD_NEXT, "dlopen");
    sha256Addr = dlsym(RTLD_NEXT, "SHA256_Final");
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "DLOPEN ADDR %p", dlopenAddr);
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "sha256Addr ADDR %p", sha256Addr);
    if (sha256Addr != nullptr) {
        chickenHook.inject((void *) &sha256Addr, (void *) &my_SHA256_Final);
    }


    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
