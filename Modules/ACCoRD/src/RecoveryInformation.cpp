/*
 * Copyright (c) 2015-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "format.h"
#include "string_util.h"
#include "RecoveryInformation.h"
#include "Units.h"

#include <string>
#include <cmath>


namespace larcfm {


RecoveryInformation::RecoveryInformation(double t, int nfactor, double hs, double vs) {
  time_ = t;
  nfactor_ = nfactor;
  horizontal_distance_ = hs;
  vertical_distance_ = vs;
}

/**
 * @return Number of time the recovery volume was reduced
 */
int RecoveryInformation::nFactor() const {
  return nfactor_;
}


/**
 * @return Time to recovery in seconds
 */
double RecoveryInformation::timeToRecovery() const {
  return time_;
}

/**
 * @return Time to recovery in given units
 */
double RecoveryInformation::timeToToRecovery(const std::string& u) const {
  return Units::to(u,time_);
}

/**
 * @return Recovery horizontal distance in internal units, i.e., [m]
 */
double RecoveryInformation::recoveryHorizontalDistance() const {
  return horizontal_distance_;
}

/**
 * @return Recovery horizontal distance in given units
 */
double RecoveryInformation::recoveryHorizontalDistance(const std::string& u) const {
  return Units::to(u,horizontal_distance_);
}

/**
 * @return Recovery vertical distance in internal units, i.e., [m]
 */
double RecoveryInformation::recoveryVerticalDistance() const {
  return vertical_distance_;
}

/**
 * @return Recovery vertical distance in given units
 */
double RecoveryInformation::recoveryVerticalDistance(const std::string& u) const {
  return Units::to(u,vertical_distance_);
}

/**
 * @return True if recovery bands are computed.
 */
bool RecoveryInformation::recoveryBandsComputed() const {
  return !ISNAN(time_) && nfactor_ >= 0;
}

/**
 * @return True if recovery are computed, but they are saturated.
 */
bool RecoveryInformation::recoveryBandsSaturated() const {
  return ISINF(time_) && time_ < 0;
}

std::string RecoveryInformation::toString() const {
  std::string str = " [time_to_recovery: "+FmPrecision(time_)+
      ", horizontal_distance: "+FmPrecision(horizontal_distance_)+
      ", vertical_distance: "+FmPrecision(vertical_distance_)+
      ", nfactor: "+Fmi(nfactor_)+"]";
  return str;
}

std::string RecoveryInformation::toStringUnits(const std::string& hunits,const std::string& vunits) const {
  std::string str = "[time_to_recovery: "+Units::str("s",time_)+
      ", horizontal_distance: "+Units::str(hunits,horizontal_distance_)+
      ", vertical_distance: "+Units::str(vunits,vertical_distance_)+
      ", nfactor: "+Fmi(nfactor_)+"]";
  return str;

}

std::string RecoveryInformation::toPVS() const {
  return "(# time:="+double2PVS(time_)+
      "::ereal,horizontal_distance:="+double2PVS(horizontal_distance_)+
      "::ereal,vertical_distance:="+double2PVS(vertical_distance_)+
      "::ereal,nfactor:="+Fmi(nfactor_)+" #)";
}

}
