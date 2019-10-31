//
// Created by Sascha Roth on 25.10.19.
//

#include <android/log.h>
#include <ucontext.h>
#include <setjmp.h>
#include "chickenHook/chickenhook.h"
#include "chickenHook/trampoline.h"

static std::vector<Trampoline> trampolines;
constexpr const char *TAG("ChickenHook");


ChickenHook &ChickenHook::getInstance() {
    {
        static ChickenHook instance; // Guaranteed to be destroyed.
        instance.init();
        // Instantiated on first use.
        return instance;
    }
}

/**
 * Injects a trampoline at the given address
 *
 * @param addr the address where the trampoline should be injected
 * @param callback the function that will be called instead
 * @return true on success
 */
bool ChickenHook::hook(void *addr, void *callback) {
    __android_log_print(ANDROID_LOG_DEBUG, TAG,
                        "Inject trampoline into <%p> with hookfun <%p>", addr, callback);
    Trampoline trampoline(addr, callback);
    trampoline.install();
    trampolines.push_back(trampoline);
    return true;
}

/**
 * Our trampoline receiver.
 *
 * This function should be triggered by the trampoline using linux signals: http://man7.org/linux/man-pages/man7/signal.7.html
 * We inject invalid code into the function to be hooked so that linux triggers a signal like SIGILL or SIGSEGV.
 * We catch those signals in this function
 * @param signal the signal code
 * @param si signal information
 * @param arg ucontext
 */
static void trampoline_receiver(int signal, siginfo_t *si, void *arg) {
    __android_log_print(ANDROID_LOG_DEBUG, "HookSignalHandler", "Caught segfault at address <%p>",
                        si->si_addr);

    auto *p = (ucontext_t *) arg;
    // search the corresponding trampoline
    for (auto trampoline : trampolines) {
        if (trampoline.getOriginal() == si->si_addr) {
            __android_log_print(ANDROID_LOG_DEBUG, "HookSignalHandler", "Found hook <%p>",
                                si->si_addr);
            void *hook = trampoline.getHook();
            auto hookFun = (void (*)()) hook;
            __android_log_print(ANDROID_LOG_DEBUG, "HookSignalHandler", "Overwrite pc");
            // now set the pc to our hook function
#ifdef __i386__
            p->uc_mcontext.gregs[REG_EIP] = reinterpret_cast<greg_t>(hookFun);
#elif __aarch64__
            p->uc_mcontext.pc = reinterpret_cast<greg_t>(hookFun);
#elif __x86_64__
            p->uc_mcontext.gregs[REG_RIP] = reinterpret_cast<greg_t>(hookFun);
#else
#error "UNSUPPORTED"
#endif
        }

    }
    __android_log_print(ANDROID_LOG_DEBUG, "HookSignalHandler", "Continue execution");

    //exit(0);
}

/**
 * Installs a new sigaction handler
 *
 * We wait for signals triggered by the trampoline.
 * We jump then to the replaced function
 */
void ChickenHook::installHandler() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = trampoline_receiver;
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
}

/**
 * Initiates chicken hook
 */
bool ChickenHook::init() {
    installHandler();
    return false;
}

/**
 * Retrieve the corresponding trampoline for the given address
 *
 * @param addr the address of the trampoline (call by reference)
 * @return true on success
 */
bool ChickenHook::getTrampolineByAddr(void *addr, Trampoline &_trampoline) {

    for (auto trampoline : trampolines) {
        if (trampoline.getOriginal() == addr) {
            _trampoline = trampoline;
            return true;
        }
    }
    return false;
}
