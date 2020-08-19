/*
 * Copyright (c) 2015-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_HZ_H_
#define WCV_HZ_H_

/* Horizontal Well Clear Volume concept based on Modified TAU,
 * and, in the vertical dimension, VMOD
 */

#include "WCV_TAUMOD.h"

namespace larcfm {
class WCV_HZ : public WCV_TAUMOD {


public:

  /** Constructor that a default instance of the WCV tables. */
  WCV_HZ();

  /** Constructor that specifies a particular instance of the WCV tables. */
  explicit WCV_HZ(const WCVTable& tab);

  virtual ~WCV_HZ() {};

  /**
   * @return one static WCV_HZ
   */
  static const WCV_HZ& A_WCV_HZ();

  Detection3D* make() const;

  /**
   * Returns a deep copy of this WCV_HZ object, including any results that have been calculated.
   */
  Detection3D* copy() const;

  std::string getSimpleClassName() const;

  bool contains(const Detection3D* cd) const;

};
}
#endif
