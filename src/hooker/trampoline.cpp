//
// Created by sascharoth on 25.10.19.
//

#include "chickenHook/trampoline.h"
#include <stdio.h>
#include <bits/sysconf.h>
#include <sys/mman.h>
#include <vector>
#include <android/log.h>

bool updatePermissions(void *addr) {
    size_t pagesize = (size_t) sysconf(_SC_PAGESIZE);
    void *lowerBoundary = (void *) ((long long) addr - ((long long) addr % pagesize));

    if (mprotect(lowerBoundary, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC) !=
        0) {
        return false;
    }
    return true;
}

bool Trampoline::install() {
    updatePermissions(addr);
    __android_log_print(ANDROID_LOG_DEBUG, "Trampoline", "Install hook at %p",
                        addr);
    code.resize(CODE_SIZE);
    memcpy(&code[0], addr, CODE_SIZE);
    ((uint64_t *) addr)[0] = 0xffffff;
    return false;
}

Trampoline::Trampoline(void *addr, void *hookFun) {
    this->addr = addr;
    this->hookFun = hookFun;
}

void *Trampoline::getHook() {
    return hookFun;
}

void *Trampoline::getOriginal() {
    return addr;
}

void *Trampoline::copyOriginal() {
    __android_log_print(ANDROID_LOG_DEBUG, "Trampoline", "Copy code at %p",
                        addr);
    memcpy(addr, &code[0], CODE_SIZE);
    //void (*origFun)() =(void (*)()) addr;
    //origFun();
    return nullptr;
}

Trampoline::Trampoline() {
};