//
// Created by Sascha Roth on 25.10.19.
//

#pragma once

#include "trampoline.h"
#include <stdio.h>
#include <bits/sysconf.h>
#include <sys/mman.h>
#include <vector>

class ChickenHook {

public:

    bool init();

    bool inject(void *addr, void *);

    bool getTrampolineByAddr(void *addr, Trampoline &);

private:


    void installHandler();
};