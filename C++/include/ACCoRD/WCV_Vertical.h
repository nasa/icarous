/*
 * Copyright (c) 2012-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef WCV_VERTICAL_H_
#define WCV_VERTICAL_H_

#include "Interval.h"

namespace larcfm {

class WCV_Vertical {


public:

  virtual ~WCV_Vertical() = 0;
  virtual bool vertical_WCV(double ZTHR, double TTHR, double sz, double vz) const = 0;
  virtual Interval vertical_WCV_interval(double ZTHR, double TTHR, double B, double T, double sz, double vz) const = 0;

};

inline WCV_Vertical::~WCV_Vertical(){}

}
#endif
