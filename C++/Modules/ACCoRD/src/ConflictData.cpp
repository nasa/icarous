/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * LossData.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: rbutler
 */

#include "ConflictData.h"
#include "format.h"

namespace larcfm {

ConflictData::ConflictData(double t_in, double t_out, double t_crit, double d_crit, const Vect3& s, const Velocity& v) : LossData(t_in, t_out) {
  time_crit = t_crit;
  dist_crit = d_crit;
  s_ = s;
  v_ = v;
}

ConflictData::ConflictData() {
  time_crit = PINFINITY;
  dist_crit = PINFINITY;
  s_ = Vect3::INVALID();
  v_ = Velocity::INVALIDV();
}

ConflictData::ConflictData(const LossData& ld, double t_crit, double d_crit, const Vect3& s, const Velocity& v) : LossData(ld) {
  time_crit = t_crit;
  dist_crit = d_crit;
  s_ = s;
  v_ = v;
}

/**
 * Returns internal vector representation of relative aircraft position.
 */
Vect3 const & ConflictData::get_s() const {
  return s_;
}

/**
 * Returns internal vector representation of relative aircraft velocity.
 */
Vect3 const & ConflictData::get_v() const {
  return v_;
}

/**
 * Returns HMD, in internal units, within lookahead time t, in seconds, assuming straight line trajectory.
 */
double ConflictData::HMD(double T) const {
  return Horizontal::hmd(s_.vect2(),v_.vect2(),T);
}

/**
 * Returns HMD, in specified units, within lookahead time t, in seconds, assuming straight line trajectory.
 */
double ConflictData::HMD(const std::string& u, double T) const {
  return Units::to(u,HMD(T));
}

/**
 * Returns VMD, in internal units, within lookahead time t, in seconds, assuming straight line trajectory.
 */
double ConflictData::VMD(double T) const {
  return Vertical::vmd(s_.z,v_.z,T);
}

/**
 * Returns VMD, in specified units, within lookahead time t, in seconds, assuming straight line trajectory.
 */
double ConflictData::VMD(const std::string& u, double T) const {
  return Units::to(u,VMD(T));
}

double ConflictData::getCriticalTimeOfConflict() const {
  return time_crit;
}

double ConflictData::getDistanceAtCriticalTime() const {
  return dist_crit;
}

std::string ConflictData::toString() const {
  std::string str = LossData::toString()+" [time_crit: " + Fm2(time_crit) +
      ", dist_crit: " + Fm2(dist_crit) +"]";
  return str;
}

} /* namespace larcfm */
