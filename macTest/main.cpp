
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

// puts output of cli tools into a string
std::string GetStdoutFromCommand(std::string cmd) {

    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
    return data;
}


int main(int argc, char *arv[]) {
    std::cout << "WELCOME TO MAC CHICKENHOOK TESTS" << std::endl;

    std::string vmmap = GetStdoutFromCommand("vmmap --wide " + std::to_string(getpid()));
    logCallback(vmmap);

    // 1. set logging callback
    ChickenHook::Hooking::getInstance().setLoggingCallback(&logCallback);

    // 2. install a hook
    ChickenHook::Hooking::getInstance().hook((void *) &doIt, (void *) &myDot);

    // 3. check if hook works
    if (doIt()) {  // after applying the hook, we expect false as return value
        return -1;
    }

    // lets check it in a loop
    for (int i = 0; i < 10; i++) {
        if (doIt()) {  // after applying the hook, we expect false as return value
            return -1;
        }
    }

    return 0;
}