/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "WCV_TCPA.h"
#include "WCV_TCOA.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Horizontal.h"
#include "WCVTable.h"
#include "LossData.h"
#include "format.h"
#include "string_util.h"

namespace larcfm {

/** Constructor that uses the default TCAS tables. */
WCV_TCPA::WCV_TCPA() {
  wcv_vertical = new WCV_TCOA();
  id = "";
}

/** Constructor that specifies a particular instance of the TCAS tables. */
WCV_TCPA::WCV_TCPA(const WCVTable& tab) {
  wcv_vertical = new WCV_TCOA();
  table.copyValues(tab);
  id = "";
}

double WCV_TCPA::horizontal_tvar(const Vect2& s, const Vect2& v) const {
  // Time variable is Modified Tau
  double TCPA = -1;
  double sdotv = s.dot(v);
  if (sdotv < 0)
    return (Util::sq(table.getDTHR())-s.sqv())/sdotv;
  return TCPA;
}

LossData WCV_TCPA::horizontal_WCV_interval(double T, const Vect2& s, const Vect2& v) const {
  double time_in = T;
  double time_out = 0;
  double sqs = s.sqv();
  double sqv = v.sqv();
  double sdotv = s.dot(v);
  double sqD = Util::sq(table.getDTHR());
  if (Util::almost_equals(sqv,0) && sqs <= sqD) { // [CAM] Changed from == to almost_equals to mitigate numerical problems
    time_in = 0;
    time_out = T;
    return LossData(time_in,time_out);
  }
  if (Util::almost_equals(sqv,0)) // [CAM] Changed from == to almost_equals to mitigate numerical problems
    return LossData(time_in,time_out);
  if (sqs <= sqD) {
    time_in = 0;
    time_out = std::min(T,Horizontal::Theta_D(s,v,1,table.getDTHR()));
    return LossData(time_in,time_out);
  }
  if (sdotv > 0)
    return LossData(time_in,time_out);
  double tcpa = Horizontal::tcpa(s,v);
  if (v.ScalAdd(tcpa, s).norm() > table.getDTHR())
    return LossData(time_in,time_out);
  double Delta = Horizontal::Delta(s,v,table.getDTHR());
  if (Delta < 0 && tcpa - table.getTTHR() > T)
    return LossData(time_in,time_out);
  if (Delta < 0) {
    time_in = std::max(0.0,tcpa-table.getTTHR());
    time_out = std::min(T,tcpa);
    return LossData(time_in,time_out);
  }
  double tmin = std::min(Horizontal::Theta_D(s,v,-1,table.getDTHR()),tcpa-table.getTTHR());
  if (tmin > T)
    return LossData(time_in,time_out);
  time_in = std::max(0.0,tmin);
  time_out = std::min(T,Horizontal::Theta_D(s,v,1,table.getDTHR()));
  return LossData(time_in,time_out);
}

Detection3D* WCV_TCPA::make() const {
  return new WCV_TCPA();
}

/**
 * Returns a deep copy of this WCV_TCPA object, including any results that have been calculated.
 */
Detection3D* WCV_TCPA::copy() const {
  WCV_TCPA* ret = new WCV_TCPA();
  ret->table.copyValues(table);
  ret->id = id;
  return ret;
}

std::string WCV_TCPA::getSimpleClassName() const {
  return "WCV_TCPA";
}

bool WCV_TCPA::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(), cd->getCanonicalClassName())) {
    return containsTable((WCV_tvar*)cd);
  }
  return false;
}

}

