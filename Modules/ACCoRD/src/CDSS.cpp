/*
 * CDSS.cpp
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Conflict detection between an ownship and traffic aircraft using state information.
 *   
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CDSS.h"
#include <cmath>
#include <iostream>
#include "Units.h"
#include "format.h"
#include "Util.h"
#include "CDCylinder.h"
#include "ConflictData.h"


namespace larcfm {

void CDSS::init(const double d, const double h, const double f) {
  D = d;
  H = h;
  filter = f;
  TimeHorizon = PINFINITY;
  tca = PINFINITY;
  stca = Vect3();
  t_in = PINFINITY;
  t_out = NINFINITY;
  cd = CDCylinder::mk(d,h);
}

/**
 * Create a new state-based conflict detection object using internal units.
 *
 * @param distance the minimum horizontal separation distance [m]
 * @param height the minimum vertical separation height [m].
 * @param cdfilter the cdfilter
 */
CDSS::CDSS(const double distance, const double height, const double cdfilter) {
  init(distance,height,cdfilter);
}


CDSS::CDSS() {
  init(Units::from("deg",5.0), Units::from("ft",1000), 1.0);
}


/**
 * Create a new state-based conflict detection object using specified units.
 *
 * @param distance the minimum horizontal separation distance [hStr]
 * @param height the minimum vertical separation height [dStr].
 */
CDSS CDSS::make(double distance, const std::string& dStr, double height, const std::string& hStr) {
  return CDSS(Units::from(dStr, distance), Units::from(hStr,height), 1.0);
}

/**
 * Create a new state-based conflict detection object using internal units.
 *
 * @param distance the minimum horizontal separation distance [m]
 * @param height the minimum vertical separation height [m].
 */
CDSS CDSS::mk(double distance, double height) {
  return CDSS(distance, height, 1.0);
}


/**
 * Create a new state-based conflict detection object using explicit units.
 *
 * @param distance the minimum horizontal separation distance [ud]
 * @param ud the explicit units of distance
 * @param height the minimum vertical separation height [uh]
 * @param uh the explicit units of height
 * @param cdfilter the cdfilter
 */
CDSS::CDSS(const double distance, const std::string& ud,
    const double height, const std::string& uh,
    const double cdfilter) {
  init(Units::from(ud,distance),Units::from(uh,height), cdfilter);
}

CDSS::CDSS(const CDSS& cdss) {
  //fpln(" $$$$$$$ CDSS COPY D="+Fm1(cdss.D)+" H="+Fm1(cdss.H));
  D = cdss.D;
  H = cdss.H;
  filter = cdss.filter;
  TimeHorizon = cdss.TimeHorizon;
  tca = cdss.tca;
  stca = cdss.stca;
  t_in = cdss.t_in;
  t_out = cdss.t_out;
  cd = cdss.cd;
}

CDSS::~CDSS() {
}

CDSS& CDSS::operator=(const CDSS& cdss) {
  //fpln(" $$$$$$$ CDSS ASSIGN D="+Fm1(cdss.D)+" H="+Fm1(cdss.H));
  D = cdss.D;
  H = cdss.H;
  filter = cdss.filter;
  TimeHorizon = cdss.TimeHorizon;
  tca = cdss.tca;
  stca = cdss.stca;
  t_in = cdss.t_in;
  t_out = cdss.t_out;
  cd = cdss.cd;
  return *this;
}

/**
 * Returns the minimum horizontal separation distance in internal units [m].
 *
 * @return the distance
 */
double CDSS::getDistance() const {
  return D;
}

/**
 * Returns the minimum horizontal separation distance in explicit units.
 *
 * @param ud the explicit units of distance
 *
 * @return the distance in explicit units [ud]
 */
double CDSS::getDistance(const std::string& ud) const {
  return Units::to(ud,D);
}

/**
 * Returns the minimum vertical separation distance in internal units.
 *
 * @return the height in internal units [m]
 */
double CDSS::getHeight() const {
  return H;
}

