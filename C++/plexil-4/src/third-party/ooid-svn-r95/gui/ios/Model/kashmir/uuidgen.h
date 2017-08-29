// uuid_gen.h -- universally unique ID - as defined by ISO/IEC 9834-8:2005

// Copyright (C) 2008 Kenneth Laskoski

/** @file uuid_gen.h
    @brief universally unique ID - as defined by ISO/IEC 9834-8:2005
    @author Copyright (C) 2008 Kenneth Laskoski
*/

#ifndef KL_UUIDGEN_H
#define KL_UUIDGEN_H

#include "uuid.h"
#include "md5gen.h"
#include "sha1gen.h"
#include "randomstream.h"

namespace kashmir {
namespace uuid {

template<class crtp_impl>
randomstream<crtp_impl>& operator>>(randomstream<crtp_impl>& is, uuid_t& uuid)
{
    // get random bytes
    // we take advantage of our representation
    char *data = reinterpret_cast<char*>(&uuid);
    is.read(data, size);

    // a more general solution would be
//    input_iterator<is> it;
//    std::copy(it, it+size, data.begin());

    // if uuid_t::value_type is larger than 8 bits, we need
    // to maintain the invariant data[i] < 256 for i < 16
    // Example (which may impact randomness):
//    for (size_t i = 0; i < size; ++i)
//        data[i] &= 0xff;

    // set variant
    // should be 0b10xxxxxx
    data[8] &= 0xbf;   // 0b10111111
    data[8] |= 0x80;   // 0b10000000

    // set version
    // should be 0b0100xxxx
    data[6] &= 0x4f;   // 0b01001111
    data[6] |= 0x40;   // 0b01000000

    return is;
}

template<class crtp_impl>
uuid_t uuid_gen(randomstream<crtp_impl>& is)
{
    uuid_t uuid;
    is >> uuid;
    return uuid;
}

template<class crtp_impl>
uuid_t uuid_gen(md5::engine<crtp_impl>& md5engine, const uuid_t& nameSpace, const std::string& name)
{
    md5engine.update(reinterpret_cast<const char *const>(&nameSpace), 16);
    md5engine.update(name.c_str(), name.size());

    kashmir::md5_t md5 = md5engine();

    kashmir::uuid_t& uuid = *(reinterpret_cast<kashmir::uuid_t*>(&md5));
    unsigned char *const data = reinterpret_cast<unsigned char *const>(&uuid);

    // set variant
    // should be 0b10xxxxxx
    data[8] &= 0xbf;   // 0b10111111
    data[8] |= 0x80;   // 0b10000000

    // set version
    // should be 0b0011xxxx
    data[6] &= 0x3f;   // 0b00111111
    data[6] |= 0x30;   // 0b00110000

    return uuid;
}

template<class crtp_impl>
uuid_t uuid_gen(sha1::engine<crtp_impl>& sha1engine, const uuid_t& nameSpace, const std::string& name)
{
    sha1engine.update(reinterpret_cast<const char *const>(&nameSpace), 16);
    sha1engine.update(name.c_str(), name.size());

    kashmir::sha1_t sha1 = sha1engine();

    kashmir::uuid_t& uuid = *(reinterpret_cast<kashmir::uuid_t*>(&sha1));
    unsigned char *const data = reinterpret_cast<unsigned char *const>(&uuid);

    // set variant
    // should be 0b10xxxxxx
    data[8] &= 0xbf;   // 0b10111111
    data[8] |= 0x80;   // 0b10000000

    // set version
    // should be 0b0101xxxx
    data[6] &= 0x5f;   // 0b01011111
    data[6] |= 0x50;   // 0b01010000

    return uuid;
}

} // namespace kashmir::uuid
} // namespace kashmir

#endif
