//
// Created by Sascha Roth on 25.10.19.
//

#include <android/log.h>
#include <ucontext.h>
#include <setjmp.h>
#include "chickenHook/chickenhook.h"
#include "chickenHook/trampoline.h"

static std::vector<Trampoline> trampolines;

/**
 * Injects a trampoline at the given address
 *
 * @param addr the address where the trampoline should be injected
 * @param callback the function that will be called instead
 * @return true on success
 */
bool ChickenHook::inject(void *addr, void *hookFun) {
    {
        Trampoline trampoline(addr, hookFun);
        trampoline.install();
        trampolines.push_back(trampoline);
        return true;
    }
}

void segfault_sigaction(int signal, siginfo_t *si, void *arg) {
    __android_log_print(ANDROID_LOG_DEBUG, "HookSignalHandler", "Caught segfault at address <%p>",
                        si->si_addr);

    ucontext_t *p = (ucontext_t *) arg;
    for (auto trampoline : trampolines) {
        if (trampoline.getOriginal() == si->si_addr) {
            __android_log_print(ANDROID_LOG_DEBUG, "HookSignalHandler", "Found hook <%p>",
                                si->si_addr);
            void *hook = trampoline.getHook();
            void (*hookFun)() =(void (*)()) hook;
            __android_log_print(ANDROID_LOG_DEBUG, "HookSignalHandler", "Overwrite link register");
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


void ChickenHook::installHandler() {
    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfault_sigaction;
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
