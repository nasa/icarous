// unique.h -- disable copy construction and assignment

// Copyright (C) 2008 Kenneth Laskoski

/** @file unique.h
    @brief disable copy construction and assignment
    @author Copyright (C) 2008 Kenneth Laskoski
*/

#ifndef KL_UNIQUE_H
#define KL_UNIQUE_H

namespace kashmir {
namespace unique_ADL_fence {

// CRTP allows better empty base optimization
template<class CRTP>
class unique
{
protected:
    unique() {}
    ~unique() {}

private:
    unique(const unique&);
    unique& operator=(const unique&);
};

}

using namespace unique_ADL_fence;

}

#endif