/**
 * Returns the minimum vertical separation distance in explicit units.
 *
 * @param uh the explicit units of height
 *
 * @return the height in explicit units [uh]
 */
double CDSS::getHeight(const std::string& uh) const {
  return Units::to(uh,H);
}

/**
 * Returns the time horizon in internal units.
 * time < 0 means infinite time horizon
 *
 * @return the time horizon [s]
 */
double CDSS::getTimeHorizon() const {
  return TimeHorizon;
}

/**
 * Returns the time horizon in explicit units.
 * time < 0 means infinite time horizon
 *
 * @param ut the explicit units of time
 *
 * @return the time horizon in explicit units [ut]
 */
double CDSS::getTimeHorizon(const std::string& ut) const {
  if (TimeHorizon < 0) return TimeHorizon;
  return Units::to(ut,TimeHorizon);
}

/**
 * Returns the conflict detection filter time.
 *
 * @return the conflict detection filter time seconds
 */
double CDSS::getFilterTime() const {
  return filter;
}

/**
 * Sets the minimum horizontal separation distance in internal units.
 *
 * @param distance the distance in internal units [m]
 */
void CDSS::setDistance(const double distance) {
  D = distance;
}

/**
 * Sets the minimum horizontal separation distance in explicit units.
 *
 * @param distance the distance in explicit units [ud]
 * @param ud the explicit units of distance
 */
void CDSS::setDistance(const double distance, const std::string& ud) {
  D = Units::from(ud,distance);
}

/**
 * Sets the minimum vertical separation distance in internal units.
 *
 * @param height the height in internal units [m]
 */
void CDSS::setHeight(const double height) {
  H = height;
}

/**
 * Sets the minimum vertical separation distance in explicit units.
 *
 * @param height the height in explicit units [uh]
 * @param uh the explicit units of time of height
 */
void CDSS::setHeight(const double height, const std::string& uh) {
  H = Units::from(uh,height);
}

/**
 * Sets the time horizon in internal units.
 *
 * @param time the time horizon in internal units [s].
 * time < 0 means infinite time horizon
 */
void CDSS::setTimeHorizon(const double time) {
  TimeHorizon = time;
}

/**
 * Sets the time horizon in explicit units.
 *
 * @param time the time horizon in explicit units [ut].
 * time < 0 means infinite time horizon
 * @param ut the explicit units of time
 */
void CDSS::setTimeHorizon(const double time, const std::string& ut) {
  TimeHorizon = Units::from(ut,time);
}

/**
 * Sets the conflict detection filter time.
 *
 * @param cdfilter the conflict detection filter time in seconds.
 */
void CDSS::setFilterTime(const double cdfilter) {
  filter = cdfilter;
}

double CDSS::getFilter() const {
  return filter;
}


/**
 * Determines if two aircraft are in loss of separation.
 * This function DOES NOT compute time interval of conflict.
 *
 * @param s the relative position of the aircraft
 *
 * @return true, if the aircraft are in loss of separation.
 */
bool CDSS::lossOfSeparation(const Vect3& s) const {
  return cd.violation(s,Velocity::ZEROV(),Vect3::ZERO(),Velocity::ZEROV(),D,H);
}

bool CDSS::lossOfSeparation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const {
  return cd.violation(so, vo, si, vi, D, H);
}

bool CDSS::lossOfSeparation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double d, double h) const {
  return cd.violation(so, vo, si, vi, d, h);
}

/**
 * Determines if two aircraft are in conflict ever within time horizon.
 * This function DOES NOT compute time interval of conflict.
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 *
 * @return true, if the aircraft are in conflict in the interval [0,TimeHorizon].
 */
bool CDSS::conflictEver(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const {
  return cd.conflict(so, vo, si, vi, D, H, 0, TimeHorizon);
}

bool CDSS::conflict(const Vect3& s, const Velocity& vo, const Velocity& vi, double D, double H, double T) { // static
  return CD3D::cd3d(s,vo,vi,D,H,T);
}

bool CDSS::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double D, double H, double T) const {
  return cd.conflict(so,vo,si,vi,D,H,0.0,T);
}


