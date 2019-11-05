
#include <iostream>
#include "chickenHook/hooking.h"
#include "chickenHook/trampoline.h"
#include "chickenHook/logging.h"


bool myFread() {
    std::cout << "CHICKEN (fread) HOOKED! RIPPING FEATHERS!" << std::endl;
    return true;
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
            if (fgets(buffer, max_buffer, stream) != nullptr) data.append(buffer);
        pclose(stream);
    }
    return data;
}


int main(int argc, char *arv[]) {
    std::cout << "WELCOME TO MAC CHICKENHOOK TESTS" << std::endl;

//    For debugging purposes
//    std::string vmmap = GetStdoutFromCommand("vmmap --wide " + std::to_string(getpid()));
//    logCallback(vmmap);

    // 1. set logging callback
    ChickenHook::Hooking::getInstance().setLoggingCallback(&logCallback);

    // 2. install a hook on C fread()
    ChickenHook::Hooking::getInstance().hook((void *) &fread, (void *) &myFread);


    unsigned char buffer[10];
    FILE *ptr;

    ptr = fopen("test.bin","rb");

    // now call read to see if it was hooked successfully
    if(!fread(buffer,sizeof(buffer),1,ptr)){
        return -1;
    }

    return 0;
}
