
#include <iostream>
#include "chickenHook/hooking.h"
#include "chickenHook/trampoline.h"
#include "chickenHook/logging.h"

bool doIt() {
    return true;
}

bool myDot() {
    return false;
}

void logCallback(const std::string logtext) {
    std::cout << logtext << std::endl;
}

int main(int argc, char *arv[]) {
    std::cout << "WELCOME TO LINUX CHICKENHOOK TESTS" << std::endl;

    // 1. set logging callback
    ChickenHook::Hooking::getInstance().setLoggingCallback(&logCallback);

    // 2. install a hook
    ChickenHook::Hooking::getInstance().hook((void *) &doIt, (void *) &myDot);

    // 3. check if hook works
    if (doIt()) {  // after applying the hook, we expect false as return value
        return -1;
    }

    return 0;
}