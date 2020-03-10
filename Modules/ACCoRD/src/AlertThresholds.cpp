/*
 * Copyright (c) 2015-2019 United States Government as represented by
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
#include "format.h"

namespace larcfm {

/**
 * Creates an alert threholds object. Parameter det is a detector,
 * alerting_time is a non-negative alerting time (possibly positive infinity),
 * early_alerting_time is a early alerting time >= at (for maneuver guidance),
 * region is the type of guidance
 */
AlertThresholds::AlertThresholds(const Detection3D* detector,
    double alerting_time, double early_alerting_time, BandsRegion::Region region) :
                                alerting_time_(std::abs(alerting_time)),
                                early_alerting_time_(Util::max(alerting_time_,early_alerting_time)),
                                region_(region),
                                spread_hdir_(0.0),
                                spread_hs_(0.0),
                                spread_vs_(0.0),
                                spread_alt_(0.0) {
  detector_ = detector != NULL ? detector->copy() : NULL;
  units_["alerting_time"] = "s";
  units_["early_alerting_time"] = "s";
  units_["spread_hdir"] = "deg";
  units_["spread_hs"] = "knot";
  units_["spread_vs"] = "fpm";
  units_["spread_alt"] = "ft";
}

AlertThresholds::AlertThresholds(const AlertThresholds& athr) :
                            alerting_time_(athr.alerting_time_),
                            early_alerting_time_(athr.early_alerting_time_),
                            region_(athr.region_),
                            spread_hdir_(athr.spread_hdir_),
                            spread_hs_(athr.spread_hs_),
                            spread_vs_(athr.spread_vs_),
                            spread_alt_(athr.spread_alt_) {
  detector_ = athr.isValid() ? athr.detector_->copy() : NULL;
  units_ = athr.units_;
}

AlertThresholds::AlertThresholds() :
                      detector_(NULL),
                      alerting_time_(0.0),
                      early_alerting_time_(0.0),
                      region_(BandsRegion::UNKNOWN),
                      spread_hdir_(0.0),
                      spread_hs_(0.0),
                      spread_vs_(0.0),
                      spread_alt_(0.0) {
  units_["alerting_time"] = "s";
  units_["early_alerting_time"] = "s";
  units_["spread_hdir"] = "deg";
  units_["spread_hs"] = "m/s";
  units_["spread_vs"] = "m/s";
  units_["spread_alt"] = "m";
}

const AlertThresholds& AlertThresholds::INVALID() {
  static AlertThresholds athr;
  return athr;
}

bool AlertThresholds::isValid() const {
  return detector_ != NULL && region_ != BandsRegion::UNKNOWN;
}

AlertThresholds::~AlertThresholds() {
  delete detector_;
}

void AlertThresholds::copyFrom(const AlertThresholds& athr) {
  if (&athr != this) {
    if (detector_ != NULL) {
      delete detector_;
    }
    detector_ = athr.isValid() ? athr.detector_->copy() : NULL;
    alerting_time_ = athr.alerting_time_;
    early_alerting_time_ = athr.early_alerting_time_;
    region_ = athr.region_;
    spread_hdir_ = athr.spread_hdir_;
    spread_hs_ = athr.spread_hs_;
    spread_vs_ = athr.spread_vs_;
    spread_alt_ = athr.spread_alt_;
    units_ = athr.units_;
  }
}

AlertThresholds& AlertThresholds::operator=(const AlertThresholds& athr) {
  copyFrom(athr);
  return *this;
}

/**
 * Return detector.
 */
Detection3D* AlertThresholds::getCoreDetectionPtr() const {
  return detector_;
}

Detection3D& AlertThresholds::getCoreDetectionRef() const {
  return *detector_;
}

/**
 * Set detector.
 */
void AlertThresholds::setCoreDetectionRef(const Detection3D& det) {
  setCoreDetectionPtr(&det);
}

/**
 * Set detector.
 */
