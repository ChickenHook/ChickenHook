//
// Created by sascharoth on 12.06.19.
//

#include <android/log.h>
#include "JniAddressFinder.h"
#include "../MapsParser/MapsParser.h"
#include <iostream>
#include <vector>
#include <string>

JniAddressFinder::JniAddressFinder(std::string library) {
    this->library = library;
}

size_t findBytePattern(uint8_t *data, size_t size, std::vector<uint8_t> bytePattern) {
    __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Search <%lx> <%lx> ", static_cast<unsigned long>(size),
                        static_cast<unsigned long>(bytePattern.size()));

    for (size_t i = 0; i < size; i++) {
        int k;
        for (k = 0; k < bytePattern.size()
                    && i + k < size; // do not exceed the library size
             k++) {
            if (data[i + k] == bytePattern[k]) {
            } else {
                goto nofound;
            }
        }
        __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Found something");
        return i;
        nofound:;
    }

    return static_cast<size_t>(-1);
}

size_t findString(uint8_t *data, size_t size, std::string str) {
    for (size_t i = 0; i < size; i++) {
        int k;
        for (k = 0; k < str.length()
                    && i + k < size; // do not exceed the library size
             k++) {
            if (data[i + k] == str.c_str()[k]) {
                __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Found equal character <%c>",
                                    str.c_str()[k]);
            } else {
                goto nofound;
            }
        }
        __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Found something");
        return i;
        nofound:;
    }

    return static_cast<size_t>(-1);
}


size_t findMethod(uint8_t *data, size_t size, std::string methodName, std::string signature) {

    if (methodName.length() < 1) {
        return static_cast<size_t>(-1);
    }

    size_t methodStringAddr = findString(data, size, methodName);
    size_t signatureStringAddr = findString(data, size, signature);


    return static_cast<size_t>(-1);
}


procmaps_struct *findLibrary(std::string libraryName, procmaps_struct *map, int order) {
    procmaps_struct *tmp = map;
    int id = 0;
    if (order < 0) order = -1;
    while (tmp != NULL) {
        //(unsigned long) tmp->addr_start;
        if (order == id || order == -1) {
            if (strlen(tmp->pathname) != 0) {
                std::size_t found = std::string(tmp->pathname).find(libraryName);
                if (found != std::string::npos)
                    return tmp;
            }
            /*__android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Backed by:\t%s\n",
                                strlen(tmp->pathname) == 0 ? "[anonym*]" : tmp->pathname);
            __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Range:\t\t%p-%p\n", tmp->addr_start, tmp->addr_end);
            __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Length:\t\t%ld\n", tmp->length);
            __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Offset:\t\t%ld\n", tmp->offset);
            __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Permissions:\t%s\n", tmp->perm);
            __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Inode:\t\t%d\n", tmp->inode);
            __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Device:\t\t%s\n", tmp->dev);*/
        }
        if (order != -1 && id > order)
            tmp = NULL;
        else if (order == -1) {
            printf("#################################\n");
            tmp = tmp->next;
        } else tmp = tmp->next;

        id++;
    }
    return nullptr;
}

bool JniAddressFinder::load() {
    int pid = -1; //-1 to use the running process id, use pid>0 to list the map of another process
    procmaps_iterator *maps = pmparser_parse(pid);
    if (maps == NULL) {
        printf("[map]: cannot parse the memory map of %d\n", pid);
        return false;
    }

    //iterate over areas
    procmaps_struct *maps_tmp = NULL;

    while ((maps_tmp = pmparser_next(maps)) != NULL) {
        if ((this->proc_entry = findLibrary(this->library, maps_tmp, 0)) != nullptr) {
            break;
        }
        //pmparser_print(maps_tmp, 0);
        //printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }

    //mandatory: should free the list
    pmparser_free(maps);
    return this->proc_entry != nullptr;
};

std::string JniAddressFinder::getPath() {
    return this->proc_entry->pathname;
}

void *JniAddressFinder::getLibraryAddress() {
    return this->proc_entry->addr_start;
}

size_t JniAddressFinder::getAddressOfJavaFunction(std::vector<uint8_t> bytePattern) {
    size_t libraryLen = (unsigned long) this->proc_entry->addr_end - (unsigned long) this->proc_entry->addr_start;
    auto *libraryAddress = (uint8_t *) this->proc_entry->addr_start;
    size_t addr = findBytePattern(libraryAddress, libraryLen, bytePattern);
    if (addr == (size_t) -1) {
        __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Could not find address of byte pattern");
    } else {

        __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Found address of byte pattern at <%p>",
                            (void *) addr);
    }
    return addr;
}


std::uint8_t JniAddressFinder::getAddressOfJavaFunction(std::string functionName, std::string signature) {


    __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Backed by:\t%s\n",
                        strlen(this->proc_entry->pathname) == 0 ? "[anonym*]" : this->proc_entry->pathname);
    __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Range:\t\t%p-%p\n", this->proc_entry->addr_start,
                        this->proc_entry->addr_end);
    __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Length:\t\t%ld\n", this->proc_entry->length);
    __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Offset:\t\t%ld\n", this->proc_entry->offset);
    __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Permissions:\t%s\n", this->proc_entry->perm);
    __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Inode:\t\t%d\n", this->proc_entry->inode);
    __android_log_print(ANDROID_LOG_DEBUG, "MapsParser", "Device:\t\t%s\n", this->proc_entry->dev);


    size_t libraryLen = this->proc_entry->length;
    auto *libraryAddress = (uint8_t *) this->proc_entry->addr_start;

    char *addressOfStringAddress = reinterpret_cast<char *>(libraryAddress + 0x767f7);

    __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Found char <%c>",
                        *addressOfStringAddress
    );

    size_t methodAddr = findMethod(libraryAddress, libraryLen, functionName, signature);
    if (methodAddr == (size_t) -1) {
        __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Could not find address of function <%s>",
                            functionName.c_str());
    } else {

        __android_log_print(ANDROID_LOG_ERROR, "JniAddressFinder", "Found address of function <%s> at <%p>",
                            functionName.c_str(), (void *) methodAddr);
    }
    return methodAddr;
};