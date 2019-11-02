//
// Created by Sascha Roth on 25.10.19.
//

#pragma once

#include <stdio.h>
#include <bits/sysconf.h>
#include <sys/mman.h>
#include <vector>
#include <ucontext.h>
#include <setjmp.h>

#include "trampoline.h"
#include "logging.h"


namespace ChickenHook {

/**
 * The chickenhook library.
 *
 * This framework is build for hooking native functions defined in libraries loaded in our process.
 * ChickenHook uses sigaction for managing jumps from the function to be hooked to our new function.
 * Therefore we write invalid code into the function to be hooked and wait for our registered signal handler.
 */
    class Hooking {

    public:

        static Hooking &getInstance();

        /**
         * Initiates chicken hook
         */
        bool init();

        /**
         * Injects a trampoline at the given address
         *
         * @param addr the address where the trampoline should be injected
         * @param callback the function that will be called instead
         * @return true on success
         */
        bool hook(void *addr, void *callback);

        /**
         * Retrieve the corresponding trampoline for the given address
         *
         * @param addr the address of the trampoline (call by reference)
         * @return true on success
         */
        bool getTrampolineByAddr(void *addr, Trampoline &);

        /**
        * Set logging callback.
        * If set, the library will call this function with logging and debugging text. If not set, or set to NULL, log
        * messages will be silently discarded.
        */
        void setLoggingCallback(void (*logCallback)(const std::string logtext));

    private:

        /**
         * Installs a new sigaction handler
         *
         * We wait for signals triggered by the trampoline.
         * We jump then to the replaced function
         */
        void installHandler();
    };

}