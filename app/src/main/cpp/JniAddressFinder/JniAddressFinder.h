//
// Created by sascharoth on 12.06.19.
//

#ifndef VMCRACKER_JNIADDRESSFINDER_H
#define VMCRACKER_JNIADDRESSFINDER_H


#include <string>
#include "../MapsParser/MapsParser.h"

class JniAddressFinder {
private:
    std::string library;
    procmaps_struct *proc_entry;

public:
    JniAddressFinder(std::string library);

    bool load();

    std::uint8_t getAddressOfJavaFunction(std::string functionName, std::string signature);

    std::string getPath();

    void * getLibraryAddress();


    size_t getAddressOfJavaFunction(std::vector<uint8_t> bytePattern);
};


#endif //VMCRACKER_JNIADDRESSFINDER_H
