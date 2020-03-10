/*
 * Copyright (c) 2012-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_TCOA_H_
#define WCV_TCOA_H_

#include "WCV_Vertical.h"

namespace larcfm {
class WCV_TCOA : public WCV_Vertical {


public:

  virtual bool vertical_WCV(double ZTHR, double TCOA, double sz, double vz) const;
  virtual Interval vertical_WCV_interval(double ZTHR, double TCOA, double B, double T, double sz, double vz) const;
  virtual ~WCV_TCOA() {};

  /**
   * Returns a deep copy of this WCV_TCOA object, including any results that have been calculated.
   */
  WCV_Vertical* copy() const;

};
}
#endif
