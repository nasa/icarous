// ccsha1.h -- hash algorithm described in FIPS 180-1
//            Apple's Common Crypto implementation

// Copyright (C) 2013 Kenneth Laskoski

/** @file ccsha1.h
    @brief Apple's Common Crypto implementation of sha1 algorithm
    @author Copyright (C) 2013 Kenneth Laskoski
*/

#ifndef KL_CCSHA1_H 
#define KL_CCSHA1_H 

#include "../sha1_gen.h"
#include "../unique.h"

#include <stdexcept>

#include <CommonCrypto/CommonDigest.h>

namespace kashmir {
namespace system {

class ccsha1 : public sha1::engine<ccsha1>, unique<ccsha1>
{
    CC_SHA1_CTX ctx;

public:
    ccsha1() { CC_SHA1_Init(&ctx); }

    void update(const char *const source, std::size_t count)
    {
        CC_SHA1_Update(&ctx, reinterpret_cast<const void *const>(source), count);
    }

    sha1_t operator()()
    {
        sha1_t data;
        CC_SHA1_Final(reinterpret_cast<unsigned char *const>(&data), &ctx);
        return data;
    }
};

}}

#endif
