// abstractrandom.h -- abstract random number generator

// Copyright (C) 2013 Kenneth Laskoski

/** @file abstractrandom.h
    @brief abstract random number generator
    @author Copyright (C) 2013 Kenneth Laskoski
*/

#ifndef KL_ABSTRACTRANDOM_H 
#define KL_ABSTRACTRANDOM_H 

#include "../randomstream.h"

namespace kashmir {
namespace system {

class AbstractRandom : public randomstream<AbstractRandom>
{
public:
    virtual ~AbstractRandom() {}

    virtual void read(char *buffer, std::size_t count) = 0;
};

}}

#endif