/**
 * Determines if two aircraft are in conflict in a given lookahed time interval (internal
 * units).
 * This function DOES NOT compute time interval of conflict.
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param B  lower bound of lookahead time interval in internal units [s] (B >= 0)
 * @param T  upper bound of lookahead time interval in internal units [s] (T > 0)
 *
 * @return true, if the aircraft are in conflict in the interval [B,T].
 */
bool CDSS::conflictBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
    const double B, const double T) const {
  //    tca = larcfm::InfiniteTime;
  //    stca = Vect3::ZERO();
  //    cd->getTimeIn() = t_out = 0;
  return cd.conflict(so,vo,si,vi,D,H,B,TimeHorizon < 0 ? T : Util::min(T,TimeHorizon));
}

/**
 * Determines if two aircraft are in conflict in a given lookahead time internal (explicit
 * units).
 * This function DOES NOT compute time interval of conflict.
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param B  lower bound of lookahead time interval in explicit units [ut] (B >= 0)
 * @param T  upper bound of lookahead time interval in explicit units [ut] (T > 0)
 * @param ut explicit units of time
 *
 * @return true, if the aircraft are in conflict in the interval [B,T].
 */
bool CDSS::conflictBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
    const double B, const double T, const std::string& ut) const {
  return conflictBetween(so,vo,si,vi,Units::from(ut,B),Units::from(ut,T));
}

/**
 * Determines if two aircraft are in conflict within a given lookahed time (internal
 * units).
 * This function DOES NOT compute time interval of conflict.
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param T  lookahead time in internal units [s] (T > 0)
 *
 * @return true, if the aircraft are in conflict in the interval [0,T].
 */
bool CDSS::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,const double T) const {
  return conflictBetween(so,vo,si,vi,0,T);
}

/**
 * Determines if two aircraft are in conflict within a given lookahead time (explicit
 * units).
 * This function DOES NOT compute time interval of conflict.
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param T  upper bound of lookahead time interval in explicit units [ut] (T > 0)
 * @param ut explicit units of time
 *
 * @return true, if the aircraft are in conflict in the interval [0,T].
 */
bool CDSS::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
    const double T, const std::string& ut) const {
  return conflictBetween(so,vo,si,vi,0,Units::from(ut,T));
}

/**
 * Detects a conflict that lasts more than filter time within time horizon
 * and computes the time interval of conflict.
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 *
 * @return true, if there is a conflict that last more than the filter time
 * in the interval [0,TimeHorizon].
 */
bool CDSS::detectionEver(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) {
  ConflictData det = cd.conflictDetection(so, vo, si, vi, D, H, 0.0, TimeHorizon);
  tca = det.getCriticalTimeOfConflict();
  Vect3 v = vo-vi;
  stca = v.ScalAdd(tca,so.Sub(si));
  t_in = det.getTimeIn();
  t_out = det.getTimeOut();
  return det.conflict(filter);
}

/**
 * Detects a conflict that lasts more than filter time in a given lookahed time
 * interval and computes the time interval of conflict (internal units).
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param B  lower bound of lookahead time interval in internal units [s] (B >= 0)
 * @param T  upper bound of lokkahed time interval in internal units [s] (T > 0)
 *
 * @return true, if there is a conflict that last more than the filter time
 * in the interval [B,T].
 */
bool CDSS::detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
    const double B, const double T) {
  //    std::cout << "CDSS:detectionBetween s=" << s.toString() << " vo=" << vo.toString() << " vi=" << vi.toString() << " B=" << B << " T=" << T << std::endl;
  bool conflict = detectionEver(so,vo,si,vi);
  tca = TimeHorizon < 0 ? Util::max(B,tca) : Util::min(Util::max(B,tca),TimeHorizon);
  Vect3 v = vo-vi;
  stca = v.ScalAdd(tca,so.Sub(si));
  return conflict && t_in < T && t_out >= B;

}

