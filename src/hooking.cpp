//
// Created by Sascha Roth on 25.10.19.
//

#include "chickenHook/hooking.h"

namespace ChickenHook {

    static std::vector<Trampoline> trampolines;
    constexpr const char *TAG("ChickenHook");


    Hooking &Hooking::getInstance() {
        {
            static Hooking instance; // Guaranteed to be destroyed.
            //instance.init();
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
    bool Hooking::hook(void *addr, void *callback) {
        log("Inject trampoline into <%p> with hookfun <%p>", addr, callback);
        Trampoline trampoline(addr, callback);
        trampoline.install();
        trampolines.push_back(std::move(trampoline));
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
    static void trampoline_receiver(int, siginfo_t *si, void *arg) {
        log("Caught segfault at address <%p>", si->si_addr);

        auto *p = (ucontext_t *) arg;
        if (trampolines.empty()) {
            log("Warning!! trampoline size is 0");
        }

        // search the corresponding trampoline
        for (auto trampoline : trampolines) {
            if (trampoline.getOriginal() == si->si_addr) {
                log("Found hook <%p>", si->si_addr);
                void *hook = trampoline.getHook();
                auto hookFun = (void (*)()) hook;
                log("Overwrite pc");
                // now set the pc to our hook function
#if defined(__APPLE__) && (__x86_64__)
                p->uc_mcontext->__ss.__rip = reinterpret_cast<register_t>(hookFun);
#elif defined(__APPLE__) && (__i386__)
                p->uc_mcontext->__ss.__eip = reinterpret_cast<register_t>(hookFun);
#else
#ifdef __i386__
                p->uc_mcontext.gregs[REG_EIP] = reinterpret_cast<greg_t>(hookFun);
#elif __aarch64__
                p->uc_mcontext.pc = reinterpret_cast<greg_t>(hookFun);
#elif __x86_64__
                p->uc_mcontext.gregs[REG_RIP] = reinterpret_cast<greg_t>(hookFun);
#else
#error "UNSUPPORTED"
#endif
#endif
                si->si_signo = SIGCONT;
            }

        }
        log("Continue execution");
    }

    /**
     * Installs a new sigaction handler
     *
     * We wait for signals triggered by the trampoline.
     * We jump then to the replaced function
     */
    void Hooking::installHandler() {
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
    bool Hooking::init() {
        installHandler();
        return false;
    }

    /**
     * Retrieve the corresponding trampoline for the given address
     *
     * @param addr the address of the trampoline (call by reference)
     * @return true on success
     */
    bool Hooking::getTrampolineByAddr(void *addr, Trampoline &_trampoline) {

        for (auto trampoline : trampolines) {
            if (trampoline.getOriginal() == addr) {
                _trampoline = trampoline;
                return true;
            }
        }
        return false;
    }

    /**
     * Set logging callback.
     * If set, the library will call this function with logging and debugging text. If not set, or set to NULL, log
     * messages will be silently discarded.
     */
    void Hooking::setLoggingCallback(void (*logCallback)(const std::string)) {
        setLogFunction(logCallback);
    }

}