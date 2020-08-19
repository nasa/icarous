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

#ifndef MOFN_H_
#define MOFN_H_

#include <deque>
#include <string>

namespace larcfm {

class MofN {
public:

  /*
   * Set M of N with a given initial value
   */
  void setMofN(int m, int n, int val=-1);

  /*
   * Creates an M of N object, with a given initial value
   */
  MofN(int m, int n, int val=-1);

  /*
   * Creates an empty M of N object, with no parameter initialization.
   * Without further use of setMofN on this object, it's considered invalid and
   * doesn't perform M of N logic.
   */
  MofN();

  /*
   * Creates a copy of M of N object
   */
  MofN(const MofN& mofn);

  /*
   * Reset M of N object with a given initial value
   */
  void reset(int val=-1);

  /*
   * Returns true if this object is able to perform M of N logic.
   */
  bool isValid() const;

  /*
   * Return M of N value for a given value.
   * Counts maximum occurrences of value assuming that that value represents all the
   * values val such that 0 <= val <= value. Return -1 if none of the values satisfies
   * M of N logic.
   */
  int m_of_n(int value);

  virtual ~MofN() {};

  bool sameAs(const MofN& mofn) const;

  std::string toString() const;

private:
  int m_;
  int n_;
  int    max_;
  std::deque<int> queue_;

};

} /* namespace larcfm */

#endif /* MOFN_H_ */
