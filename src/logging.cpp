//
// Created by Johny George Malayil on 01.11.19.
//

#include "chickenHook/logging.h"

namespace ChickenHook {

    // static function pointer to logging callback
    static void (*logFunction)(const std::string logtext);

    /*
     * Calculate number of characters that is required to apply pargs to format string.
     *
     * \return The number of characters that would be required by any printf function to format the string, not counting
     * 		the terminating null character. If an encoding error occurs, a negative number is returned.
     */
    int _vscprintf(const char *format, va_list pargs) {
        va_list argcopy;
        va_copy(argcopy, pargs);
        int retval = vsnprintf(NULL, 0, format, argcopy);
        va_end(argcopy);
        return retval;
    }

    /*
     * convert CFormatString and parameters into formatted std::string.
     */
    int vaListToString(const char *CFormatString, va_list argptr, std::string &result) {
        // calculate the required buffer length of without actually writing
        int len = _vscprintf(CFormatString, argptr);
        if (len < 0) {
            return len;
        }

        // write into an allocated buffer (add 1 for null termination) and create a string from that result
        char *buffer = new char[len + 1];
        len = vsnprintf(buffer, len + 1, CFormatString, argptr);
        result = std::string(buffer);
        delete[] buffer;
        return len;
    }

    /**
     * Use this to log into the provided logging callback
     */
    void log(const char *CFormatString, ...) {

        if (CFormatString == nullptr) {
            return;
        }

        if (logFunction == NULL) {
            return;
        }

        va_list argptr;
        va_start(argptr, CFormatString);
        std::string res;
        int len = vaListToString(CFormatString, argptr, res);
        if (len > 0) {
            (*logFunction)(res);
        }
        va_end(argptr);

    }

    /*
    * Set the logging callback
    */
    void setLogFunction(void (*logCallback)(const std::string)) {
        logFunction = logCallback;
    }
}



