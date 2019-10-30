//
// Created by Sascha Roth on 25.10.19.
//

#include "chickenHook/trampoline.h"
#include <stdio.h>
#include <sys/mman.h>
#include <vector>
#include <android/log.h>
#include <unistd.h>

constexpr const char *TAG("Trampoline");


bool updatePermissions(void *addr, int permissions) {
    size_t pagesize = (size_t) sysconf(_SC_PAGESIZE);
    void *lowerBoundary = (void *) ((long long) addr - ((long long) addr % pagesize));

    if (mprotect(lowerBoundary, pagesize, permissions) !=
        0) {
        return false;
    }
    return true;
}

/**
 * Installs the trampoline at the given address
 * @return true on success
 */
bool Trampoline::install() {
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "Install hook at <%p>",
                        _original_addr);
    if (!updatePermissions(_original_addr, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "Unable to update permissions <%p>",
                            _original_addr);
        return false;
    }
    _original_code.resize(CODE_SIZE);
    memcpy(&_original_code[0], _original_addr, CODE_SIZE);
    ((uint64_t *) _original_addr)[0] = 0xffffff;
    if (!updatePermissions(_original_addr, PROT_READ | PROT_EXEC)) {
        return true; // this doesn't break our use-case
    }
    return true;
}

/**
 * Reinstalls the trampoline at the given address
 * Also unlocks the trampoline
 * @return true on success
 */
bool Trampoline::reinstall() {
    install();
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
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "Copy _original_code at %p",
                        _original_addr);
    lock();
    if(!updatePermissions(_original_addr, PROT_READ | PROT_WRITE | PROT_EXEC)){
        return false;
    }
    memcpy(_original_addr, &_original_code[0], CODE_SIZE);
    updatePermissions(_original_addr, PROT_READ | PROT_EXEC);
    return true;
}

void Trampoline::lock() {
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "Lock %p",
                        _original_addr);
    if (_trampoline_lock != nullptr) {
        (*_trampoline_lock).lock();
    }
}

void Trampoline::unlock() {
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "Unlock %p",
                        _original_addr);
    if (_trampoline_lock != nullptr) {
        (*_trampoline_lock).unlock();
    }
}

