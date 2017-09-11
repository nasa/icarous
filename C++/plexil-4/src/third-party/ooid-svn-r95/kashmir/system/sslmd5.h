// sslmd5.h -- hash algorithm described in IETF RFC 1321
//             Open SSL implementation

// Copyright (C) 2013 Kenneth Laskoski

/** @file sslmd5.h
    @brief Open SSL implementation of md5 algorithm
    @author Copyright (C) 2013 Kenneth Laskoski
*/

#ifndef KL_SSLMD5_H 
#define KL_SSLMD5_H 

#include "../md5_gen.h"
#include "../unique.h"

#include <stdexcept>

#include <openssl/md5.h>

namespace kashmir {
namespace system {

class sslmd5 : public md5::engine<sslmd5>, unique<sslmd5>
{
    MD5_CTX ctx;

public:
    sslmd5() { MD5_Init(&ctx); }

    void update(const char *const source, std::size_t count)
    {
        MD5_Update(&ctx, reinterpret_cast<const void *const>(source), count);
    }

    md5_t operator()()
    {
        md5_t data;
        MD5_Final(reinterpret_cast<unsigned char *const>(&data), &ctx);
        return data;
    }
};

}}

#endif
