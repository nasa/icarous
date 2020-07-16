// md5.h -- hash algorithm data product described in IETF RFC 1321

// Copyright (C) 2010 Kenneth Laskoski

/** @file md5.h
    @brief hash algorithm data product described in IETF RFC 1321
    @author Copyright (C) 2010 Kenneth Laskoski
*/

#ifndef KL_MD5_H
#define KL_MD5_H 

#include "array.h"

#include <istream>
#include <ostream>
#include <sstream>

#include <stdexcept>

#include "iostate.h"

namespace kashmir {
namespace md5 {

/** @class md5_t
    @brief This class implements the data product of the hash algorithm described in
    
    - IETF RFC 1321 - available at http://tools.ietf.org/html/rfc1321

    This documents the code below.
*/

// an MD5 is a string of 16 octets (128 bits)
// we use an unpacked representation, value_type may be larger than 8 bits,
// in which case every input operation must assert data[i] < 256 for i < 16
// note that even char may be more than 8 bits in some particular platform

typedef unsigned char value_type;
typedef std::size_t size_type;

const size_type size = 16, string_size = 32;

class md5_t
{
    typedef array<value_type, size> data_type;
    data_type data;

public:
    // we keep data uninitialized to stress concreteness
    md5_t() {}
    ~md5_t() {}

    // trivial copy and assignment
    md5_t(const md5_t& rhs) : data(rhs.data) {}
    md5_t& operator=(const md5_t& rhs)
    {
        data = rhs.data;
        return *this;
    }

    // OK, now we bow to convenience
    // initialization from C string
    explicit md5_t(const char* string)
    {
        std::stringstream stream(string);
        get(stream);
    }

    // test for nil value
    bool is_nil() const
    {
        for (size_type i = 0; i < size; ++i)
            if (data[i])
                return false;
        return true;
    }

    // safe bool idiom
    typedef data_type md5_t::*bool_type; 
    operator bool_type() const
    {
        return is_nil() ? 0 : &md5_t::data;
    }

    // only equality makes sense here
    bool operator==(const md5_t& rhs) const
    {
        return data == rhs.data;
    }

    // stream operators
    template<class char_t, class char_traits>
    std::basic_ostream<char_t, char_traits>& put(std::basic_ostream<char_t, char_traits>& os) const;

    template<class char_t, class char_traits>
    std::basic_istream<char_t, char_traits>& get(std::basic_istream<char_t, char_traits>& is);
};

// only equality makes sense here
inline bool operator!=(const md5_t& lhs, const md5_t& rhs) { return !(lhs == rhs); }

template<class char_t, class char_traits>
std::basic_ostream<char_t, char_traits>& md5_t::put(std::basic_ostream<char_t, char_traits>& os) const
{
    if (!os.good())
        return os;

    const typename std::basic_ostream<char_t, char_traits>::sentry ok(os);
    if (ok)
    {
        ios_flags_saver flags(os);
        basic_ios_fill_saver<char_t, char_traits> fill(os);

        const std::streamsize width = os.width(0);
        const std::streamsize mysize = string_size;

        // right padding
        if (flags.value() & (std::ios_base::right | std::ios_base::internal))
            for (std::streamsize i = width; i > mysize; --i)
                os << fill.value();

        os << std::hex;
        os.fill(os.widen('0'));

        for (size_t i = 0; i < size; ++i)
        {
            os.width(2);
            os << static_cast<unsigned>(data[i]);
        }

        // left padding
        if (flags.value() & std::ios_base::left)
            for (std::streamsize i = width; i > mysize; --i)
                os << fill.value();
    }

    return os;
}

template<class char_t, class char_traits>
std::basic_istream<char_t, char_traits>& md5_t::get(std::basic_istream<char_t, char_traits>& is)
{
    if (!is.good())
        return is;

    const typename std::basic_istream<char_t, char_traits>::sentry ok(is);
    if (ok)
    {
        char_t hexdigits[16];
        char_t* const npos = hexdigits+16;

        typedef std::ctype<char_t> facet_t;
        const facet_t& facet = std::use_facet<facet_t>(is.getloc());

        const char* tmp = "0123456789abcdef";
        facet.widen(tmp, tmp+16, hexdigits);

        char_t c;
        char_t* f;
        for (size_t i = 0; i < size; ++i)
        {
            is >> c;
            c = facet.tolower(c);

            f = std::find(hexdigits, npos, c);
            if (f == npos)
            {
                is.setstate(std::ios_base::failbit);
                break;
            }

            data[i] = static_cast<value_type>(std::distance(hexdigits, f));

            is >> c;
            c = facet.tolower(c);

            f = std::find(hexdigits, npos, c);
            if (f == npos)
            {
                is.setstate(std::ios_base::failbit);
                break;
            }

            data[i] <<= 4;
            data[i] |= static_cast<value_type>(std::distance(hexdigits, f));
        }

        if (!is)
            throw std::runtime_error("failed to extract valid md5 from stream.");
    }

    return is;
}

template<class char_t, class char_traits>
inline std::basic_ostream<char_t, char_traits>& operator<<(std::basic_ostream<char_t, char_traits>& os, const md5_t& md5)
{
    return md5.put(os);
}

template<class char_t, class char_traits>
inline std::basic_istream<char_t, char_traits>& operator>>(std::basic_istream<char_t, char_traits>& is, md5_t& md5)
{
    return md5.get(is);
}

} // namespace kashmir::md5

using md5::md5_t;

} // namespace kashmir

#endif
