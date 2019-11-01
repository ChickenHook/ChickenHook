#pragma once

#include <stdarg.h>
#include <iostream>
#include <vector>

namespace ChickenHook {

    /**
     * Use this to log into the provided logging callback
     */
    void log(const char *CFormatString, ...);

    /**
     * Set the logging callback
     */
    void setLogFunction(void (*logCallback)(const std::string logtext));

}