/**
 * Detects a conflict that lasts more than filter time in a given lookahead time
 * interval and computes the time interval of conflict (explicit units).
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param B  lower bound of lookahead time interval in explicit units [ut] (B >= 0)
 * @param T  upper bound of lookahead time interval in explicit units [ut] (T > 0)
 * @param ut explicit units of time
 *
 * @return true, if there is a conflict that last more than the filter time
 * in the interval [B,T].
 */
bool CDSS::detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
    const double B, const double T, const std::string& ut) {
  return detectionBetween(so,vo,si,vi,Units::from(ut,B),Units::from(ut,T));
}

/**
 * Detects a conflict that lasts more than filter time within a given lookahed time
 * and computes the time interval of conflict (internal units).
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param T  the lookahed time in internal units [s] (T > 0)
 *
 * @return true, if there is a conflict that last more than the filter time
 * in the interval [0,T].
 */
bool CDSS::detection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, const double T) {
  return detectionBetween(so,vo,si,vi,0.0,T);
}

/**
 * Detects a conflict that lasts more than filter time within a given lookahead time
 * and computes the time interval of conflict (explicit units).
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param T  the lookahead time in explicit units [ut] (T > 0)
 * @param ut explicit units of time
 *
 * @return true, if there is a conflict that last more than the filter time
 * in the interval [0,T].
 */
bool CDSS::detection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
    const double T, const std::string& ut) {
  return detectionBetween(so,vo,si,vi,0.0,Units::from(ut,T));
}

/**
 * Duration of conflict in internal units.
 *
 * @return the duration of conflict. Zero means that there is not conflict.
 * A negative number means that the conflict last for ever.
 */
double CDSS::conflictDuration() const {
  if (!conflict())
    return 0;
  return t_out - t_in;
}

/**
 * Duration of conflict in explicit units.
 *
 * @param ut the explicit units of time
 *
 * @return the duration of conflict in explicit units [ut]. Zero  means that
 * there is no conflict. A negative number means that the conflict last for ever.
 */
double CDSS::conflictDuration(const std::string& ut) const {
  return Units::to(ut,conflictDuration());
}

/** EXPERIMENTAL STATIC TIME INTO LOS (assumes infinite lookahead time), undefined if not conflict
 *
 * @param s the relative position of the aircraft
 * @param vo the ownship's velocity
 * @param vi the intruder's velocity
 * @param D the minimum horizontal distance
 * @param H the minimum vertical distance
 *
 * @return
 */
double CDSS::timeIntoLoS(const Vect3& s, const Vect3& vo, const Vect3& vi, double D, double H) {
  double t_in  = PINFINITY;  // no conflict
  Vect2 s2  = s.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();
  double vz = vo.z-vi.z;
  if (vo2.almostEquals(vi2) && Horizontal::almost_horizontal_los(s2,D)) {
    if (!Util::almost_equals(vo.z,vi.z)) {
      t_in  = Util::max(Vertical::Theta_H(s.z,vz,Entry,H),0.0);
    } else if (Vertical::almost_vertical_los(s.z,H)) {
      t_in  = 0;
    }
  } else {
    Vect2 v2 = vo2.Sub(vi2);
    if (Horizontal::Delta(s2,v2,D) > 0) {
      double td1 = Horizontal::Theta_D(s2,v2,Entry,D);
      if (!Util::almost_equals(vo.z,vi.z)) {
        double tin  = Util::max(td1,Vertical::Theta_H(s.z,vz,Entry,H));
        t_in  = Util::max(tin,0.0);
      } else if (Vertical::almost_vertical_los(s.z,H) ) {
        t_in  = Util::max(td1,0.0);
      }
    }
  }
  //fpln(" CDSS::timeIntoLoS: t_in =  "+Fm1(t_in));
  return t_in;
}

/**
 * Was there a detected conflict?
 */
bool CDSS::conflict() const {
  return t_in < t_out;
}

