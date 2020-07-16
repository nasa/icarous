// sha1_gen.h -- hash algorithm described in FIPS 180-1

// Copyright (C) 2013 Kenneth Laskoski

/** @file sha1_gen.h
    @brief hash algorithm described in FIPS 180-1
    @author Copyright (C) 2013 Kenneth Laskoski
*/

#ifndef KL_SHA1_GEN_H
#define KL_SHA1_GEN_H

#include "sha1.h"

namespace kashmir {
namespace sha1 {

template<class crtp_impl>
class engine
{
    crtp_impl *const self;

public:
    engine<crtp_impl>() : self(static_cast<crtp_impl*>(this)) {}

    void update(const char *const source, std::size_t count)
    {
        self->update(source, count);
    }

    sha1_t operator()() { return (*self)(); }
};

} // namespace kashmir::sha1
} // namespace kashmir

#endif
