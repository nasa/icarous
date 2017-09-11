// devrandom.h -- UNIX random number generator

// Copyright (C) 2008 Kenneth Laskoski

/** @file devrandom.h
    @brief UNIX random number generator
    @author Copyright (C) 2008 Kenneth Laskoski
*/

#ifndef KL_DEVRANDOM_H 
#define KL_DEVRANDOM_H 

#include "../randomstream.h"
#include "../unique.h"

#include <fstream>
#include <stdexcept>

namespace kashmir {
namespace system {

class DevRandom : public randomstream<DevRandom>, unique<DevRandom>
{
    std::ifstream file;

public:
    DevRandom() : file("/dev/random", std::ios::binary)
    {
        if (!file)
            throw std::runtime_error("failed to open random device.");
    }

    void read(char* buffer, std::size_t count)
    {
        file.read(buffer, count);
    }
};

}}

#endif
