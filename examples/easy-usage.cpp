
#include "chickenHook/chickenhook.h"


int my_open(const char *__path, int __flags,
            ...) { // the function signature must match the original function !!
    printf("open called [-] %s", __path);

    int res = -1;
    Trampoline trampoline;
    if (ChickenHook::getInstance().getTrampolineByAddr((void *) &open, trampoline)) {
        printf("call original function");

        trampoline.copyOriginal();
        res = open(__path, __flags);
        trampoline.reinstall();
        return res;
    } else {
        printf("cannot call original function");

    }
    return -1;
}


int main(int argc, char *argv[]) {
    ChickenHook::getInstance().inject((void *) &open, (void *) &my_open);
    open("/proc/self/maps", "r");
    return 0;
}