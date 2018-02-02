// md5_gen.h -- hash algorithm described in IETF RFC 1321

// Copyright (C) 2013 Kenneth Laskoski

/** @file md5_gen.h
    @brief hash algorithm described in IETF RFC 1321
    @author Copyright (C) 2013 Kenneth Laskoski
*/

#ifndef KL_MD5_GEN_H
#define KL_MD5_GEN_H

#include "md5.h"

namespace kashmir {
namespace md5 {

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

    md5_t operator()() { return (*self)(); }
};

} // namespace kashmir::md5
} // namespace kashmir

#endif
