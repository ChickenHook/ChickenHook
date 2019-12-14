
#pragma once
namespace ChickenHook {

/**
 *
 * @return -1 on failure, otherwise next possible instruction offset
 */
    static int nextInstOff(void *fun, size_t hookLen, char *) {
        if (fun == nullptr) {
            // TODO error log
            return -1;
        }
        return (int) hookLen;
    }
}