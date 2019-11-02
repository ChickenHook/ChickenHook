//
// Created by Sascha Roth on 25.10.19.
//

#include "chickenHook/trampoline.h"

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

    /**
     * Installs the trampoline at the given address
     * @return true on success
     */
    bool Trampoline::install(bool doLock) {
        if (doLock) lock();
        log("Install hook at <%p>", _original_addr);
        printInfo();
        if (!updatePermissions(_original_addr, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            log("Unable to update permissions <%p>", _original_addr);

            if (doLock) unlock();
            return false;
        }
        _original_code.resize(CODE_SIZE);
        memcpy(&_original_code[0], _original_addr, CODE_SIZE);
        ((uint64_t *) _original_addr)[0] = 0xffffff;
        if (!updatePermissions(_original_addr, PROT_READ | PROT_EXEC)) {
            if (doLock) unlock();
            return true; // this doesn't break our use-case
        }
        if (doLock) unlock();
        return true;
    }

    /**
     * Reinstalls the trampoline at the given address
     * Also unlocks the trampoline
     * @return true on success
     */
    bool Trampoline::reinstall() {
        install(false);
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
        log("Copy _original_code at %p", _original_addr);
        lock();
        if (!updatePermissions(_original_addr, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            return false;
        }
        memcpy(_original_addr, &_original_code[0], CODE_SIZE);
        updatePermissions(_original_addr, PROT_READ | PROT_EXEC);
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
}