//
// Created by Sascha Roth on 25.10.19.
//

#include "chickenHook/trampoline.h"
// Instructions helper
#ifdef __aarch64__
#include "instsHelper/arm64_insts_helper.h" // arm64?
#elif __i386__

#include "instsHelper/x86_insts_helper.h"

#elif __x86_64__
#include "instsHelper/x86_64_insts_helper.h"
#else
#error "UNSUPPORTED"
#endif

namespace ChickenHook {

    constexpr const char *TAG("Trampoline");


    bool updatePermissions(void *addr, int permissions) {
        size_t pagesize = (size_t) sysconf(_SC_PAGESIZE);
        void *lowerBoundary = (void *) ((long long) addr - ((long long) addr % pagesize));

        if (mprotect(lowerBoundary, pagesize, permissions) != 0) {
            return false;
        }
        return true;
    }

    extern "C" void doCrash() {
        void (*test)() = nullptr;
        test();
    }

    std::vector<uint8_t> generateJump(void *src, void *dest) {
        std::vector<uint8_t> jump;

#ifdef __i386__

        // x86
        jump.resize(5);
        jump[0] = 0xE9; // indirect jump
        int insOff = 0x5;
        uint32_t relativeAddr = ((uint32_t) dest - (uint32_t) src - insOff);
        log("Calculate relative jump addr <%d> - src <%p> dest <%p>", (int32_t) relativeAddr, src,
            dest);

        memcpy(&jump[1], &relativeAddr, 4);

#elif __aarch64__

#elif __x86_64__
        // x86_64
        jump.resize(TRAMPOLINE_SIZE);
        jump[0] = 0xFF; // indirect jump
        jump[1] = 0x25;
        jump[2] = 0x00;
        jump[3] = 0x00;
        jump[4] = 0x00;
        jump[5] = 0x00;
        int insOff = TRAMPOLINE_SIZE;
        uint64_t relativeAddr = (uint64_t) dest; //((uint64_t) dest - (uint64_t) src - insOff); // - insOff
        log("Calculate relative jump addr <%p> - src <%p> dest <%p>", (uint64_t) relativeAddr, src,
            dest);

        memcpy(&jump[6], &relativeAddr, TRAMPOLINE_SIZE - 6);
#else
#error "UNSUPPORTED"
#endif

        return jump;
    }

    /**
     * Installs the trampoline at the given address
     * @return true on success
     */
    bool Trampoline::install(bool doLock) {
        if (doLock) lock();
        log("Install hook at <%p>", _original_addr);
        printInfo();
        if (!updatePermissions(_original_addr, PROT_NONE | PROT_READ | PROT_WRITE | PROT_EXEC)) {
            log("Unable to update permissions <%p>", _original_addr);

            if (doLock) unlock();
            return false;
        }
        // make backup of original code
        _original_code.resize(CODE_SIZE);
        memcpy(&_original_code[0], _original_addr, CODE_SIZE);

        // check function compatibility and required trampoline size (to not destroy greater instructions)
        int requiredTrampolinSize = nextInstOff(_original_addr, TRAMPOLINE_SIZE);
        if (requiredTrampolinSize == -1) {
            /**
             * Without landing pad, we have to copy the original code, execute it and install hook again.
             */
            log("Unsupported instructions found.. cannot use landing pad. ");
            _real_call_addr = nullptr;
            _real_call_code.clear();
        } else {
            /**
             * Our hook can call the landing pad instead of the original function in order to execute the original code.
             */
            log("Required trampoline size <%d>", requiredTrampolinSize);

            // create real call code
            _real_call_code.resize(
                    requiredTrampolinSize + TRAMPOLINE_SIZE); // 2* TR size + opcode fix

            // copy original code of trampoline size and add trampoline to jump back
            memcpy(&_real_call_code[0], &_original_code[0], requiredTrampolinSize);
            std::vector<uint8_t> backjump = generateJump(
                    (void *) &_real_call_code[requiredTrampolinSize],
                    (void *) (((char *) _original_addr + (requiredTrampolinSize))));
            memcpy(&_real_call_code[requiredTrampolinSize], &backjump[0], TRAMPOLINE_SIZE);

            // make real call code executable
            if (!updatePermissions(&_real_call_code[0], PROT_READ | PROT_WRITE | PROT_EXEC)) {
                log("Unable to update permissions of landing pad <%p>", &_real_call_code[0]);

                _real_call_addr = nullptr;
                _real_call_code.clear();
            } else {
                _real_call_addr = &_real_call_code[0];
                log("Created landing pad at <%p>", _real_call_addr);
            }
        }


        // insert jump to hooking function
        _trampoline = generateJump(_original_addr, _hook_addr);
        memcpy(_original_addr, &_trampoline[0], TRAMPOLINE_SIZE);

        /*if (!updatePermissions(_original_addr, PROT_READ | PROT_EXEC)) {
            if (doLock) unlock();
            return true; // this doesn't break our use-case
        }*/


        if (doLock) unlock();
        return true;
    }

