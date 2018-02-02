/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "AlertThresholds.h"
#include "Vect3.h"
#include "Util.h"
#include "Velocity.h"
#include "Detection3D.h"
#include "ConflictData.h"
#include "KinematicMultiBands.h"
#include "format.h"
#include "KinematicBandsParameters.h" // REMOVE ME!  Circular reference!

namespace larcfm {

/**
 * Creates an alert threholds object. Parameter det is a detector,
 * alerting_time is a non-negative alerting time (possibly positive infinity),
 * early_alerting_time is a early alerting time >= at (for maneuver guidance),
 * region is the type of guidance
 */
AlertThresholds::AlertThresholds(const Detection3D* detector,
    double alerting_time, double early_alerting_time,
    BandsRegion::Region region) {
  detector_ = detector == NULL ? NULL :detector->copy();
  alerting_time_ = std::abs(alerting_time);
  early_alerting_time_ = Util::max(alerting_time_,early_alerting_time);
  region_ = region;
  spread_trk_ = 0;
  spread_gs_ = 0;
  spread_vs_ = 0;
  spread_alt_ = 0;
  units_["alerting_time"] = "s";
  units_["early_alerting_time"] = "s";
  units_["spread_trk"] = "deg";
  units_["spread_gs"] = "knot";
  units_["spread_vs"] = "fpm";
  units_["spread_alt"] = "ft";
}

AlertThresholds::AlertThresholds(const AlertThresholds& athr) {
  detector_ = athr.isValid() ? athr.detector_->copy() : NULL;
  alerting_time_ = athr.alerting_time_;
  early_alerting_time_ = athr.early_alerting_time_;
  region_ = athr.region_;
  spread_trk_ = athr.spread_trk_;
  spread_gs_ = athr.spread_gs_;
  spread_vs_ = athr.spread_vs_;
  spread_alt_ = athr.spread_alt_;
  units_ = athr.units_;
}

AlertThresholds::AlertThresholds() {
  detector_ = NULL;
  alerting_time_ = 0;
  early_alerting_time_ = 0;
  region_ = BandsRegion::UNKNOWN;
  spread_trk_ = 0;
  spread_gs_ = 0;
  spread_vs_ = 0;
  spread_alt_ = 0;
  units_["alerting_time"] = "s";
  units_["early_alerting_time"] = "s";
  units_["spread_trk"] = "deg";
  units_["spread_gs"] = "m/s";
  units_["spread_vs"] = "m/s";
  units_["spread_alt"] = "m";
}

const AlertThresholds AlertThresholds::INVALID = AlertThresholds();

bool AlertThresholds::isValid() const {
  return detector_ != NULL && region_ != BandsRegion::UNKNOWN;
}

AlertThresholds::~AlertThresholds() {
  delete detector_;
}

AlertThresholds& AlertThresholds::operator=(const AlertThresholds& athr) {
  delete detector_;
  detector_ = athr.isValid() ? athr.detector_->copy() : NULL;
  alerting_time_ = athr.alerting_time_;
  early_alerting_time_ = athr.early_alerting_time_;
  region_ = athr.region_;
  spread_trk_ = athr.spread_trk_;
  spread_gs_ = athr.spread_gs_;
  spread_vs_ = athr.spread_vs_;
  spread_alt_ = athr.spread_alt_;
  units_ = athr.units_;
  return *this;
}

/**
 * Return detector.
 */
Detection3D* AlertThresholds::getDetectorRef() const {
  return detector_;
}

/**
 * Set detector.
 */
void AlertThresholds::setDetector(const Detection3D* det) {
  delete detector_;
  detector_ = det != NULL ? det->copy() : NULL;
}

/**
 * Return alerting time in seconds.
 */
double AlertThresholds::getAlertingTime() const {
  return alerting_time_;
}

/**
 * Return alerting time in specified units.
 */
double AlertThresholds::getAlertingTime(const std::string& u) const {
  return Units::to(u,alerting_time_);
}

/**
 * Set alerting time in seconds. Alerting time is non-negative number.
 */
void AlertThresholds::setAlertingTime(double val) {
  alerting_time_ = std::abs(val);
}

/**
 * Set alerting time in specified units. Alerting time is non-negative number.
 */
void AlertThresholds::setAlertingTime(double t, const std::string& u) {
  setAlertingTime(Units::from(u,t));
  units_["alerting_time"] = u;
}

/**
 * Return early alerting time in seconds.
 */
double AlertThresholds::getEarlyAlertingTime() const {
  return early_alerting_time_;
}

/**
 * Return early alerting time in specified units.
 */
double AlertThresholds::getEarlyAlertingTime(const std::string& u) const {
  return Units::to(u,early_alerting_time_);
}

/**
 * Set early alerting time in seconds. Early alerting time is a positive number >= alerting time
 */
void AlertThresholds::setEarlyAlertingTime(double t) {
  early_alerting_time_ = std::abs(t);
}

/**
 * Set early alerting time in specified units. Early alerting time is a positive number >= alerting time
 */
void AlertThresholds::setEarlyAlertingTime(double t, const std::string& u) {
  setEarlyAlertingTime(Units::from(u,t));
  units_["early_alerting_time"] = u;
}

/**
 * Return guidance region.
 */
BandsRegion::Region AlertThresholds::getRegion() const {
  return region_;
}

/**
 * Set guidance region.
 */
void AlertThresholds::setRegion(BandsRegion::Region region) {
  region_ = region;
}

/**
 * Get track spread in internal units [rad]. Spread is relative to ownship's track
 */
double AlertThresholds::getTrackSpread() const {
  return spread_trk_;
}

/**
 * Get track spread in given units [u]. Spread is relative to ownship's track
 */
double AlertThresholds::getTrackSpread(const std::string& u) const {
  return Units::to(u,spread_trk_);
}

/**
 * Set track spread in internal units. Spread is relative to ownship's track and is expected
 * to be in [0,pi].
 */
void AlertThresholds::setTrackSpread(double spread) {
  spread_trk_ = std::abs(Util::to_pi(spread));
}

/**
 * Set track spread in given units. Spread is relative to ownship's track and is expected
 * to be in [0,pi] [u].
 */
void AlertThresholds::setTrackSpread(double spread, const std::string& u) {
  setTrackSpread(Units::from(u,spread));
  units_["spread_trk"] = u;
}

/**
 * Get ground speed spread in internal units [m/s]. Spread is relative to ownship's ground speed
 */
double AlertThresholds::getGroundSpeedSpread() const {
  return spread_gs_;
}

/**
 * Get ground speed spread in given units. Spread is relative to ownship's ground speed
 */
double AlertThresholds::getGroundSpeedSpread(const std::string& u) const {
  return Units::to(u,spread_gs_);
}

/**
 * Set ground speed spread in internal units [m/s]. Spread is relative to ownship's ground speed and is expected
 * to be non-negative
 */
void AlertThresholds::setGroundSpeedSpread(double spread) {
  spread_gs_ = std::abs(spread);
}

/**
 * Set ground speed spread in given units. Spread is relative to ownship's ground speed and is expected
 * to be non-negative
 */
void AlertThresholds::setGroundSpeedSpread(double spread, const std::string& u) {
  setGroundSpeedSpread(Units::from(u,spread));
  units_["spread_gs"] = u;
}

/**
 * Get vertical speed spread in internal units [m/s]. Spread is relative to ownship's vertical speed
 */
double AlertThresholds::getVerticalSpeedSpread() const {
  return spread_vs_;
}

/**
 * Get vertical speed spread in given units. Spread is relative to ownship's vertical speed
 */
double AlertThresholds::getVerticalSpeedSpread(const std::string& u) const {
  return Units::to(u,spread_vs_);
}

/**
 * Set vertical speed spread in internal units [m/s]. Spread is relative to ownship's vertical speed and is expected
 * to be non-negative
 */
void AlertThresholds::setVerticalSpeedSpread(double spread) {
  spread_vs_ = std::abs(spread);
}

/**
 * Set vertical speed spread in given units. Spread is relative to ownship's vertical speed and is expected
 * to be non-negative
 */
void AlertThresholds::setVerticalSpeedSpread(double spread, const std::string& u) {
  setVerticalSpeedSpread(Units::from(u,spread));
  units_["spread_vs"] = u;
}

/**
 * Get altitude spread in internal units [m]. Spread is relative to ownship's altitude
 */
double AlertThresholds::getAltitudeSpread() const {
  return spread_alt_;
}

/**
 * Get altitude spread in given units. Spread is relative to ownship's altitude
 */
double AlertThresholds::getAltitudeSpread(const std::string& u) const {
  return Units::to(u,spread_alt_);
}

/**
 * Set altitude spread in internal units [m]. Spread is relative to ownship's altitude and is expected
 * to be non-negative
 */
void AlertThresholds::setAltitudeSpread(double spread) {
  spread_alt_ = std::abs(spread);
}

/**
 * Set altitude spread in given units. Spread is relative to ownship's altitude and is expected
 * to be non-negative
 */
void AlertThresholds::setAltitudeSpread(double spread, const std::string& u) {
  setAltitudeSpread(Units::from(u,spread));
  units_["spread_alt"] = u;
}

std::string AlertThresholds::toString() const {
  return  (detector_ == NULL ? "INVALID_DETECTOR" : detector_->toString())+
      ", alerting_time = "+Units::str(getUnits("alerting_time"),alerting_time_)+
      ", early_alerting_time = "+Units::str(getUnits("early_alerting_time"),early_alerting_time_)+
      ", region = "+BandsRegion::to_string(region_)+
      ", spread_trk = "+Units::str(getUnits("spread_trk"),spread_trk_)+
      ", spread_gs = "+Units::str(getUnits("spread_gs"),spread_gs_)+
      ", spread_vs = "+Units::str(getUnits("spread_vs"),spread_vs_)+
      ", spread_alt = "+Units::str(getUnits("spread_alt"),spread_alt_);
}

std::string AlertThresholds:: toPVS(int prec) const {
  return "(# wcv:= "+(detector_ == NULL ? "INVALID_DETECTOR" :detector_->toPVS(prec))+
      ", alerting_time:= "+Fm1(alerting_time_)+
      ", early_alerting_time:= "+Fm1(early_alerting_time_)+
      ", region:= "+BandsRegion::to_string(region_)+
      ", spread_trk:= ("+FmPrecision(spread_trk_,prec)+","+FmPrecision(spread_trk_,prec)+")"+
      ", spread_gs:= ("+FmPrecision(spread_gs_,prec)+","+FmPrecision(spread_gs_,prec)+")"+
      ", spread_vs:= ("+FmPrecision(spread_vs_,prec)+","+FmPrecision(spread_vs_,prec)+")"+
      ", spread_alt:= ("+FmPrecision(spread_alt_,prec)+","+FmPrecision(spread_alt_,prec)+")"+
      " #)";
}

ParameterData AlertThresholds::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void AlertThresholds::updateParameterData(ParameterData& p) const {
  p.set("region",BandsRegion::to_string(region_));
  if (detector_ != NULL) {
    p.set("detector",detector_->getIdentifier());
  }
  p.setInternal("alerting_time",alerting_time_,getUnits("alerting_time"));
  p.setInternal("early_alerting_time",early_alerting_time_,getUnits("early_alerting_time"));
  p.setInternal("spread_trk",spread_trk_,getUnits("spread_trk"));
  p.setInternal("spread_gs",spread_gs_,getUnits("spread_gs"));
  p.setInternal("spread_vs",spread_vs_,getUnits("spread_vs"));
  p.setInternal("spread_alt",spread_alt_,getUnits("spread_alt"));
}

void AlertThresholds::setParameters(const ParameterData& p) {
  if (p.contains("region")) {
    setRegion(BandsRegion::valueOf(p.getString("region")));
  }
  if (p.contains("alerting_time")) {
    setAlertingTime(p.getValue("alerting_time"));
    units_["alerting_time"] = p.getUnit("alerting_time");
  }
  if (p.contains("early_alerting_time")) {
    setEarlyAlertingTime(p.getValue("early_alerting_time"));
    units_["early_alerting_time"] = p.getUnit("early_alerting_time");
  }
  if (p.contains("spread_trk")) {
    setTrackSpread(p.getValue("spread_trk"));
    units_["spread_trk"] = p.getUnit("spread_trk");
  }
  if (p.contains("spread_gs")) {
    setGroundSpeedSpread(p.getValue("spread_gs"));
    units_["spread_gs"] = p.getUnit("spread_gs");
  }
  if (p.contains("spread_vs")) {
    setVerticalSpeedSpread(p.getValue("spread_vs"));
    units_["spread_vs"] = p.getUnit("spread_vs");
  }
  if (p.contains("spread_alt")) {
    setAltitudeSpread(p.getValue("spread_alt"));
    units_["spread_alt"] = p.getUnit("spread_alt");
  }
}

std::string AlertThresholds::getUnits(const std::string& key) const {
  std::map<std::string,std::string>::const_iterator got = units_.find(key);
  if (got == units_.end()) {
    return "unspecified";
  }
  return got->second;
}

}
