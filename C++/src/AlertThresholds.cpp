/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "AlertThresholds.h"
#include "Vect3.h"
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
AlertThresholds::AlertThresholds(const Detection3D* det,
    double alerting_time, double early_alerting_time,
    BandsRegion::Region region) {
  detector_ = det->copy();
  alerting_time_ = std::abs(alerting_time);
  early_alerting_time_ = std::max(alerting_time_,early_alerting_time);
  region_ = region;
  spread_trk_ = 0;
  spread_gs_ = 0;
  spread_vs_ = 0;
  spread_alt_ = 0;
}

AlertThresholds::AlertThresholds(const AlertThresholds& athr) {
  detector_ = athr.detector_->copy();
  alerting_time_ = athr.alerting_time_;
  early_alerting_time_ = athr.early_alerting_time_;
  region_ = athr.region_;
  spread_trk_ = athr.spread_trk_;
  spread_gs_ = athr.spread_gs_;
  spread_vs_ = athr.spread_vs_;
  spread_alt_ = athr.spread_alt_;
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
}

const AlertThresholds AlertThresholds::INVALID = AlertThresholds();

bool AlertThresholds::isValid() const {
  return detector_ != NULL && region_ != BandsRegion::UNKNOWN;
}

AlertThresholds::~AlertThresholds() {
  delete detector_;
}

AlertThresholds& AlertThresholds::operator=(const AlertThresholds& athr) {
  detector_ = athr.detector_->copy();
  alerting_time_ = athr.alerting_time_;
  early_alerting_time_ = athr.early_alerting_time_;
  region_ = athr.region_;
  spread_trk_ = athr.spread_trk_;
  spread_gs_ = athr.spread_gs_;
  spread_vs_ = athr.spread_vs_;
  spread_alt_ = athr.spread_alt_;
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
  detector_ = det->copy();
}

/**
 * Return alerting time in seconds.
 */
double AlertThresholds::getAlertingTime() const {
  return alerting_time_;
}

/**
 * Set alerting time in seconds. Alerting time is non-negative number.
 */
void AlertThresholds::setAlertingTime(double val) {
  alerting_time_ = std::abs(val);
}

/**
 * Return early alerting time in seconds.
 */
double AlertThresholds::getEarlyAlertingTime() const {
  return early_alerting_time_;
}

/**
 * Set early alerting time in seconds. Early alerting time is a positive number >= alerting time
 */
void AlertThresholds::setEarlyAlertingTime(double t) {
  early_alerting_time_ = std::abs(t);
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
  return Units::to(u,getTrackSpread());
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
  return Units::to(u,getGroundSpeedSpread());
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
  return Units::to(u,getVerticalSpeedSpread());
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
  return Units::to(u,getAltitudeSpread());
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
}

std::string AlertThresholds::toString() const {
  return detector_->toString()+", Alerting Time: "+Fm1(alerting_time_)+
      " [s], Early Alerting Time: "+Fm1(early_alerting_time_)+
      " [s], Region: "+BandsRegion::to_string(region_)+
      ", Track Spread: "+Fm1(Units::to("deg",spread_trk_))+
      " [deg] , Ground Speed Spread: "+Fm1(Units::to("knot",spread_gs_))+
      " [knot], Vertical Speed Spread: "+Fm1(Units::to("fpm",spread_vs_))+
      " [fpm], Altitude Spread: "+Fm1(Units::to("ft",spread_alt_))+
      " [ft]";
}

std::string AlertThresholds:: toPVS(int prec) const {
  return "(# wcv:= "+detector_->toPVS(prec)+
      ", alerting_time:= "+Fm1(alerting_time_)+
      ", early_alerting_time:= "+Fm1(early_alerting_time_)+
      ", region:= "+BandsRegion::to_string(region_)+
      ", spread_trk:= ("+FmPrecision(spread_trk_,prec)+","+FmPrecision(spread_trk_,prec)+")"+
      ", spread_gs:= ("+FmPrecision(spread_gs_,prec)+","+FmPrecision(spread_gs_,prec)+")"+
      ", spread_vs:= ("+FmPrecision(spread_vs_,prec)+","+FmPrecision(spread_vs_,prec)+")"+
      ", spread_alt:= ("+FmPrecision(spread_alt_,prec)+","+FmPrecision(spread_alt_,prec)+")"+
      " #)";
}

}