    /**
     * Reinstalls the trampoline at the given address
     * Also unlocks the trampoline
     * @return true on success
     */
    bool Trampoline::reinstall() {
        log("Renstall hook at <%p>", _original_addr);
        if (!updatePermissions(_original_addr, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            log("Unable to update permissions <%p>", _original_addr);

            unlock();
            return false;
        }
        memcpy(_original_addr, &_trampoline[0], TRAMPOLINE_SIZE);
        //updatePermissions(_original_addr, PROT_READ | PROT_EXEC);
        unlock();
        return true;
    }

    /**
     * Creates a new Trampoline
     * @param addr the function the trampoline will be isntalled
     * @param callback the function that will be called instead
     */
    Trampoline::Trampoline(void *addr, void *hookFun) {
        this->_original_addr = addr;
        this->_hook_addr = hookFun;
        this->_trampoline_lock = new std::mutex();
        this->infoAvailable = dladdr(addr, &__info);
        log("Constructor CALL!!");
    }

    /**
     * Used for call by reference calls
     */
    Trampoline::Trampoline() {};

    /**
     * Returns the hook function
     * @return the address of the function to be called instead of the original function
     */
    void *Trampoline::getHook() {
        return _hook_addr;
    }

    /**
     * @return the address of the function where the trampoline was / will be installed
     */
    void *Trampoline::getOriginal() {
        return _original_addr;
    }

    /**
     * Copies the original bytecode into the original function
     * @return
     */
    bool Trampoline::copyOriginal() {
        log("Copy _original_code at %p with len <%d>", _original_addr, _original_code.size());
        lock();
        if (!updatePermissions(_original_addr, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            return false;
        }
        memcpy(_original_addr, &_original_code[0], TRAMPOLINE_SIZE);
        //updatePermissions(_original_addr, PROT_READ | PROT_EXEC);
        return true;
    }

    /**
     * Locks the trampoline.
     * Other requests to copy or reinstall the trampoline must wait till trampoline is unlocked.
     */
    void Trampoline::lock() {
        log("Lock %p", _original_addr);
        if (_trampoline_lock != nullptr) {
            (*_trampoline_lock).lock();
        }
    }

    /**
    * Locks the trampoline.
    * Other requests to copy or reinstall the trampoline must wait till trampoline is unlocked.
    */
    void Trampoline::unlock() {
        log("Unlock %p", _original_addr);
        if (_trampoline_lock != nullptr) {
            (*_trampoline_lock).unlock();
        }
    }

    /**
     * Prints some information about the trampoline
     */
    void Trampoline::printInfo() {
        if (infoAvailable == 0) {
            return;
        }
        std::string libName;
        std::string symbolName;
        if (__info.dli_fname != nullptr) {
            libName = __info.dli_fname;
        }
        if (__info.dli_sname != nullptr) {
            symbolName = __info.dli_sname;
        }

        log("Trampoline: <%p> => %s:%s ",
            _original_addr, symbolName.c_str(), libName.c_str());
    }

    /**
     * @inherit
     */
    void *Trampoline::getRealCallAddr() {
        log("Forward landing pad at <%p>", _real_call_addr);
        return _real_call_addr;
    }
}