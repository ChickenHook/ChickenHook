
#pragma once

#include <stdio.h>

#include <vector>
#include <setjmp.h>
#ifdef __aarch64__
#define CODE_SIZE 64
#elif __i386__
#define CODE_SIZE 128
#else
#error "UNSUPPORTED"
#endif

class Trampoline {
public:
    Trampoline(void *addr, void *);
    Trampoline();

    bool install();


    void *getHook();

    void *copyOriginal();

    void *getOriginal();

private:
    void *addr;
    void *hookFun;
    std::vector<uint8_t> code;

};