/**
 * Time to loss of separation in internal units.
 *
 * @return the time to loss of separation. If time is negative then there
 * is no conflict.
 */
double CDSS::getTimeIn() const {
  if (conflict())
    return t_in;
  return PINFINITY;
}


/**
 * Time to loss of separation in explicit units.
 *
 * @param ut the explicit units of time
 *
 * @return the time to loss of separation in explicit units [ut].
 * If time is negative then there is no conflict.
 */
double CDSS::getTimeIn(const std::string& ut) const {
  return Units::to(ut,getTimeIn());
}

/**
 * Time to recovery of loss of separation in internal units.
 *
 * @return the time to recovery of loss of separation. If timeOut is zero then
 * there is no conflict. If timeOut is negative then, timeOut is infinite.
 */
double CDSS::getTimeOut() const {
  if (conflict())
    return t_out;
  return NINFINITY;
}

/**
 * Time to recovery of loss of separation in explicit units.
 *
 * @param ut the explicit units of time
 *
 * @return the time to recovery of loss of separation in explicit units [ut].
 * If timeOut is zero then there is no conflict. If timeOut is negative then,
 * timeOut is infinite.
 */
double CDSS::getTimeOut(const std::string& ut) const {
  return Units::to(ut,getTimeOut());
}

/**
 * Time of closest approach in internal units.
 *
 * @return the cylindrical time of closest approach [s].
 */
double CDSS::timeOfClosestApproach() const {
  return tca;
}

/**
 * Time of cylindrical closest approach in explicit units.
 *
 * @param ut the explicit units of time
 *
 * @return the time of (cylindrical) closest approach in explicit units [ut]
 */
double CDSS::timeOfClosestApproach(const std::string& ut) const {
  return Units::to(ut,timeOfClosestApproach());
}

/**
 * Cylindrical distance at time of closest approach.
 *
 * @return the cylindrical distance at time of closest approach. This distance normalizes
 * horizontal and vertical distances. Therefore, it is unitless. It has the property that
 * the value is less than 1 if and only if the aircraft are in loss of separation. The value is 1
 * if the ownship is at the boundary of the intruder's protected zone.
 */
double CDSS::cylindricalDistanceAtTCA() const {
  return stca.cyl_norm(D,H);
}

/**
 * Relative position at time of closest approach (internal units).
 *
 * @return the relative position of the ownship with respect to the intruder
 * at time of closest approach.
 *
 */
Vect3 CDSS::relativePositionAtTCA() const {
  return stca;
}

/**
 * Vertical distance at time of closest approach (internal units).
 *
 * @return the vertical distance at time of closest approach.
 *
 */
double CDSS::verticalDistanceAtTCA() const {
  return std::abs(stca.z);
}

/**
 * Vertical distance at time of closest approach (explicit units).
 *
 * @param ud the explicit units of distance
 *
 * @return the vertical distance at time of closest approach.
 *
 */
double CDSS::verticalDistanceAtTCA(const std::string& ud) const {
  return Units::to(ud,verticalDistanceAtTCA());
}

/**
 * Horizontal distance at time of closest approach (internal units).
 *
 * @return the horizontal distance at time of closest approach.
 */
double CDSS::horizontalDistanceAtTCA() const {
  return stca.vect2().norm();
}

/**
 * Horizontal distance at time of closest approach (explicit units).
 *
 * @param ud the explicit units of distance
 *
 * @return the horizontal distance at time of closest approach.
 */
double CDSS::horizontalDistanceAtTCA(const std::string& ud) const {
  return Units::to(ud,horizontalDistanceAtTCA());
}



std::string CDSS::toString() {
  std::string rtn = "CDSS: D = "+Units::str("nm",D)+" H = "+Units::str("ft",H)+" TimeHorizon = "+Fm1(TimeHorizon)+
      " t_in = "+Fm1(t_in)+" t_out =  "+Fm1(t_out)+" tca = "+Fm1(tca)+"\n";
  if (t_in == t_out) rtn = rtn + "--------- No conflict ---------";
  return rtn;
}

}
