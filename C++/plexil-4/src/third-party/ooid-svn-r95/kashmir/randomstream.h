// randomstream.h -- random number stream

// Copyright (C) 2008 Kenneth Laskoski

/** @file randomstream.h
    @brief random number stream
    @author Copyright (C) 2008 Kenneth Laskoski
*/

#ifndef KL_RANDOMSTREAM_H
#define KL_RANDOMSTREAM_H

#include <cstddef>

namespace kashmir {

// CRTP stands for curiously recurring template pattern
template<class crtp_impl>
class randomstream
{
    crtp_impl *const self;

public:
    randomstream<crtp_impl>() : self(static_cast<crtp_impl*>(this)) {}

    void read(char *buffer, std::size_t count)
    {
        self->read(buffer, count);
    }

    randomstream<crtp_impl>& operator>>(char& c) { read(&c, 1); return *this; }
    randomstream<crtp_impl>& operator>>(signed char& c) { read(reinterpret_cast<char*>(&c), 1); return *this; }
    randomstream<crtp_impl>& operator>>(unsigned char& c) { read(reinterpret_cast<char*>(&c), 1); return *this; }

    randomstream<crtp_impl>& operator>>(int& n) { read(reinterpret_cast<char*>(&n), sizeof(int)); return *this; }
    randomstream<crtp_impl>& operator>>(long& n) { read(reinterpret_cast<char*>(&n), sizeof(long)); return *this; }
    randomstream<crtp_impl>& operator>>(short& n) { read(reinterpret_cast<char*>(&n), sizeof(short)); return *this; }

    randomstream<crtp_impl>& operator>>(unsigned int& u) { read(reinterpret_cast<char*>(&u), sizeof(unsigned int)); return *this; }
    randomstream<crtp_impl>& operator>>(unsigned long& u) { read(reinterpret_cast<char*>(&u), sizeof(unsigned long)); return *this; }
    randomstream<crtp_impl>& operator>>(unsigned short& u) { read(reinterpret_cast<char*>(&u), sizeof(unsigned short)); return *this; }

    randomstream<crtp_impl>& operator>>(float& f) { read(reinterpret_cast<char*>(&f), sizeof(float)); return *this; }
    randomstream<crtp_impl>& operator>>(double& f) { read(reinterpret_cast<char*>(&f), sizeof(double)); return *this; }
    randomstream<crtp_impl>& operator>>(long double& f) { read(reinterpret_cast<char*>(&f), sizeof(long double)); return *this; }

    randomstream<crtp_impl>& operator>>(bool& b) { read(reinterpret_cast<char*>(&b), sizeof(bool)); b &= 1; return *this; }
    randomstream<crtp_impl>& operator>>(void*& p) { read(reinterpret_cast<char*>(&p), sizeof(void*)); return *this; }
};

} // namespace kashmir

#endif
