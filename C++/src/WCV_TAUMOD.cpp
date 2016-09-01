/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "WCV_tvar.h"
#include "WCV_TAUMOD.h"
#include "WCV_TCPA.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Horizontal.h"
#include "WCVTable.h"
#include "LossData.h"
#include "format.h"
#include "string_util.h"

namespace larcfm {

/** Constructor that uses the default TCAS tables. */
WCV_TAUMOD::WCV_TAUMOD() {
  id = "";
}

/** Constructor that specifies a particular instance of the TCAS tables. */
WCV_TAUMOD::WCV_TAUMOD(const WCVTable& tab) {
  table.copyValues(tab);
  id = "";
}

double WCV_TAUMOD::horizontal_tvar(const Vect2& s, const Vect2& v) const {
  // Time variable is Modified Tau
  double taumod = -1;
  double sdotv = s.dot(v);
  if (sdotv < 0)
    return (Util::sq(table.getDTHR())-s.sqv())/sdotv;
  return taumod;
}

LossData WCV_TAUMOD::horizontal_WCV_interval(double T, const Vect2& s, const Vect2& v) const {
  double time_in = T;
  double time_out = 0;
  double sqs = s.sqv();
  double sdotv = s.dot(v);
  double sqD = Util::sq(table.getDTHR());
  double a = v.sqv();
  double b = 2*sdotv+table.getTTHR()*v.sqv();
  double c = sqs+table.getTTHR()*sdotv-sqD;
  if (Util::almost_equals(a,0) && sqs <= sqD) { // [CAM] Changed from == to almost_equals to mitigate numerical problems
    time_in = 0;
    time_out = T;
    return LossData(time_in,time_out);
  }
  if (sqs <= sqD) {
    time_in = 0;
    time_out = std::min(T,Horizontal::Theta_D(s,v,1,table.getDTHR()));
    return LossData(time_in,time_out);
  }
  double discr = Util::sq(b)-4*a*c;
  if (sdotv >= 0 || discr < 0)
    return LossData(time_in,time_out);
  double t = (-b - std::sqrt(discr))/(2*a);
  if (Horizontal::Delta(s, v,table.getDTHR()) >= 0 && t <= T) {
    time_in = std::max(0.0,t);
    time_out = std::min(T, Horizontal::Theta_D(s,v,1,table.getDTHR()));
  }
  return LossData(time_in,time_out);
}

Detection3D* WCV_TAUMOD::make() const {
  return new WCV_TAUMOD();
}

/**
 * Returns a deep copy of this WCV_TAUMOD object, including any results that have been calculated.
 */
Detection3D* WCV_TAUMOD::copy() const {
  WCV_TAUMOD* ret = new WCV_TAUMOD();
  ret->table.copyValues(table);
  ret->id = id;
  return ret;
}

std::string WCV_TAUMOD::getSimpleClassName() const {
  return "WCV_TAUMOD";
}

bool WCV_TAUMOD::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(), cd->getCanonicalClassName())) {
    WCV_TAUMOD* d = (WCV_TAUMOD*)cd;
    return table.contains(d->table);
  }
  if (larcfm::equals("gov.nasa.larcfm.ACCoRD.WCV_TCPA", cd->getCanonicalClassName())) {
    WCVTable tab = ((WCV_TCPA*)cd)->getWCVTable();
    return table.contains(tab);
  }
  return false;
}

}