void AlertThresholds::setCoreDetectionPtr(const Detection3D* det) {
  if (detector_ != NULL) {
    delete detector_;
  }
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
double AlertThresholds::getHorizontalDirectionSpread() const {
  return spread_hdir_;
}

/**
 * Get track spread in given units [u]. Spread is relative to ownship's track
 */
double AlertThresholds::getHorizontalDirectionSpread(const std::string& u) const {
  return Units::to(u,spread_hdir_);
}

/**
 * Set track spread in internal units. Spread is relative to ownship's track and is expected
 * to be in [0,pi].
 */
void AlertThresholds::setHorizontalDirectionSpread(double spread) {
  spread_hdir_ = std::abs(Util::to_pi(spread));
}

/**
 * Set track spread in given units. Spread is relative to ownship's track and is expected
 * to be in [0,pi] [u].
 */
void AlertThresholds::setHorizontalDirectionSpread(double spread, const std::string& u) {
  setHorizontalDirectionSpread(Units::from(u,spread));
  units_["spread_hdir"] = u;
}

/**
 * Get ground speed spread in internal units [m/s]. Spread is relative to ownship's ground speed
 */
double AlertThresholds::getHorizontalSpeedSpread() const {
  return spread_hs_;
}

/**
 * Get ground speed spread in given units. Spread is relative to ownship's ground speed
 */
double AlertThresholds::getHorizontalSpeedSpread(const std::string& u) const {
  return Units::to(u,spread_hs_);
}

/**
 * Set ground speed spread in internal units [m/s]. Spread is relative to ownship's ground speed and is expected
 * to be non-negative
 */
void AlertThresholds::setHorizontalSpeedSpread(double spread) {
  spread_hs_ = std::abs(spread);
}

/**
 * Set ground speed spread in given units. Spread is relative to ownship's ground speed and is expected
 * to be non-negative
 */
void AlertThresholds::setHorizontalSpeedSpread(double spread, const std::string& u) {
  setHorizontalSpeedSpread(Units::from(u,spread));
  units_["spread_hs"] = u;
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
  return "volume = "+(detector_ == NULL ? "INVALID_DETECTOR" : detector_->toString())+
      ", alerting_time = "+Units::str(getUnits("alerting_time"),alerting_time_)+
      ", early_alerting_time = "+Units::str(getUnits("early_alerting_time"),early_alerting_time_)+
      ", region = "+BandsRegion::to_string(region_)+
      ", spread_hdir = "+Units::str(getUnits("spread_hdir"),spread_hdir_)+
      ", spread_hs = "+Units::str(getUnits("spread_hs"),spread_hs_)+
      ", spread_vs = "+Units::str(getUnits("spread_vs"),spread_vs_)+
      ", spread_alt = "+Units::str(getUnits("spread_alt"),spread_alt_);
}

std::string AlertThresholds:: toPVS() const {
  return "(# volume:= "+(detector_ == NULL ? "INVALID_DETECTOR" :detector_->toPVS())+
      ", alerting_time:= "+FmPrecision(alerting_time_)+
      ", early_alerting_time:= "+FmPrecision(early_alerting_time_)+
      ", region:= "+BandsRegion::to_string(region_)+
      ", spread_hdir:= ("+FmPrecision(spread_hdir_)+","+FmPrecision(spread_hdir_)+")"+
      ", spread_hs:= ("+FmPrecision(spread_hs_)+","+FmPrecision(spread_hs_)+")"+
      ", spread_vs:= ("+FmPrecision(spread_vs_)+","+FmPrecision(spread_vs_)+")"+
      ", spread_alt:= ("+FmPrecision(spread_alt_)+","+FmPrecision(spread_alt_)+")"+
      " #)";
}

ParameterData AlertThresholds::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void AlertThresholds::updateParameterData(ParameterData& p) const {
  p.set("region",BandsRegion::to_string(region_));
  p.setInternal("alerting_time",alerting_time_,getUnits("alerting_time"));
  p.setInternal("early_alerting_time",early_alerting_time_,getUnits("early_alerting_time"));
  p.setInternal("spread_hdir",spread_hdir_,getUnits("spread_hdir"));
  p.setInternal("spread_hs",spread_hs_,getUnits("spread_hs"));
  p.setInternal("spread_vs",spread_vs_,getUnits("spread_vs"));
  p.setInternal("spread_alt",spread_alt_,getUnits("spread_alt"));
  if (detector_ != NULL) {
    p.set("detector",detector_->getIdentifier());
  }
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
  if (p.contains("spread_hdir")) {
    setHorizontalDirectionSpread(p.getValue("spread_hdir"));
    units_["spread_hdir"] = p.getUnit("spread_hdir");
  } else if (p.contains("spread_trk")) {
    setHorizontalDirectionSpread(p.getValue("spread_trk"));
    units_["spread_hdir"] = p.getUnit("spread_trk");
  }
  if (p.contains("spread_hs")) {
    setHorizontalSpeedSpread(p.getValue("spread_hs"));
    units_["spread_hs"] = p.getUnit("spread_hs");
  } else if (p.contains("spread_gs")) {
    setHorizontalSpeedSpread(p.getValue("spread_gs"));
    units_["spread_hs"] = p.getUnit("spread_gs");
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
