//
// Created by Sascha Roth on 25.10.19.
//

#include <android/log.h>
#include <ucontext.h>
#include <setjmp.h>
#include "chickenHook/injector.h"
#include "chickenHook/trampoline.h"

static std::vector<Trampoline> trampolines;

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
            //p->uc_mcontext.regs[29] = reinterpret_cast<greg_t>(hookFun);
            p->uc_mcontext.pc = reinterpret_cast<greg_t>(hookFun);
#else
#error "UNSUPPORTED"
#endif
            //hookFun();
            //trampoline.copyOriginal();
            //void (*origFun)() =(void (*)()) trampoline.getOriginal();
            //origFun();
            //trampoline.install();
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

bool ChickenHook::init() {
    installHandler();
    return false;
}

bool ChickenHook::getTrampolineByAddr(void *addr, Trampoline &_trampoline) {

    for (auto trampoline : trampolines) {
        if (trampoline.getOriginal() == addr) {
            _trampoline = trampoline;
            return true;
        }
    }
    return false;
}
