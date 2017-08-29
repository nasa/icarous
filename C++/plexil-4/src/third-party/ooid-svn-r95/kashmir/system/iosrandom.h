// iosrandom.h -- iOS random number generator

// Copyright (C) 2013 Kenneth Laskoski

/** @file iosrandom.h
    @brief iOS random number generator
    @author Copyright (C) 2013 Kenneth Laskoski
*/

#ifndef KL_IOSRANDOM_H 
#define KL_IOSRANDOM_H 

#include "../randomstream.h"
#include "../unique.h"

#include <stdexcept>

#include <Security/SecRandom.h>

namespace kashmir {
namespace system {

class iOSRandom : public randomstream<iOSRandom>, unique<iOSRandom>
{
    SecRandomRef ref;

public:
    iOSRandom(SecRandomRef ref=kSecRandomDefault) : ref(ref) {}

    void read(char *buffer, std::size_t count)
    {
        if (SecRandomCopyBytes(ref, count, reinterpret_cast<uint8_t*>(buffer)))
            throw std::runtime_error("system failed to generate random data.");
    }
};

}}

#endif
