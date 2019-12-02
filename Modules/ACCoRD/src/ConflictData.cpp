/*
 * Copyright (c) 2014-2018 United States Government as represented by
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
#include "Units.h"
#include "Velocity.h"

namespace larcfm {

ConflictData::ConflictData(double t_in, double t_out, double t_crit, double d_crit, const Vect3& s, const Vect3& v) : LossData(t_in, t_out) {
  time_crit = t_crit;
  dist_crit = d_crit;
  s_ = s;
  v_ = v;
}

ConflictData::ConflictData() {
  time_crit = PINFINITY;
  dist_crit = PINFINITY;
  s_ = Vect3::INVALID();
  v_ = Vect3::INVALID();
}

const ConflictData& ConflictData::EMPTY() {
  static ConflictData tmp;
  return tmp;
}

ConflictData::ConflictData(const LossData& ld, double t_crit, double d_crit, const Vect3& s, const Vect3& v) : LossData(ld) {
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

/**
 * Horizontal separation
 * @return Horizontal separation in internal units at current time
 */
 double ConflictData::horizontalSeparation() const {
    return s_.norm2D();
}

/**
 * Horizontal separation
 * @param u units
 * @return Horizontal separation in specified units u at current time
 */
 double ConflictData::horizontalSeparation(const std::string& u) const {
    return Units::to(u,horizontalSeparation());
}

/**
 * Horizontal separation
 * @param time in seconds
 * @return Horizontal separation in internal units at given time
 */
 double ConflictData::horizontalSeparationAtTime(double time) const {
    return s_.AddScal(time,v_).norm2D();
}

/**
 * Horizontal separation at given time
 * @param time time in seconds
 * @param u units
 * @return Horizontal separation in specified units at given time
 */
 double ConflictData::horizontalSeparationAtTime(const std::string& u, double time) const {
    return Units::to(u,horizontalSeparationAtTime(time));
}

/**
 * Vertical separation
 * @return Vertical separation in internal units at current time
 */
 double ConflictData::verticalSeparation() const {
    return std::abs(s_.z);
}

/**
 * Vertical separation
 * @param u units
 * @return Vertical separation in specified units at current time
 */
 double ConflictData::verticalSeparation(const std::string& u) const {
    return Units::to(u,verticalSeparation());
}

/**
 * Vertical separation at given time
 * @param time time in seconds
 * @return Vertical separation in internal units at given time
 */
 double ConflictData::verticalSeparationAtTime(double time) const {
    return std::abs(s_.AddScal(time,v_).z);
}

/**
 * Vertical separation at given time
 * @param time time in seconds
 * @param u units
 * @return Vertical separation in specified units at given time
 */
 double ConflictData::verticalSeparationAtTime(const std::string& u, double time) const {
    return Units::to(u,verticalSeparationAtTime(time));
}

/**
 * Time to horizontal closest point of approach in seconds.
 * When aircraft are diverging, tcpa is defined as 0.
 */
 double ConflictData::tcpa2D() const {
    return Util::max(0.0,Horizontal::tcpa(s_.vect2(),v_.vect2()));
}

/**
 * Time to 3D closest point of approach in seconds.
 * When aircraft are diverging, tcpa is defined as 0
 */
 double ConflictData::tcpa3D() const {
    return Vect3::tcpa(s_,Vect3::ZERO(),v_,Velocity::ZEROV());
}

/**
 * Time to co-altitude.
 * @return time to co-altitude in seconds. Returns NaN is v_.z is zero.
 */
 double ConflictData::tcoa() const {
    return Vertical::time_coalt(s_.z,v_.z);
}

/**
 * Horizontal closure rate
 * @return Horizontal closure rate in internal units at current time
 */
 double ConflictData::horizontalClosureRate() const {
    return v_.norm2D();
}

/**
 * Horizontal closure rate
 * @param u units
 * @return Horizontal closure rate in specified units u at current time
 */
 double ConflictData::horizontalClosureRate(const std::string& u) const {
    return Units::to(u,horizontalClosureRate());
}

/**
 * Vertical closure rate
 * @return Vertical closure rate in internal units at current time
 */
 double ConflictData::verticalClosureRate() const {
    return std::abs(v_.z);
}

/**
 * Vertical closure rate
 * @param u units
 * @return Vertical closure rate in specified units at current time
 */
 double ConflictData::verticalClosureRate(const std::string& u) const {
    return Units::to(u,verticalClosureRate());
}

/**
 * @return A time in seconds that can be used to compare severity of conflicts for arbitrary
 * well-clear volumes. This time is not necessarily TCPA. ** Don't use it as TCPA. **
 */
double ConflictData::getCriticalTimeOfConflict() const {
  return time_crit;
}

double ConflictData::getDistanceAtCriticalTime() const {
  return dist_crit;
}

/**
 * @return A non-negative scalar that can be used to compare severity of conflicts for arbitrary
 * well-clear volumes. This scalar is a distance in the mathematical way. It is 0 when aircraft are
 * at the same poistion, but it isn't a distance in the physical way. In particular, this distance
 * is unitless. ** Don't use as CPA **
 */
std::string ConflictData::toString() const {
  std::string str = LossData::toString()+" [time_crit: " + FmPrecision(time_crit) +
      ", dist_crit: " + FmPrecision(dist_crit) +"]";
  return str;
}

} /* namespace larcfm */
