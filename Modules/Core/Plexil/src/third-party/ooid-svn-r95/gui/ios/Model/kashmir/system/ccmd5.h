// ccmd5.h -- hash algorithm described in IETF RFC 1321
//            Apple's Common Crypto implementation

// Copyright (C) 2013 Kenneth Laskoski

/** @file ccmd5.h
    @brief Apple's Common Crypto implementation of md5 algorithm
    @author Copyright (C) 2013 Kenneth Laskoski
*/

#ifndef KL_CCMD5_H 
#define KL_CCMD5_H 

#include "../md5gen.h"
#include "../unique.h"

#include <stdexcept>

#include <CommonCrypto/CommonDigest.h>

namespace kashmir {
namespace system {

class ccmd5 : public md5::engine<ccmd5>, unique<ccmd5>
{
    CC_MD5_CTX ctx;

public:
    ccmd5() { CC_MD5_Init(&ctx); }

    void update(const char* const source, std::size_t count)
    {
        CC_MD5_Update(&ctx, reinterpret_cast<const void *const>(source), count);
    }

    md5_t operator()()
    {
        md5_t data;
        CC_MD5_Final(reinterpret_cast<unsigned char *const>(&data), &ctx);
        return data;
    }
};

}}

#endif
