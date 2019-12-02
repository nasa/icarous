/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "WCV_TEP.h"
#include "WCV_TAUMOD.h"
#include "WCV_TCPA.h"
#include "WCV_TCOA.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Horizontal.h"
#include "WCVTable.h"
#include "LossData.h"
#include "Util.h"
#include "format.h"
#include "string_util.h"

namespace larcfm {

/** Constructor that uses the default TCAS tables. */
WCV_TEP::WCV_TEP() {
  wcv_vertical = new WCV_TCOA();
  id = "";
}

/** Constructor that specifies a particular instance of the TCAS tables. */
WCV_TEP::WCV_TEP(const WCVTable& tab) {
  wcv_vertical = new WCV_TCOA();
  table = tab;
  id = "";
}

double WCV_TEP::horizontal_tvar(const Vect2& s, const Vect2& v) const {
  // Time variable is Modified Tau
  double TEP = -1;
  double sdotv = s.dot(v);
  if (sdotv < 0)
    return (Util::sq(table.getDTHR())-s.sqv())/sdotv;
  return TEP;
}

LossData WCV_TEP::horizontal_WCV_interval(double T, const Vect2& s, const Vect2& v) const {
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
    time_out = Util::min(T,Horizontal::Theta_D(s,v,1,table.getDTHR()));
    return LossData(time_in,time_out);
  }
  if (sdotv > 0 || Horizontal::Delta(s,v,table.getDTHR()) < 0)
    return LossData(time_in,time_out);
  double tep = Horizontal::Theta_D(s,v,-1,table.getDTHR());
  if (tep-table.getTTHR() > T)
    return LossData(time_in,time_out);
  time_in = Util::max(0.0,tep-table.getTTHR());
  time_out = Util::min(T,Horizontal::Theta_D(s,v,1,table.getDTHR()));
  return LossData(time_in,time_out);
}

Detection3D* WCV_TEP::make() const {
  return new WCV_TEP();
}

/**
 * Returns a deep copy of this WCV_TEP object, including any results that have been calculated.
 */
Detection3D* WCV_TEP::copy() const {
  WCV_TEP* ret = new WCV_TEP(table);
  ret->id = id;
  return ret;
}

std::string WCV_TEP::getSimpleClassName() const {
  return "WCV_TEP";
}

bool WCV_TEP::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(), cd->getCanonicalClassName()) ||
      larcfm::equals("gov.nasa.larcfm.ACCoRD.WCV_TAUMOD", cd->getCanonicalClassName()) ||
      larcfm::equals("gov.nasa.larcfm.ACCoRD.WCV_TCPA", cd->getCanonicalClassName())) {
    return containsTable((WCV_tvar*)cd);
  }
  return false;
}

}

