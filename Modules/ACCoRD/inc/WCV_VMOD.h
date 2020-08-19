/*
 * Copyright (c) 2012-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_VMOD_H_
#define WCV_VMOD_H_

#include "WCV_Vertical.h"

namespace larcfm {

/* Non-Hazard Zone VMOD concept */

class WCV_VMOD : public WCV_Vertical {

public:

  virtual bool vertical_WCV(double ZTHR, double T_star, double sz, double vz) const;
  virtual Interval vertical_WCV_interval(double ZTHR, double T_star, double B, double T, double sz, double vz) const;
  virtual ~WCV_VMOD() {};

  /**
   * Returns a deep copy of this WCV_VMOD object, including any results that have been calculated.
   */
  WCV_Vertical* copy() const;

};
}
#endif
