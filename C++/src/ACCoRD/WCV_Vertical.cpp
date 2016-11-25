/*
 * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "WCV_Vertical.h"
#include "Vertical.h"

namespace larcfm {

WCV_Vertical::WCV_Vertical() {
  time_in = time_out = -1.0;
}

// Vertical Not Well Clear Violation
// ZTHR and TTHR are altitude and time thresholds
bool WCV_Vertical::vertical_WCV(double ZTHR, double TCOA, double sz, double vz) {
  return std::abs(sz) <= ZTHR ||
      (vz != 0 && sz*vz <= 0 && Vertical::time_coalt(sz,vz) <= TCOA);
}

void WCV_Vertical::vertical_WCV_interval(double ZTHR, double TCOA, double B, double T, double sz, double vz) {
  time_in = B;
  time_out = T;
  if (Util::almost_equals(vz,0) && std::abs(sz) <= ZTHR) // [CAM] Changed from == to almost_equals to mitigate numerical problems
    return;
  if (Util::almost_equals(vz,0)) { // [CAM] Changed from == to almost_equals to mitigate numerical problems
    time_in = T;
    time_out = B;
    return;
  }
  double act_H = std::max(ZTHR,std::abs(vz)*TCOA);
  double tentry = Vertical::Theta_H(sz,vz,-1,act_H);
  double texit = Vertical::Theta_H(sz,vz,1,ZTHR);
  if (T < tentry || texit < B) {
    time_in = T;
    time_out = B;
    return;
  }
  time_in = std::max(B,tentry);
  time_out = std::min(T,texit);
}

}
