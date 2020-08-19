/*
 * Implementation of M of N filtering logic
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "MofN.h"
#include "Util.h"
#include "format.h"

#include <vector>

namespace larcfm {

/*
 * Set M of N with a given initial value
 */
void MofN::setMofN(int m, int n, int val) {
  m_ = m;
  n_ = n;
  reset(val);
}

/*
 * Creates an M of N object, with a given initial value
 */
MofN::MofN(int m, int n, int val) {
  setMofN(m,n,val);
}

/*
 * Creates an empty M of N object, with no parameter initialization.
 * Without further use of setMofN on this object, it's considered invalid and
 * doesn't perform M of N logic.
 */
MofN::MofN() {
  setMofN(0,0,-1);
}

/*
 * Creates a copy of M of N object
 */
MofN::MofN(const MofN& mofn) : m_(mofn.m_), n_(mofn.n_), max_(mofn.max_),
    queue_(mofn.queue_) {}

/*
 * Reset M of N object with a given initial value
 */
void MofN::reset(int val) {
  queue_.clear();
  max_ = val;
  for (int i=0;i<n_;++i) {
    queue_.push_back(i < m_ ? val : -1);
  }
}

/*
 * Returns true if this object is able to perform M of N logic.
 */
bool MofN::isValid() const {
  return !queue_.empty() && m_ > 0 && m_ <= n_;
}

/*
 * Return M of N value for a given value.
 * Counts maximum occurrences of value assuming that that value represents all the
 * values val such that 0 <= val <= value. Return -1 if none of the values satisfies
 * M of N logic.
 */
int MofN::m_of_n(int value) {
  if (!isValid()) {
    return value;
  }
  if (value > max_) {
    max_ = value;
  }
  queue_.pop_front();
  queue_.push_back(value);
  if (max_ < 0) {
    return max_;
  }
  std::vector<int> count(max_+1,0);
  for (std::deque<int>::const_iterator val_ptr = queue_.begin(); val_ptr != queue_.end(); ++val_ptr) {
    for (int i=*val_ptr;i >= 0;--i) {
      count[i]++;
    }
  }
  for (int i=max_; i >= 0; --i) {
    if (count[i] >= m_) {
      return i;
    }
  }
  return -1;
}

bool MofN::sameAs(const MofN& mofn) const {
  if (max_ != mofn.max_  && queue_.size() != mofn.queue_.size()) {
      return false;
  }
  std::deque<int>::const_iterator it1_ptr = queue_.begin();
  std::deque<int>::const_iterator it2_ptr = mofn.queue_.begin();
  while (it1_ptr != queue_.end() && it2_ptr != mofn.queue_.end()) {
      if (*it1_ptr != *it2_ptr) {
          return false;
      }
      ++it1_ptr;
      ++it2_ptr;
  }
  return true;
}

std::string MofN::toString() const {
  std::string s=Fmi(m_)+" of "+Fmi(n_)+": [";
  bool comma = false;
  for (std::deque<int>::const_iterator val_ptr = queue_.begin(); val_ptr != queue_.end(); ++val_ptr) {
    if (comma) {
      s+=",";
    } else {
      comma = true;
    }
    s += Fmi(*val_ptr);
  }
  s+="]";
  return s;
}


} /* namespace larcfm */
