/*
 * Copyright (c) 2015-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusParameters.h"
#include "ParameterData.h"
#include "StateReader.h"
#include "Units.h"
#include "Util.h"
#include "Alerter.h"
#include "ACCoRDConfig.h"
#include "string_util.h"
#include "format.h"
#include "Constants.h"
#include "WCV_TAUMOD_SUM.h"
#include <vector>
#include <map>

namespace larcfm {

const std::string DaidalusParameters::VERSION = "2.0.2";
const INT64FM DaidalusParameters::ALMOST_ = PRECISION5;
bool DaidalusParameters::initialized = false;

void DaidalusParameters::init() {
  if (!initialized) {
    Constants::set_output_precision(10);
    Constants::set_trailing_zeros(false);
    initialized = true;
  }
}

/* NOTE: By default, no alert levels are configured */
DaidalusParameters::DaidalusParameters() : error("DaidalusParameters") {

  // Bands Parameters
  lookahead_time_ = 180.0; // [s]
  units_["lookahead_time"] = "s";

  left_hdir_  = Units::from("deg",180.0);
  units_["left_hdir"] = "deg";

  right_hdir_ = Units::from("deg",180.0);
  units_["right_hdir"] = "deg";

  min_hs_  = Units::from("knot",10.0);
  units_["min_hs"] = "knot";

  max_hs_  = Units::from("knot",700.0);
  units_["max_hs"] = "knot";

  min_vs_  = Units::from("fpm",-6000.0);
  units_["min_vs"] = "fpm";

  max_vs_  = Units::from("fpm",6000.0);
  units_["max_vs"] = "fpm";

  min_alt_ = Units::from("ft",100.0);
  units_["min_alt"] = "ft";

  max_alt_ = Units::from("ft",50000.0);
  units_["max_alt"] = "ft";

  // Relative Bands
  below_relative_hs_ = 0.0;
  units_["below_relative_hs"] = "knot";

  above_relative_hs_ = 0.0;
  units_["above_relative_hs"] = "knot";

  below_relative_vs_ = 0.0;
  units_["below_relative_vs"] = "fpm";

  above_relative_vs_ = 0.0;
  units_["above_relative_vs"] = "fpm";

  below_relative_alt_ = 0.0;
  units_["below_relative_alt"] = "ft";

  above_relative_alt_ = 0.0;
  units_["above_relative_alt"] = "ft";

  // Kinematic Parameters
  step_hdir_ = Units::from("deg",1.0);
  units_["step_hdir"] = "deg";

  step_hs_ = Units::from("knot",5.0);
  units_["step_hs"] = "knot";

  step_vs_ = Units::from("fpm",100.0);
  units_["step_vs"] = "fpm";

  step_alt_ = Units::from("ft", 100.0);
  units_["step_alt"] = "ft";

  horizontal_accel_ = Units::from("m/s^2",2.0);
  units_["horizontal_accel"] = "m/s^2";

  vertical_accel_ = Units::from("G",0.25);    // Section 1.2.3, DAA MOPS V3.6
  units_["vertical_accel"] = "G";

  turn_rate_ = Units::from("deg/s",3.0); // Section 1.2.3, DAA MOPS V3.6
  units_["turn_rate"] = "deg/s";

  bank_angle_ = 0.0;
  units_["bank_angle"] = "deg";

  vertical_rate_ = Units::from("fpm",500.0);   // Section 1.2.3, DAA MOPS V3.6
  units_["vertical_rate"] = "fpm";

  // Recovery Bands Parameters
  min_horizontal_recovery_ = 0.0;
  units_["min_horizontal_recovery"] = "nmi";

  min_vertical_recovery_ = 0.0;
  units_["min_vertical_recovery"] = "ft";

  recovery_hdir_ = true;

  recovery_hs_ = true;

  recovery_vs_ = true;

  recovery_alt_ = true;

  // Collision Avoidance Bands Parameters
  ca_bands_ = false;

  ca_factor_ = 0.1;

  horizontal_nmac_ = ACCoRDConfig::NMAC_D;      // Defined in RTCA SC-147
  units_["horizontal_nmac"] = "ft";

  vertical_nmac_ = ACCoRDConfig::NMAC_H;        // Defined in RTCA SC-147
  units_["vertical_nmac"] = "ft";

  // Hysteresis and persistence parameters
  recovery_stability_time_ = 3.0; // [s]
  units_["recovery_stability_time"] = "s";

  hysteresis_time_ = 0.0; // [s]
  units_["hysteresis_time"] = "s";

  persistence_time_ = 0.0; // [s]
  units_["persistence_time"] = "s";

  bands_persistence_ = false;

  persistence_preferred_hdir_ = Units::from("deg",0.0);
  units_["persistence_preferred_hdir"] = "deg";

  persistence_preferred_hs_ = Units::from("knot",0.0);
  units_["persistence_preferred_hs"] = "knot";

  persistence_preferred_vs_ = Units::from("fpm",0.0);
  units_["persistence_preferred_vs"] = "fpm";

  persistence_preferred_alt_ = Units::from("ft",0.0);
  units_["persistence_preferred_alt"] = "ft";

  alerting_m_ = 0;
  alerting_n_ = 0;

  // Implicit Coordination Parameters
  conflict_crit_ = false;

  recovery_crit_ = false;

  // SUM parameters
  h_pos_z_score_ = 0.0;

  h_vel_z_score_min_ = 0.0;

  h_vel_z_score_max_ = 0.0;

  h_vel_z_distance_ = 0.0;
  units_["h_vel_z_distance"] = "nmi";

  v_pos_z_score_ = 0.0;

  v_vel_z_score_ = 0.0;

  // Horizontal Contour Threshold
  contour_thr_ = Units::from("deg",180.0);
  units_["contour_thr"] = "deg";

  // DAA Terminal Area (DTA)
  dta_logic_ = 0;
  dta_latitude_ = 0.0;
  units_["dta_latitude"] = "deg";
  dta_longitude_ = 0.0;
  units_["dta_longitude"] = "deg";
  dta_radius_ = 0.0;
  units_["dta_radius"] = "nmi";
  dta_height_ = 0.0;
  units_["dta_height"] = "ft";
  dta_alerter_  = 0;

  // Alerting logic
  ownship_centric_alerting_ = true;

  corrective_region_ = BandsRegion::NEAR;

  init();
}

const DaidalusParameters::aliasestype& DaidalusParameters::getAliases() {
  static aliasestype aliases_;
  if (aliases_.empty()) {
    // Aliases of parameters (for compatibility between different versions of DAIDALUS)
    aliases_["left_hdir"].push_back("left_trk");
    aliases_["right_hdir"].push_back("right_trk");
    aliases_["min_hs"].push_back("min_gs");
    aliases_["max_hs"].push_back("max_gs");
    aliases_["step_hdir"].push_back("trk_step");
    aliases_["step_hs"].push_back("gs_step");
    aliases_["step_vs"].push_back("vs_step");
    aliases_["step_alt"].push_back("alt_step");
    aliases_["recovery_hdir"].push_back("recovery_trk");
    aliases_["recovery_hs"].push_back("recovery_gs");
    aliases_["hysteresis_time"].push_back("resolution_hysteresis_time");
    aliases_["persistence_preferred_hdir"].push_back("max_delta_resolution_hdir");
    aliases_["persistence_preferred_hs"].push_back("max_delta_resolution_hs");
    aliases_["persistence_preferred_vs"].push_back("max_delta_resolution_vs");
    aliases_["persistence_preferred_alt"].push_back("max_delta_resolution_alt");

  }
  return aliases_;
}

DaidalusParameters::~DaidalusParameters() {}

int DaidalusParameters::numberOfAlerters() const {
  return alerters_.size();
}

const Alerter& DaidalusParameters::getAlerterAt(int i) const {
  if (1 <= i && i <= static_cast<int>(alerters_.size())) {
    return alerters_[i-1];
  } else {
    return Alerter::INVALID();
  }
}

int DaidalusParameters::getAlerterIndex(const std::string& id) const {
  for (int i=0; i < static_cast<int>(alerters_.size()); ++i) {
    if (equals(id, alerters_[i].getId())) {
      return i+1;
    }
  }
  return 0;
}

void DaidalusParameters::clearAlerters() {
  alerters_.clear();
}

/**
 * Add alerter (if id of alerter already exists, replaces alerter with new one).
 * Return index of added alerter
 */
int DaidalusParameters::addAlerter(const Alerter& alerter) {
  int i = getAlerterIndex(alerter.getId());
  if (i == 0) {
    alerters_.push_back(alerter);
    i = alerters_.size();
  } else {
    alerters_[i-1] = alerter;
  }
  set_alerter_with_SUM_parameters(alerters_[i-1]);
  return i;
}

/** This method is needed because WCV_TAUMOD_SUM doesn't require the
 *  user to initialize SUM parameters, which may be specified globally.
 */
void DaidalusParameters::set_alerter_with_SUM_parameters(Alerter& alerter) {
  for (int level=1; level <= alerter.mostSevereAlertLevel(); ++level) {
    Detection3D* det = alerter.getDetectorPtr(level);
    if (det != NULL && larcfm::equals(det->getSimpleClassName(),"WCV_TAUMOD_SUM")) {
      ((WCV_TAUMOD_SUM*)det)->set_global_SUM_parameters(*this);
    }
  }
}

/** This method is needed because WCV_TAUMOD_SUM doesn't require the
 *  user to initialize SUM parameters, which may be specified globally.
 */
void DaidalusParameters::set_alerters_with_SUM_parameters() {
  for (int i=0; i < static_cast<int>(alerters_.size()); ++i) {
    set_alerter_with_SUM_parameters(alerters_[i]);
  }
}

double DaidalusParameters::getLookaheadTime() const {
  return lookahead_time_;
}

double DaidalusParameters::getLookaheadTime(const std::string& u) const {
  return Units::to(u,getLookaheadTime());
}

double DaidalusParameters::getLeftHorizontalDirection() const {
  return left_hdir_;
}


double DaidalusParameters::getLeftHorizontalDirection(const std::string& u) const {
  return Units::to(u,getLeftHorizontalDirection());
}


double DaidalusParameters::getRightHorizontalDirection() const {
  return right_hdir_;
}


double DaidalusParameters::getRightHorizontalDirection(const std::string& u) const {
  return Units::to(u,getRightHorizontalDirection());
}


double DaidalusParameters::getMinHorizontalSpeed() const {
  return min_hs_;
}


double DaidalusParameters::getMinHorizontalSpeed(const std::string& u) const {
  return Units::to(u,getMinHorizontalSpeed());
}


double DaidalusParameters::getMaxHorizontalSpeed() const {
  return max_hs_;
}


double DaidalusParameters::getMaxHorizontalSpeed(const std::string& u) const {
  return Units::to(u,getMaxHorizontalSpeed());
}


double DaidalusParameters::getMinVerticalSpeed() const {
  return min_vs_;
}


double DaidalusParameters::getMinVerticalSpeed(const std::string& u) const {
  return Units::to(u,getMinVerticalSpeed());
}


double DaidalusParameters::getMaxVerticalSpeed() const {
  return max_vs_;
}


double DaidalusParameters::getMaxVerticalSpeed(const std::string& u) const {
  return Units::to(u,getMaxVerticalSpeed());
}


double DaidalusParameters::getMinAltitude() const {
  return min_alt_;
}


double DaidalusParameters::getMinAltitude(const std::string& u) const {
  return Units::to(u,getMinAltitude());
}


double DaidalusParameters::getMaxAltitude() const {
  return max_alt_;
}


double DaidalusParameters::getMaxAltitude(const std::string& u) const {
  return Units::to(u,getMaxAltitude());
}


double DaidalusParameters::getBelowRelativeHorizontalSpeed() const {
  return below_relative_hs_;
}


double DaidalusParameters::getBelowRelativeHorizontalSpeed(const std::string& u) const {
  return Units::to(u,getBelowRelativeHorizontalSpeed());
}


double DaidalusParameters::getAboveRelativeHorizontalSpeed() const {
  return above_relative_hs_;
}


double DaidalusParameters::getAboveRelativeHorizontalSpeed(const std::string& u) const {
  return Units::to(u,getAboveRelativeHorizontalSpeed());
}


double DaidalusParameters::getBelowRelativeVerticalSpeed() const {
  return below_relative_vs_;
}


double DaidalusParameters::getBelowRelativeVerticalSpeed(const std::string& u) const {
  return Units::to(u,getBelowRelativeVerticalSpeed());
}


double DaidalusParameters::getAboveRelativeVerticalSpeed() const {
  return above_relative_vs_;
}


double DaidalusParameters::getAboveRelativeVerticalSpeed(const std::string& u) const {
  return Units::to(u,getAboveRelativeVerticalSpeed());
}


double DaidalusParameters::getBelowRelativeAltitude() const {
  return below_relative_alt_;
}


double DaidalusParameters::getBelowRelativeAltitude(const std::string& u) const {
  return Units::to(u,getBelowRelativeAltitude());
}


double DaidalusParameters::getAboveRelativeAltitude() const {
  return above_relative_alt_;
}


double DaidalusParameters::getAboveRelativeAltitude(const std::string& u) const {
  return Units::to(u,getAboveRelativeAltitude());
}


double DaidalusParameters::getHorizontalDirectionStep() const {
  return step_hdir_;
}


double DaidalusParameters::getHorizontalDirectionStep(const std::string& u) const {
  return Units::to(u,getHorizontalDirectionStep());
}


double DaidalusParameters::getHorizontalSpeedStep() const {
  return step_hs_;
}


double DaidalusParameters::getHorizontalSpeedStep(const std::string& u) const {
  return Units::to(u,getHorizontalSpeedStep());
}


double DaidalusParameters::getVerticalSpeedStep() const {
  return step_vs_;
}


double DaidalusParameters::getVerticalSpeedStep(const std::string& u) const {
  return Units::to(u,getVerticalSpeedStep());
}


double DaidalusParameters::getAltitudeStep() const {
  return step_alt_;
}


double DaidalusParameters::getAltitudeStep(const std::string& u) const {
  return Units::to(u,getAltitudeStep());
}


double DaidalusParameters::getHorizontalAcceleration() const {
  return horizontal_accel_;
}


double DaidalusParameters::getHorizontalAcceleration(const std::string& u) const {
  return Units::to(u,getHorizontalAcceleration());
}


double DaidalusParameters::getVerticalAcceleration() const {
  return vertical_accel_;
}


double DaidalusParameters::getVerticalAcceleration(const std::string& u) const {
  return Units::to(u,getVerticalAcceleration());
}


double DaidalusParameters::getTurnRate() const {
  return turn_rate_;
}


double DaidalusParameters::getTurnRate(const std::string& u) const {
  return Units::to(u,getTurnRate());
}


double DaidalusParameters::getBankAngle() const {
  return bank_angle_;
}


double DaidalusParameters::getBankAngle(const std::string& u) const {
  return Units::to(u,getBankAngle());
}


double DaidalusParameters::getVerticalRate() const {
  return vertical_rate_;
}


double DaidalusParameters::getVerticalRate(const std::string& u) const {
  return Units::to(u,getVerticalRate());
}


double DaidalusParameters::getHorizontalNMAC() const {
  return horizontal_nmac_;
}


double DaidalusParameters::getHorizontalNMAC(const std::string& u) const {
  return Units::to(u,getHorizontalNMAC());
}


double DaidalusParameters::getVerticalNMAC() const {
  return vertical_nmac_;
}


double DaidalusParameters::getVerticalNMAC(const std::string& u) const {
  return Units::to(u,getVerticalNMAC());
}


double DaidalusParameters::getRecoveryStabilityTime() const {
  return recovery_stability_time_;
}


double DaidalusParameters::getRecoveryStabilityTime(const std::string& u) const {
  return Units::to(u,getRecoveryStabilityTime());
}

/**
 * @return hysteresis time in seconds.
 */
double DaidalusParameters::getHysteresisTime() const {
  return hysteresis_time_;
}

/**
 * @return hysteresis time in specified units [u]
 */
double DaidalusParameters::getHysteresisTime(const std::string& u) const {
  return Units::to(u,getHysteresisTime());
}

double DaidalusParameters::getPersistenceTime() const {
  return persistence_time_;
}

double DaidalusParameters::getPersistenceTime(const std::string& u) const {
  return Units::to(u,getPersistenceTime());
}

/**
 * @return true if bands persistence is enabled
 */
bool DaidalusParameters::isEnabledBandsPersistence() const {
  return bands_persistence_;
}

/**
 * Enable/disable bands persistence
 */
void DaidalusParameters::setBandsPersistence(bool flag) {
  bands_persistence_ = flag;
}

/**
 * Enable bands persistence
 */
void DaidalusParameters::enableBandsPersistence() {
  setBandsPersistence(true);
}

/**
 * Disable bands persistence
 */
void DaidalusParameters::disableBandsPersistence() {
  setBandsPersistence(false);
}

double DaidalusParameters::getPersistencePreferredHorizontalDirectionResolution() const {
  return persistence_preferred_hdir_;
}


double DaidalusParameters::getPersistencePreferredHorizontalDirectionResolution(const std::string& u) const {
  return Units::to(u,getPersistencePreferredHorizontalDirectionResolution());
}


double DaidalusParameters::getPersistencePreferredHorizontalSpeedResolution() const {
  return persistence_preferred_hs_;
}


double DaidalusParameters::getPersistencePreferredHorizontalSpeedResolution(const std::string& u) const {
  return Units::to(u,getPersistencePreferredHorizontalSpeedResolution());
}


double DaidalusParameters::getPersistencePreferredVerticalSpeedResolution() const {
  return persistence_preferred_vs_;
}


double DaidalusParameters::getPersistencePreferredVerticalSpeedResolution(const std::string& u) const {
  return Units::to(u,getPersistencePreferredVerticalSpeedResolution());
}


double DaidalusParameters::getPersistencePreferredAltitudeResolution() const {
  return persistence_preferred_alt_;
}


double DaidalusParameters::getPersistencePreferredAltitudeResolution(const std::string& u) const {
  return Units::to(u,getPersistencePreferredAltitudeResolution());
}

/**
 * @return Alerting parameter m of "M of N" strategy
 */
int DaidalusParameters::getAlertingParameterM() const {
  return alerting_m_;
}

/**
 * @return Alerting parameter n of "M of N" strategy
 */
int DaidalusParameters::getAlertingParameterN() const {
  return alerting_n_;
}

double DaidalusParameters::getMinHorizontalRecovery() const {
  return min_horizontal_recovery_;
}


double DaidalusParameters::getMinHorizontalRecovery(const std::string& u) const {
  return Units::to(u,getMinHorizontalRecovery());
}


double DaidalusParameters::getMinVerticalRecovery() const {
  return min_vertical_recovery_;
}


double DaidalusParameters::getMinVerticalRecovery(const std::string& u) const {
  return Units::to(u,getMinVerticalRecovery());
}


bool DaidalusParameters::setLookaheadTime(double val) {
  if (error.isPositive("setLookaheadTime",val)) {
    lookahead_time_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setLookaheadTime(double val, const std::string& u) {
  if (setLookaheadTime(Units::from(u,val))) {
    units_["lookahead_time"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setLeftHorizontalDirection(double val) {
  left_hdir_ = std::abs(Util::to_pi(val));
  return true;
}


bool DaidalusParameters::setLeftHorizontalDirection(double val, const std::string& u) {
  units_["left_hdir"] = u;
  return setLeftHorizontalDirection(Units::from(u,val));
}


bool DaidalusParameters::setRightHorizontalDirection(double val) {
  right_hdir_ = std::abs(Util::to_pi(val));
  return true;
}


bool DaidalusParameters::setRightHorizontalDirection(double val, const std::string& u) {
  units_["right_hdir"] = u;
  return setRightHorizontalDirection(Units::from(u,val));
}


bool DaidalusParameters::setMinHorizontalSpeed(double val) {
  if (error.isNonNegative("setMinHorizontalSpeed",val)) {
    min_hs_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMinHorizontalSpeed(double val, const std::string& u) {
  if (setMinHorizontalSpeed(Units::from(u,val))) {
    units_["min_hs"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMaxHorizontalSpeed(double val) {
  if (error.isNonNegative("setMaxHorizontalSpeed",val)) {
    max_hs_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMaxHorizontalSpeed(double val, const std::string& u) {
  if (setMaxHorizontalSpeed(Units::from(u,val))) {
    units_["max_hs"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMinVerticalSpeed(double val) {
  min_vs_ = val;
  return true;
}


bool DaidalusParameters::setMinVerticalSpeed(double val, const std::string& u) {
  units_["min_vs"] = u;
  return setMinVerticalSpeed(Units::from(u,val));
}


bool DaidalusParameters::setMaxVerticalSpeed(double val) {
  max_vs_ = val;
  return true;
}


bool DaidalusParameters::setMaxVerticalSpeed(double val, const std::string& u) {
  units_["max_vs"] = u;
  return setMaxVerticalSpeed(Units::from(u,val));
}


bool DaidalusParameters::setMinAltitude(double val) {
  if (error.isNonNegative("setMinAltitude",val)) {
    min_alt_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMinAltitude(double val, const std::string& u) {
  if (setMinAltitude(Units::from(u,val))) {
    units_["min_alt"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMaxAltitude(double val) {
  if (error.isPositive("setMaxAltitude",val)) {
    max_alt_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMaxAltitude(double val, const std::string& u) {
  if (setMaxAltitude(Units::from(u,val))) {
    units_["max_alt"] = u;
    return true;
  }
  return false;
}


void DaidalusParameters::setBelowRelativeHorizontalSpeed(double val) {
  below_relative_hs_ = val;
}


void DaidalusParameters::setBelowRelativeHorizontalSpeed(double val,const std::string& u) {
  units_["below_relative_hs"] = u;
  setBelowRelativeHorizontalSpeed(Units::from(u, val));
}


void DaidalusParameters::setAboveRelativeHorizontalSpeed(double val) {
  above_relative_hs_ = val;
}


void DaidalusParameters::setAboveRelativeHorizontalSpeed(double val, const std::string& u) {
  units_["above_relative_hs"] = u;
  setAboveRelativeHorizontalSpeed(Units::from(u, val));
}


void DaidalusParameters::setBelowRelativeVerticalSpeed(double val) {
  below_relative_vs_ = val;
}


void DaidalusParameters::setBelowRelativeVerticalSpeed(double val, const std::string& u) {
  units_["below_relative_vs"] = u;
  setBelowRelativeVerticalSpeed(Units::from(u, val));
}


void DaidalusParameters::setAboveRelativeVerticalSpeed(double val) {
  above_relative_vs_ = val;
}


void DaidalusParameters::setAboveRelativeVerticalSpeed(double val, const std::string& u) {
  units_["above_relative_vs"] = u;
  setAboveRelativeVerticalSpeed(Units::from(u, val));
}


void DaidalusParameters::setBelowRelativeAltitude(double val) {
  below_relative_alt_ = val;
}


void DaidalusParameters::setBelowRelativeAltitude(double val, const std::string& u) {
  units_["below_relative_alt"] = u;
  setBelowRelativeAltitude(Units::from(u, val));
}


void DaidalusParameters::setAboveRelativeAltitude(double val) {
  above_relative_alt_ = val;
}


void DaidalusParameters::setAboveRelativeAltitude(double val, const std::string& u) {
  units_["above_relative_alt"] = u;
  setAboveRelativeAltitude(Units::from(u, val));
}


void DaidalusParameters::setBelowToMinRelativeHorizontalSpeed() {
  below_relative_hs_ = -1;
}


void DaidalusParameters::setAboveToMaxRelativeHorizontalSpeed() {
  above_relative_hs_ = -1;
}


void DaidalusParameters::setBelowToMinRelativeVerticalSpeed() {
  below_relative_vs_ = -1;
}


void DaidalusParameters::setAboveToMaxRelativeVerticalSpeed() {
  above_relative_vs_ = -1;
}


void DaidalusParameters::setBelowToMinRelativeAltitude() {
  below_relative_alt_ = -1;
}


void DaidalusParameters::setAboveToMaxRelativeAltitude() {
  above_relative_alt_ = -1;
}


void DaidalusParameters::disableRelativeHorizontalSpeedBands() {
  below_relative_hs_ = 0;
  above_relative_hs_ = 0;
}


void DaidalusParameters::disableRelativeVerticalSpeedBands() {
  below_relative_vs_ = 0;
  above_relative_vs_ = 0;
}


void DaidalusParameters::disableRelativeAltitude() {
  below_relative_alt_ = 0;
  above_relative_alt_ = 0;
}


bool DaidalusParameters::setHorizontalDirectionStep(double val) {
  if (error.isPositive("setDirectionStep",val) &&
      error.isLessThan("setDirectionStep",val,Pi)) {
    step_hdir_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setHorizontalDirectionStep(double val, const std::string& u) {
  if (setHorizontalDirectionStep(Units::from(u,val))) {
    units_["step_hdir"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setHorizontalSpeedStep(double val) {
  if (error.isPositive("setHorizontalSpeedStep",val)) {
    step_hs_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setHorizontalSpeedStep(double val, const std::string& u) {
  if (setHorizontalSpeedStep(Units::from(u,val))) {
    units_["step_hs"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setVerticalSpeedStep(double val) {
  if (error.isPositive("setVerticalSpeedStep",val)) {
    step_vs_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setVerticalSpeedStep(double val, const std::string& u) {
  if (setVerticalSpeedStep(Units::from(u,val))) {
    units_["step_vs"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setAltitudeStep(double val) {
  if (error.isPositive("setAltitudeStep",val)) {
    step_alt_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setAltitudeStep(double val, const std::string& u) {
  if (setAltitudeStep(Units::from(u,val))) {
    units_["step_alt"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setHorizontalAcceleration(double val) {
  if (error.isNonNegative("setHorizontalAcceleration",val)) {
    horizontal_accel_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setHorizontalAcceleration(double val, const std::string& u) {
  if (setHorizontalAcceleration(Units::from(u,val))) {
    units_["horizontal_accel"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setVerticalAcceleration(double val) {
  if (error.isNonNegative("setVerticalAcceleration",val)) {
    vertical_accel_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setVerticalAcceleration(double val, const std::string& u) {
  if (setVerticalAcceleration(Units::from(u,val))) {
    units_["vertical_accel"] = u;
    return true;
  }
  return false;
}

bool DaidalusParameters::set_turn_rate(double val) {
  if (error.isNonNegative("setTurnRate",val)) {
    turn_rate_ = val;
    return true;
  }
  return false;
}

bool DaidalusParameters::set_bank_angle(double val) {
  if (error.isNonNegative("setBankAngle",val)) {
    bank_angle_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setTurnRate(double val) {
  if (set_turn_rate(val)) {
    set_bank_angle(0.0);
    return true;
  }
  return false;
}


bool DaidalusParameters::setTurnRate(double val, const std::string& u) {
  if (setTurnRate(Units::from(u,val))) {
    units_["turn_rate"] = u;
    return true;
  }
  return false;
}

bool DaidalusParameters::setBankAngle(double val) {
  if (set_bank_angle(val)) {
    set_turn_rate(0.0);
    return true;
  }
  return false;
}


bool DaidalusParameters::setBankAngle(double val, const std::string& u) {
  if (setBankAngle(Units::from(u,val))) {
    units_["bank_angle"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setVerticalRate(double val) {
  if (error.isNonNegative("setVerticalRate",val)) {
    vertical_rate_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setVerticalRate(double val, const std::string& u) {
  if (setVerticalRate(Units::from(u,val))) {
    units_["vertical_rate"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setHorizontalNMAC(double val) {
  if (error.isNonNegative("setHorizontalNMAC",val)) {
    horizontal_nmac_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setHorizontalNMAC(double val, const std::string& u) {
  if (setHorizontalNMAC(Units::from(u,val))) {
    units_["horizontal_nmac"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setVerticalNMAC(double val) {
  if (error.isNonNegative("setVerticalNMAC",val)) {
    vertical_nmac_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setVerticalNMAC(double val, const std::string& u) {
  if (setVerticalNMAC(Units::from(u,val))) {
    units_["vertical_nmac"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setRecoveryStabilityTime(double val) {
  if (error.isNonNegative("setRecoveryStabilityTime",val)) {
    recovery_stability_time_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setRecoveryStabilityTime(double val, const std::string& u) {
  if (setRecoveryStabilityTime(Units::from(u,val))) {
    units_["recovery_stability_time"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setHysteresisTime(double val) {
  if (error.isNonNegative("setHysteresisTime",val)) {
    hysteresis_time_ = val;
    return true;
  }
  return false;
}

bool DaidalusParameters::setHysteresisTime(double val, const std::string& u) {
  if (setHysteresisTime(Units::from(u,val))) {
    units_["hysteresis_time"] = u;
    return true;
  }
  return false;
}

bool DaidalusParameters::setPersistenceTime(double val) {
  if (error.isNonNegative("setPersistenceTime",val)) {
    persistence_time_ = val;
    return true;
  }
  return false;
}

bool DaidalusParameters::setPersistenceTime(double val, const std::string& u) {
  if (setPersistenceTime(Units::from(u,val))) {
    units_["persistence_time"] = u;
    return true;
  }
  return false;
}

bool DaidalusParameters::setPersistencePreferredHorizontalDirectionResolution(double val) {
  if (error.isNonNegative("persistence_preferred_hdir",val)) {
    persistence_preferred_hdir_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setPersistencePreferredHorizontalDirectionResolution(double val, const std::string& u) {
  if (setPersistencePreferredHorizontalDirectionResolution(Units::from(u,val))) {
    units_["persistence_preferred_hdir"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setPersistencePreferredHorizontalSpeedResolution(double val) {
  if (error.isNonNegative("persistence_preferred_hs",val)) {
    persistence_preferred_hs_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setPersistencePreferredHorizontalSpeedResolution(double val, const std::string& u) {
  if (setPersistencePreferredHorizontalSpeedResolution(Units::from(u,val))){
    units_["persistence_preferred_hs"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setPersistencePreferredVerticalSpeedResolution(double val) {
  if (error.isNonNegative("persistence_preferred_vs",val)) {
    persistence_preferred_vs_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setPersistencePreferredVerticalSpeedResolution(double val, const std::string& u) {
  if (setPersistencePreferredVerticalSpeedResolution(Units::from(u,val))) {
    units_["persistence_preferred_vs"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setPersistencePreferredAltitudeResolution(double val) {
  if (error.isNonNegative("persistence_preferred_alt",val)) {
    persistence_preferred_alt_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setPersistencePreferredAltitudeResolution(double val, const std::string& u) {
  if (setPersistencePreferredAltitudeResolution(Units::from(u,val))) {
    units_["persistence_preferred_alt"] = u;
    return true;
  }
  return false;
}

/**
 * Set alerting parameters of M of N strategy
 */
bool DaidalusParameters::setAlertingMofN(int m, int n) {
  if (m >= 0 && n >= 0) {
    alerting_m_ = m;
    alerting_n_ = n;
    return true;
  }
  return false;
}


/**
 * Set alerting parameter m of "M of N" strategy
 */
bool DaidalusParameters::set_alerting_parameterM(int m) {
  if (m >= 0) {
    alerting_m_ = m;
    return true;
  }
  return false;
}

/**
 * Set alerting parameter n of "M of N" strategy
 */
bool DaidalusParameters::set_alerting_parameterN(int n) {
  if (n >= 0) {
    alerting_n_ = n;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMinHorizontalRecovery(double val) {
  if (error.isNonNegative("setMinHorizontalRecovery",val)) {
    min_horizontal_recovery_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMinHorizontalRecovery(double val, const std::string& u) {
  if (setMinHorizontalRecovery(Units::from(u,val))) {
    units_["min_horizontal_recovery"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMinVerticalRecovery(double val) {
  if (error.isNonNegative("setMinVerticalRecovery",val)) {
    min_vertical_recovery_ = val;
    return true;
  }
  return false;
}


bool DaidalusParameters::setMinVerticalRecovery(double val, const std::string& u) {
  if (setMinVerticalRecovery(Units::from(u,val))) {
    units_["min_vertical_recovery"] = u;
    return true;
  }
  return false;
}


bool DaidalusParameters::isEnabledConflictCriteria() const {
  return conflict_crit_;
}


void DaidalusParameters::setConflictCriteria(bool flag) {
  conflict_crit_ = flag;
}


void DaidalusParameters::enableConflictCriteria() {
  setConflictCriteria(true);
}


void DaidalusParameters::disableConflictCriteria() {
  setConflictCriteria(false);
}


bool DaidalusParameters::isEnabledRecoveryCriteria() const {
  return recovery_crit_;
}


void DaidalusParameters::setRecoveryCriteria(bool flag) {
  recovery_crit_ = flag;
}


void DaidalusParameters::enableRecoveryCriteria() {
  setRecoveryCriteria(true);
}


void DaidalusParameters::disableRecoveryCriteria() {
  setRecoveryCriteria(false);
}


void DaidalusParameters::setRepulsiveCriteria(bool flag) {
  setConflictCriteria(flag);
  setRecoveryCriteria(flag);
}


void DaidalusParameters::enableRepulsiveCriteria() {
  setRepulsiveCriteria(true);
}


void DaidalusParameters::disableRepulsiveCriteria() {
  setRepulsiveCriteria(false);
}


bool DaidalusParameters::isEnabledRecoveryHorizontalDirectionBands() const {
  return recovery_hdir_;
}


bool DaidalusParameters::isEnabledRecoveryHorizontalSpeedBands() const {
  return recovery_hs_;
}


bool DaidalusParameters::isEnabledRecoveryVerticalSpeedBands() const {
  return recovery_vs_;
}


bool DaidalusParameters::isEnabledRecoveryAltitudeBands() const {
  return recovery_alt_;
}


void DaidalusParameters::setRecoveryBands(bool flag) {
  setRecoveryHorizontalDirectionBands(flag);
  setRecoveryHorizontalSpeedBands(flag);
  setRecoveryVerticalSpeedBands(flag);
  setRecoveryAltitudeBands(flag);
}


void DaidalusParameters::enableRecoveryBands() {
  setRecoveryBands(true);
}


void DaidalusParameters::disableRecoveryBands() {
  setRecoveryBands(false);
}


void DaidalusParameters::setRecoveryHorizontalDirectionBands(bool flag) {
  recovery_hdir_ = flag;
}


void DaidalusParameters::setRecoveryHorizontalSpeedBands(bool flag) {
  recovery_hs_ = flag;
}


void DaidalusParameters::setRecoveryVerticalSpeedBands(bool flag) {
  recovery_vs_ = flag;
}


void DaidalusParameters::setRecoveryAltitudeBands(bool flag) {
  recovery_alt_ = flag;
}


bool DaidalusParameters::isEnabledCollisionAvoidanceBands() const {
  return ca_bands_ && ca_factor_ > 0;
}


void DaidalusParameters::setCollisionAvoidanceBands(bool flag) {
  ca_bands_ = flag;
}


void DaidalusParameters::enableCollisionAvoidanceBands() {
  setCollisionAvoidanceBands(true);
}


void DaidalusParameters::disableCollisionAvoidanceBands() {
  setCollisionAvoidanceBands(false);
}


double DaidalusParameters::getCollisionAvoidanceBandsFactor() const {
  return ca_factor_;
}


bool DaidalusParameters::setCollisionAvoidanceBandsFactor(double val) {
  if (error.isNonNegative("setCollisionAvoidanceBandsFactor",val) &&
      error.isLessThan("setCollisionAvoidanceBandsFactor", val,1)) {
    ca_factor_ = val;
    return true;
  }
  return false;
}


double DaidalusParameters::getHorizontalPositionZScore() const {
  return h_pos_z_score_;
}


bool DaidalusParameters::setHorizontalPositionZScore(double val) {
  if (error.isNonNegative("setHorizontalPositionZScore",val)) {
    h_pos_z_score_ = val;
    set_alerters_with_SUM_parameters();
    return true;
  }
  return false;
}


double DaidalusParameters::getHorizontalVelocityZScoreMin() const {
  return h_vel_z_score_min_;
}


bool DaidalusParameters::setHorizontalVelocityZScoreMin(double val) {
  if (error.isNonNegative("setHorizontalVelocityZScoreMin",val)) {
    h_vel_z_score_min_ = val;
    set_alerters_with_SUM_parameters();
    return true;
  }
  return false;
}


double DaidalusParameters::getHorizontalVelocityZScoreMax() const {
  return h_vel_z_score_max_;
}


bool DaidalusParameters::setHorizontalVelocityZScoreMax(double val) {
  if (error.isNonNegative("setHorizontalVelocityZScoreMax",val)) {
    h_vel_z_score_max_ = val;
    set_alerters_with_SUM_parameters();
    return true;
  }
  return false;
}


double DaidalusParameters::getHorizontalVelocityZDistance() const {
  return h_vel_z_distance_;
}


bool DaidalusParameters::setHorizontalVelocityZDistance(double val) {
  if (error.isNonNegative("setHorizontalVelocityZDistance",val)) {
    h_vel_z_distance_ = val;
    set_alerters_with_SUM_parameters();
    return true;
  }
  return false;
}


double DaidalusParameters::getHorizontalVelocityZDistance(const std::string& u) const {
  return Units::to(u,h_vel_z_distance_);
}


bool DaidalusParameters::setHorizontalVelocityZDistance(double val, const std::string& u) {
  if (setHorizontalVelocityZDistance(Units::from(u,val))) {
    units_["h_vel_z_distance"] = u;
    return true;
  }
  return false;
}


double DaidalusParameters::getVerticalPositionZScore() const {
  return v_pos_z_score_;
}


bool DaidalusParameters::setVerticalPositionZScore(double val) {
  if (error.isNonNegative("setVerticalPositionZScore",val)) {
    v_pos_z_score_ = val;
    set_alerters_with_SUM_parameters();
    return true;
  }
  return false;
}


double DaidalusParameters::getVerticalSpeedZScore() const {
  return v_vel_z_score_;
}


bool DaidalusParameters::setVerticalSpeedZScore(double val) {
  if (error.isNonNegative("setVerticalSpeedZScore",val)) {
    v_vel_z_score_ = val;
    set_alerters_with_SUM_parameters();
    return true;
  }
  return false;
}


double DaidalusParameters::getHorizontalContourThreshold() const {
  return contour_thr_;
}


double DaidalusParameters::getHorizontalContourThreshold(const std::string& u) const {
  return Units::to(u,getHorizontalContourThreshold());
}


bool DaidalusParameters::setHorizontalContourThreshold(double val) {
  contour_thr_ = std::abs(Util::to_pi(val));
  return error.isBetween("setHorizontalContourThreshold",val,0,Pi);
}


bool DaidalusParameters::setHorizontalContourThreshold(double val, const std::string& u) {
  units_["contour_thr"] = u;
  return setHorizontalContourThreshold(Units::from(u,val));
}

/**
 * DTA Logic:
 * 0: Disabled
 * 1: Enabled special DTA maneuver guidance. Horizontal recovery is fully enabled,
 * but vertical recovery blocks down resolutions when alert is higher than corrective.
 * -1: Enabled special DTA maneuver guidance. Horizontal recovery is disabled,
 * vertical recovery blocks down resolutions when raw alert is higher than corrective.
 * NOTE:
 * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
 * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
 * intruder-centric logic).
 */
int DaidalusParameters::getDTALogic() const {
  return dta_logic_;
}

/**
 * DTA Logic:
 * 0: Disabled
 * 1: Enabled special DTA maneuver guidance. Horizontal recovery is fully enabled,
 * but vertical recovery blocks down resolutions when alert is higher than corrective.
 * -1: Enabled special DTA maneuver guidance. Horizontal recovery is disabled,
 * vertical recovery blocks down resolutions when raw alert is higher than corrective.
 * NOTE:
 * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
 * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
 * intruder-centric logic).
 */
void DaidalusParameters::setDTALogic(int dta_logic) {
  dta_logic_ = Util::signTriple(dta_logic);
}

/**
 * Get DAA Terminal Area (DTA) position (lat/lon)
 */
const Position& DaidalusParameters::getDTAPosition() const {
  static Position dta_position;
  if (dta_latitude_ != dta_position.lat() ||
      dta_longitude_ != dta_position.lon()) {
    std::string ulat = getUnitsOf("dta_latitude");
    std::string ulon = getUnitsOf("dta_longitude");
    if (Units::isCompatible(ulat,ulon)) {
      if (Units::isCompatible("m",ulat)) {
        dta_position = Position::mkXYZ(dta_latitude_,dta_longitude_,0.0);
      } else if (Units::isCompatible("deg",ulat)) {
        dta_position = Position::mkLatLonAlt(dta_latitude_,dta_longitude_,0.0);
      } else {
        dta_position = Position::INVALID();
      }
    } else {
      dta_position = Position::INVALID();
    }
  }
  return dta_position;
}

/**
 * Set DAA Terminal Area (DTA) latitude (internal units)
 */
void DaidalusParameters::setDTALatitude(double lat) {
  dta_latitude_ = lat;
}

/**
 * Set DAA Terminal Area (DTA) latitude in given units
 */
void DaidalusParameters::setDTALatitude(double lat, const std::string& ulat) {
  setDTALatitude(Units::from(ulat,lat));
  units_["dta_latitude"] =  ulat;

}

/**
 * Set DAA Terminal Area (DTA) longitude (internal units)
 */
void DaidalusParameters::setDTALongitude(double lon) {
  dta_longitude_ = lon;
}

/**
 * Set DAA Terminal Area (DTA) longitude in given units
 */
void DaidalusParameters::setDTALongitude(double lon, const std::string& ulon) {
  setDTALongitude(Units::from(ulon,lon));
  units_["dta_longitude"] = ulon;
}

/**
 * Get DAA Terminal Area (DTA) radius (internal units)
 */
double DaidalusParameters::getDTARadius() const {
  return dta_radius_;
}

/**
 * Get DAA Terminal Area (DTA) radius in given units
 */
double DaidalusParameters::getDTARadius(const std::string& u) const {
  return Units::to(u,dta_radius_);
}

/**
 * Set DAA Terminal Area (DTA) radius (internal units)
 */
void DaidalusParameters::setDTARadius(double val) {
  dta_radius_ = val;
}

/**
 * Set DAA Terminal Area (DTA) radius in given units
 */
void DaidalusParameters::setDTARadius(double val, const std::string& u) {
  setDTARadius(Units::from(u,val));
  units_["dta_radius"] = u;
}

/**
 * Get DAA Terminal Area (DTA) height (internal units)
 */
double DaidalusParameters::getDTAHeight() const {
  return dta_height_;
}

/**
 * Get DAA Terminal Area (DTA) height in given units
 */
double DaidalusParameters::getDTAHeight(const std::string& u) const {
  return Units::to(u,dta_height_);
}

/**
 * Set DAA Terminal Area (DTA) height (internal units)
 */
void DaidalusParameters::setDTAHeight(double val) {
  dta_height_ = val;
}

/**
 * Set DAA Terminal Area (DTA) height in given units
 */
void DaidalusParameters::setDTAHeight(double val, const std::string& u) {
  setDTAHeight(Units::from(u,val));
  units_["dta_height"] = u;
}

/**
 * Get DAA Terminal Area (DTA) alerter
 */
int DaidalusParameters::getDTAAlerter() const {
  return dta_alerter_;
}

/**
 * Set DAA Terminal Area (DTA) alerter
 */
void DaidalusParameters::setDTAAlerter(int alerter) {
  dta_alerter_ = alerter;
}

void DaidalusParameters::setAlertingLogic(bool ownship_centric) {
  ownship_centric_alerting_ = ownship_centric;
}


void DaidalusParameters::setOwnshipCentricAlertingLogic() {
  setAlertingLogic(true);
}


void DaidalusParameters::setIntruderCentricAlertingLogic() {
  setAlertingLogic(false);
}


bool DaidalusParameters::isAlertingLogicOwnshipCentric() const {
  return ownship_centric_alerting_;
}

/**
 * Get corrective region for calculation of resolution maneuvers and bands saturation.
 */
BandsRegion::Region DaidalusParameters::getCorrectiveRegion() const {
  return corrective_region_;
}

/**
 * Set corrective region for calculation of resolution maneuvers and bands saturation.
 */
bool DaidalusParameters::setCorrectiveRegion(BandsRegion::Region val) {
  if (BandsRegion::isConflictBand(val)) {
    corrective_region_ = val;
    return true;
  } else {
    error.addError("setCorrectiveRegion: "+BandsRegion::to_string(val)+" is not a conflict region");
    return false;
  }
}

/**
 * @param alerter_idx Indice of an alerter (starting from 1)
 * @return corrective level of alerter at alerter_idx. The corrective level
 * is the first alert level that has a region equal to or more severe than corrective_region.
 * Return -1 if alerter_idx is out of range or if there is no corrective alert level
 * for this alerter.
 */
int DaidalusParameters::correctiveAlertLevel(int alerter_idx) {
  if (1 <= alerter_idx && alerter_idx <= static_cast<int>(alerters_.size())) {
    Alerter alerter = alerters_[alerter_idx-1];
    return alerter.alertLevelForRegion(corrective_region_);
  } else {
    error.addError("correctiveAlertLevel: alerter_idx ("+Fmi(alerter_idx)+") is out of range");
    return -1;
  }
}

/**
 * @return maximum alert level for all alerters. Returns 0 if alerter list is empty.
 */
int DaidalusParameters::maxAlertLevel() const {
  int maxalert_level = 0;
  for (int alerter_idx=1; alerter_idx <= static_cast<int>(alerters_.size()); ++alerter_idx) {
    maxalert_level = std::max(maxalert_level,alerters_[alerter_idx-1].mostSevereAlertLevel());
  }
  return maxalert_level;
}

/**
 * Set instantaneous bands.
 */
void DaidalusParameters::setInstantaneousBands() {
  set_turn_rate(0.0);
  set_bank_angle(0.0);
  setHorizontalAcceleration(0.0);
  setVerticalAcceleration(0.0);
  setVerticalRate(0.0);
}

/**
 * Set kinematic bands.
 * Set turn rate to 3 deg/s, when type is true; set turn rate to  1.5 deg/s
 * when type is false;
 */
void DaidalusParameters::setKinematicBands(bool type) {
  // Section 1.2.3 in RTCA DO-365
  setTurnRate(type ? 3.0 : 1.5,"deg/s");
  setHorizontalAcceleration(2.0,"m/s^2");
  setVerticalAcceleration(0.25,"G");
  setVerticalRate(500.0,"fpm");
}


void DaidalusParameters::disableHysteresis() {
  setHysteresisTime(0.0);
  disableBandsPersistence();
  setPersistencePreferredHorizontalDirectionResolution(0.0);
  setPersistencePreferredHorizontalSpeedResolution(0.0);
  setPersistencePreferredVerticalSpeedResolution(0.0);
  setPersistencePreferredAltitudeResolution(0.0);
  setPersistenceTime(0.0);
  setAlertingMofN(0,0);
}


bool DaidalusParameters::loadFromFile(const std::string& file) {
  StateReader reader;
  reader.open(file);
  ParameterData parameters;
  reader.updateParameterData(parameters);
  setParameters(parameters);
  return !reader.hasError();
}


bool DaidalusParameters::saveToFile(const std::string& file) {
  std::ofstream out;
  out.open(file.c_str());
  if ( out.fail() ) {
    error.addError("saveToFile: File "+file+" is protected");
    return false;
  }
  fp(&out,toString());
  out.close();
  return true;
}

/**
 * The following method set default output precision and enable/disable trailing zeros.
 * It doesn't affect computations.
 */
void DaidalusParameters::setDefaultOutputPrecision(int precision, bool trailing_zeros) {
  Constants::set_output_precision(precision);
  Constants::set_trailing_zeros(trailing_zeros);
  initialized = true;
}

std::string DaidalusParameters::toString() const {
  std::string s = "# V-"+VERSION+"\n";
  ParameterData p;
  updateParameterData(p);
  s+=p.listToString(p.getKeyListEntryOrder());
  return s;
}

std::string DaidalusParameters::toPVS() const {
  std::string s = "";
  s+="(# ";
  s+="lookahead_time := "+FmPrecision(lookahead_time_)+", ";
  s+="left_hdir := "+FmPrecision(left_hdir_)+", ";
  s+="right_hdir := "+FmPrecision(right_hdir_)+", ";
  s+="min_hs := "+FmPrecision(min_hs_)+", ";
  s+="max_hs := "+FmPrecision(max_hs_)+", ";
  s+="min_vs := "+FmPrecision(min_vs_)+", ";
  s+="max_vs := "+FmPrecision(max_vs_)+", ";
  s+="min_alt := "+FmPrecision(min_alt_)+", ";
  s+="max_alt := "+FmPrecision(max_alt_)+", ";
  s+="below_relative_hs := "+FmPrecision(below_relative_hs_)+", ";
  s+="above_relative_hs := "+FmPrecision(above_relative_hs_)+", ";
  s+="below_relative_vs := "+FmPrecision(below_relative_vs_)+", ";
  s+="above_relative_vs := "+FmPrecision(above_relative_vs_)+", ";
  s+="below_relative_alt := "+FmPrecision(below_relative_alt_)+", ";
  s+="above_relative_alt := "+FmPrecision(above_relative_alt_)+", ";
  s+="step_hdir := "+FmPrecision(step_hdir_)+", ";
  s+="step_hs := "+FmPrecision(step_hs_)+", ";
  s+="step_vs := "+FmPrecision(step_vs_)+", ";
  s+="step_alt := "+FmPrecision(step_alt_)+", ";
  s+="horizontal_accel := "+FmPrecision(horizontal_accel_)+", ";
  s+="vertical_accel := "+FmPrecision(vertical_accel_)+", ";
  s+="turn_rate := "+FmPrecision(turn_rate_)+", ";
  s+="bank_angle := "+FmPrecision(bank_angle_)+", ";
  s+="vertical_rate := "+FmPrecision(vertical_rate_)+", ";
  s+="min_horizontal_recovery := "+FmPrecision(min_horizontal_recovery_)+", ";
  s+="min_vertical_recovery := "+FmPrecision(min_vertical_recovery_)+", ";
  s+="recovery_hdir := "+Fmb(recovery_hdir_)+", ";
  s+="recovery_hs := "+Fmb(recovery_hs_)+", ";
  s+="recovery_vs := "+Fmb(recovery_vs_)+", ";
  s+="recovery_alt := "+Fmb(recovery_alt_)+", ";
  s+="ca_bands := "+Fmb(ca_bands_)+", ";
  s+="ca_factor := "+FmPrecision(ca_factor_)+", ";
  s+="horizontal_nmac :="+FmPrecision(horizontal_nmac_)+", ";
  s+="vertical_nmac :="+FmPrecision(vertical_nmac_)+", ";
  s+="recovery_stability_time := "+FmPrecision(recovery_stability_time_)+", ";
  s+="hysteresis_time := "+FmPrecision(hysteresis_time_)+", ";
  s+="persistence_time := "+FmPrecision(persistence_time_)+", ";
  s+="bands_persistence := "+Fmb(bands_persistence_)+", ";
  s+="persistence_preferred_hdir := "+FmPrecision(persistence_preferred_hdir_)+", ";
  s+="persistence_preferred_hs := "+FmPrecision(persistence_preferred_hs_)+", ";
  s+="persistence_preferred_vs := "+FmPrecision(persistence_preferred_vs_)+", ";
  s+="persistence_preferred_alt := "+FmPrecision(persistence_preferred_alt_)+", ";
  s+="alert_m := "+Fmi(alerting_m_)+", ";
  s+="alert_n := "+Fmi(alerting_n_)+", ";
  s+="conflict_crit := "+Fmb(conflict_crit_)+", ";
  s+="recovery_crit := "+Fmb(recovery_crit_)+", ";
  s+="h_pos_z_score := "+FmPrecision(h_pos_z_score_)+", ";
  s+="h_vel_z_score_min := "+FmPrecision(h_vel_z_score_min_)+", ";
  s+="h_vel_z_score_max := "+FmPrecision(h_vel_z_score_max_)+", ";
  s+="h_vel_z_distance := "+FmPrecision(h_vel_z_distance_)+", ";
  s+="v_pos_z_score := "+FmPrecision(v_pos_z_score_)+", ";
  s+="v_vel_z_score := "+FmPrecision(v_vel_z_score_)+", ";
  s+="contour_thr := "+FmPrecision(contour_thr_)+", ";
  s+="dta_logic := "+Fmi(dta_logic_)+", ";
  s+="dta_latitude := "+FmPrecision(dta_latitude_)+", ";
  s+="dta_longitude := "+FmPrecision(dta_longitude_)+", ";
  s+="dta_radius := "+FmPrecision(dta_radius_)+", ";
  s+="dta_height := "+FmPrecision(dta_height_)+", ";
  s+="dta_alerter := "+Fmi(dta_alerter_)+", ";
  s+="ownship_centric_alerting := "+Fmb(ownship_centric_alerting_)+", ";
  s+="corrective_region := "+BandsRegion::to_string(corrective_region_)+", ";
  s+="alerters := "+Alerter::listToPVS(alerters_);
  s+="#)";
  return s;
}

ParameterData DaidalusParameters::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

/**
 * Return a ParameterData suitable to be read by readAlerterList() based on the supplied Alerter instances.
 * This is a cosmetic alteration to allow for the string representation to have parameters grouped together.
 */
void DaidalusParameters::writeAlerterList(ParameterData& p) const {
  std::vector<std::string> names;
  std::vector<Alerter>::const_iterator alerter_ptr;
  for (alerter_ptr = alerters_.begin(); alerter_ptr != alerters_.end(); ++alerter_ptr) {
    names.push_back(alerter_ptr->getId());
  }
  p.set("alerters",names);
  for (alerter_ptr = alerters_.begin(); alerter_ptr != alerters_.end(); ++alerter_ptr) {
    p.copy(alerter_ptr->getParameters().copyWithPrefix(alerter_ptr->getId()+"_"),true);
  }
}

void DaidalusParameters::updateParameterData(ParameterData& p) const {
  // Bands Parameters
  p.setInternal("lookahead_time", lookahead_time_, getUnitsOf("lookahead_time"));
  p.updateComment("lookahead_time","Bands Parameters");
  p.setInternal("left_hdir", left_hdir_, getUnitsOf("left_hdir"));
  p.setInternal("right_hdir", right_hdir_, getUnitsOf("right_hdir"));
  p.setInternal("min_hs", min_hs_, getUnitsOf("min_hs"));
  p.setInternal("max_hs", max_hs_, getUnitsOf("max_hs"));
  p.setInternal("min_vs", min_vs_, getUnitsOf("min_vs"));
  p.setInternal("max_vs", max_vs_, getUnitsOf("max_vs"));
  p.setInternal("min_alt", min_alt_, getUnitsOf("min_alt"));
  p.setInternal("max_alt", max_alt_, getUnitsOf("max_alt"));

  // Relative Bands
  p.setInternal("below_relative_hs",below_relative_hs_, getUnitsOf("below_relative_hs"));
  p.updateComment("below_relative_hs", "Relative Bands Parameters");
  p.setInternal("above_relative_hs",above_relative_hs_, getUnitsOf("above_relative_hs"));
  p.setInternal("below_relative_vs",below_relative_vs_, getUnitsOf("below_relative_vs"));
  p.setInternal("above_relative_vs",above_relative_vs_, getUnitsOf("above_relative_vs"));
  p.setInternal("below_relative_alt",below_relative_alt_, getUnitsOf("below_relative_alt"));
  p.setInternal("above_relative_alt",above_relative_alt_, getUnitsOf("above_relative_alt"));

  // Kinematic Parameters
  p.setInternal("step_hdir", step_hdir_, getUnitsOf("step_hdir"));
  p.updateComment("step_hdir","Kinematic Parameters");
  p.setInternal("step_hs", step_hs_, getUnitsOf("step_hs"));
  p.setInternal("step_vs", step_vs_, getUnitsOf("step_vs"));
  p.setInternal("step_alt", step_alt_, getUnitsOf("step_alt"));
  p.setInternal("horizontal_accel", horizontal_accel_, getUnitsOf("horizontal_accel"));
  p.setInternal("vertical_accel", vertical_accel_, getUnitsOf("vertical_accel"));
  p.setInternal("turn_rate", turn_rate_, getUnitsOf("turn_rate"));
  p.setInternal("bank_angle", bank_angle_, getUnitsOf("bank_angle"));
  p.setInternal("vertical_rate", vertical_rate_, getUnitsOf("vertical_rate"));

  // Recovery Bands Parameters
  p.setInternal("min_horizontal_recovery", min_horizontal_recovery_, getUnitsOf("min_horizontal_recovery"));
  p.updateComment("min_horizontal_recovery","Recovery Bands Parameters");
  p.setInternal("min_vertical_recovery", min_vertical_recovery_, getUnitsOf("min_vertical_recovery"));
  p.setBool("recovery_hdir", recovery_hdir_);
  p.setBool("recovery_hs", recovery_hs_);
  p.setBool("recovery_vs", recovery_vs_);
  p.setBool("recovery_alt", recovery_alt_);

  // Collision Avoidance Bands Parameters
  p.setBool("ca_bands", ca_bands_);
  p.updateComment("ca_bands","Collision Avoidance Bands Parameters");
  p.setInternal("ca_factor", ca_factor_, "unitless");
  p.setInternal("horizontal_nmac",horizontal_nmac_, getUnitsOf("horizontal_nmac"));
  p.setInternal("vertical_nmac",vertical_nmac_, getUnitsOf("vertical_nmac"));

  // Hysteresis and persistence parameters
  p.setInternal("recovery_stability_time", recovery_stability_time_, getUnitsOf("recovery_stability_time"));
  p.updateComment("recovery_stability_time","Hysteresis and persistence parameters");
  p.setInternal("hysteresis_time", hysteresis_time_, getUnitsOf("hysteresis_time"));
  p.setInternal("persistence_time", persistence_time_, getUnitsOf("persistence_time"));
  p.setBool("bands_persistence",bands_persistence_);
  p.setInternal("persistence_preferred_hdir", persistence_preferred_hdir_, getUnitsOf("persistence_preferred_hdir"));
  p.setInternal("persistence_preferred_hs", persistence_preferred_hs_, getUnitsOf("persistence_preferred_hs"));
  p.setInternal("persistence_preferred_vs", persistence_preferred_vs_, getUnitsOf("persistence_preferred_vs"));
  p.setInternal("persistence_preferred_alt", persistence_preferred_alt_, getUnitsOf("persistence_preferred_alt"));
  p.setInt("alerting_m",alerting_m_);
  p.setInt("alerting_n",alerting_n_);

  // Implicit Coordination Parameters
  p.setBool("conflict_crit", conflict_crit_);
  p.updateComment("conflict_crit","Implicit Coordination Parameters");
  p.setBool("recovery_crit", recovery_crit_);

  // SUM parameters
  p.setInternal("h_pos_z_score", h_pos_z_score_, "unitless");
  p.updateComment("h_pos_z_score","Sensor Uncertainty Mitigation Parameters");
  p.setInternal("h_vel_z_score_min", h_vel_z_score_min_, "unitless");
  p.setInternal("h_vel_z_score_max", h_vel_z_score_max_, "unitless");
  p.setInternal("h_vel_z_distance", h_vel_z_distance_, getUnitsOf("h_vel_z_distance"));
  p.setInternal("v_pos_z_score", v_pos_z_score_, "unitless");
  p.setInternal("v_vel_z_score", v_vel_z_score_, "unitless");

  // Horizontal Contour Threshold
  p.setInternal("contour_thr", contour_thr_, getUnitsOf("contour_thr"));
  p.updateComment("contour_thr","Horizontal Contour Threshold");

  // DAA Terminal Area (DTA)
  p.setInt("dta_logic", dta_logic_);
  p.updateComment("dta_logic","DAA Terminal Area (DTA)");
  p.setInternal("dta_latitude", dta_latitude_, getUnitsOf("dta_latitude"));
  p.setInternal("dta_longitude", dta_longitude_, getUnitsOf("dta_longitude"));
  p.setInternal("dta_radius", dta_radius_, getUnitsOf("dta_radius"));
  p.setInternal("dta_height", dta_height_, getUnitsOf("dta_height"));
  p.setInt("dta_alerter", dta_alerter_);

  // Alerting logic
  p.setBool("ownship_centric_alerting",ownship_centric_alerting_);
  p.updateComment("ownship_centric_alerting","Alerting Logic");
  p.set("corrective_region",BandsRegion::to_string(corrective_region_));
  writeAlerterList(p);
}

bool DaidalusParameters::contains(const ParameterData& p, const std::string& key) {
  class Contains : public Function<const std::string&,bool> {
  private:
    const ParameterData& p_;
  public:
    Contains(const ParameterData& p) : p_(p) {}
    bool apply(const std::string& key) { return p_.contains(key); }
  };
  Contains f(p);
  return parameter_data<bool>(p,key,f);
}

std::string DaidalusParameters::getUnit(const ParameterData& p,const std::string& key) {
  class GetUnit : public Function<const std::string&,std::string> {
  private:
    const ParameterData& p_;
  public:
    GetUnit(const ParameterData& p) : p_(p) {}
    std::string apply(const std::string& key) { return p_.getUnit(key); }
  };
  GetUnit f(p);
  return parameter_data<std::string>(p,key,f);
}

double DaidalusParameters::getValue(const ParameterData& p,const std::string& key) {
  class GetValue : public Function<const std::string&,double> {
  private:
    const ParameterData& p_;
  public:
    GetValue(const ParameterData& p) : p_(p) {}
    double apply(const std::string& key) { return p_.getValue(key); }
  };
  GetValue f(p);
  return parameter_data<double>(p,key,f);
}

bool DaidalusParameters::getBool(const ParameterData& p,const std::string& key) {
  class GetBool : public Function<const std::string&,bool> {
  private:
    const ParameterData& p_;
  public:
    GetBool(const ParameterData& p) : p_(p) {}
    bool apply(const std::string& key) { return p_.getBool(key); }
  };
  GetBool f(p);
  return parameter_data<bool>(p,key,f);
}

int DaidalusParameters::getInt(const ParameterData& p,const std::string& key) {
  class GetInt : public Function<const std::string&,int> {
  private:
    const ParameterData& p_;
  public:
    GetInt(const ParameterData& p) : p_(p) {}
    int apply(const std::string& key) { return p_.getInt(key); }
  };
  GetInt f(p);
  return parameter_data<int>(p,key,f);
}

std::string DaidalusParameters::getString(const ParameterData& p,const std::string& key) {
  class GetString : public Function<const std::string&,std::string> {
  private:
    const ParameterData& p_;
  public:
    GetString(const ParameterData& p) : p_(p) {}
    std::string apply(const std::string& key) { return p_.getString(key); }
  };
  GetString f(p);
  return parameter_data<std::string>(p,key,f);
}

std::vector<std::string> DaidalusParameters::getListString(const ParameterData& p,const std::string& key) {
  class GetListString : public Function<const std::string&,std::vector<std::string> > {
  private:
    const ParameterData& p_;
  public:
    GetListString(const ParameterData& p) : p_(p) {}
    std::vector<std::string> apply(const std::string& key) { return p_.getListString(key); }
  };
  GetListString f(p);
  return parameter_data<std::vector<std::string> >(p,key,f);
}

void DaidalusParameters::readAlerterList(const std::vector<std::string>& alerter_list, const ParameterData& params) {
  alerters_.clear();
  for (int i = 0; i < static_cast<int>(alerter_list.size()); i++) {
    std::string id = alerter_list[i];
    ParameterData aPd = params.extractPrefix(id+"_");
    Alerter alerter(id);
    if (aPd.size() > 0) {
      alerter.setParameters(aPd);
    }
    alerters_.push_back(alerter);
  }
}

bool DaidalusParameters::setParameterData(const ParameterData& p) {
  bool setit = false;
  if (contains(p,"lookahead_time")) {
    setLookaheadTime(getValue(p,"lookahead_time"));
    units_["lookahead_time"] = getUnit(p,"lookahead_time");
    setit = true;
  }
  if (contains(p,"left_hdir")) {
    setLeftHorizontalDirection(getValue(p,"left_hdir"));
    units_["left_hdir"] = getUnit(p,"left_hdir");
    setit = true;
  }
  if (contains(p,"right_hdir")) {
    setRightHorizontalDirection(getValue(p,"right_hdir"));
    units_["right_hdir"] = getUnit(p,"right_hdir");
    setit = true;
  }
  if (contains(p,"min_hs")) {
    setMinHorizontalSpeed(getValue(p,"min_hs"));
    units_["min_hs"] = getUnit(p,"min_hs");
    setit = true;
  }
  if (contains(p,"max_hs")) {
    setMaxHorizontalSpeed(getValue(p,"max_hs"));
    units_["max_hs"] = getUnit(p,"max_hs");
    setit = true;
  }
  if (contains(p,"min_vs")) {
    setMinVerticalSpeed(getValue(p,"min_vs"));
    units_["min_vs"] = getUnit(p,"min_vs");
    setit = true;
  }
  if (contains(p,"max_vs")) {
    setMaxVerticalSpeed(getValue(p,"max_vs"));
    units_["max_vs"] = getUnit(p,"max_vs");
    setit = true;
  }
  if (contains(p,"min_alt")) {
    setMinAltitude(getValue(p,"min_alt"));
    units_["min_alt"] = getUnit(p,"min_alt");
    setit = true;
  }
  if (contains(p,"max_alt")) {
    setMaxAltitude(getValue(p,"max_alt"));
    units_["max_alt"] = getUnit(p,"max_alt");
    setit = true;
  }
  // Relative Bands
  if (contains(p,"below_relative_hs")) {
    setBelowRelativeHorizontalSpeed(getValue(p,"below_relative_hs"));
    units_["below_relative_hs"] = getUnit(p,"below_relative_hs");
    setit = true;
  }
  if (contains(p,"above_relative_hs")) {
    setAboveRelativeHorizontalSpeed(getValue(p,"above_relative_hs"));
    units_["above_relative_hs"] = getUnit(p,"above_relative_hs");
    setit = true;
  }
  if (contains(p,"below_relative_vs")) {
    setBelowRelativeVerticalSpeed(getValue(p,"below_relative_vs"));
    units_["below_relative_vs"] = getUnit(p,"below_relative_vs");
    setit = true;
  }
  if (contains(p,"above_relative_vs")) {
    setAboveRelativeVerticalSpeed(getValue(p,"above_relative_vs"));
    units_["above_relative_vs"] = getUnit(p,"above_relative_vs");
    setit = true;
  }
  if (contains(p,"below_relative_alt")) {
    setBelowRelativeAltitude(getValue(p,"below_relative_alt"));
    units_["below_relative_alt"] = getUnit(p,"below_relative_alt");
    setit = true;
  }
  if (contains(p,"above_relative_alt")) {
    setAboveRelativeAltitude(getValue(p,"above_relative_alt"));
    units_["above_relative_alt"] = getUnit(p,"above_relative_alt");
    setit = true;
  }
  // Kinematic bands
  if (contains(p,"step_hdir")) {
    setHorizontalDirectionStep(getValue(p,"step_hdir"));
    units_["step_hdir"] = getUnit(p,"step_hdir");
    setit = true;
  }
  if (contains(p,"step_hs")) {
    setHorizontalSpeedStep(getValue(p,"step_hs"));
    units_["step_hs"] = getUnit(p,"step_hs");
    setit = true;
  }
  if (contains(p,"step_vs")) {
    setVerticalSpeedStep(getValue(p,"step_vs"));
    units_["step_vs"] = getUnit(p,"step_vs");
    setit = true;
  }
  if (contains(p,"step_alt")) {
    setAltitudeStep(getValue(p,"step_alt"));
    units_["step_alt"] = getUnit(p,"step_alt");
    setit = true;
  }
  if (contains(p,"horizontal_accel")) {
    setHorizontalAcceleration(getValue(p,"horizontal_accel"));
    units_["horizontal_accel"] = getUnit(p,"horizontal_accel");
    setit = true;
  }
  if (contains(p,"vertical_accel")) {
    setVerticalAcceleration(getValue(p,"vertical_accel"));
    units_["vertical_accel"] = getUnit(p,"vertical_accel");
    setit = true;
  }
  if (contains(p,"turn_rate")) {
    set_turn_rate(getValue(p,"turn_rate"));
    units_["turn_rate"] = getUnit(p,"turn_rate");
    setit = true;
  }
  if (contains(p,"bank_angle")) {
    set_bank_angle(getValue(p,"bank_angle"));
    units_["bank_angle"] = getUnit(p,"bank_angle");
    setit = true;
  }
  if (contains(p,"vertical_rate")) {
    setVerticalRate(getValue(p,"vertical_rate"));
    units_["vertical_rate"] = getUnit(p,"vertical_rate");
    setit = true;
  }
  // Recovery bands
  if (contains(p,"min_horizontal_recovery")) {
    setMinHorizontalRecovery(getValue(p,"min_horizontal_recovery"));
    units_["min_horizontal_recovery"] = getUnit(p,"min_horizontal_recovery");
    setit = true;
  }
  if (contains(p,"min_vertical_recovery")) {
    setMinVerticalRecovery(getValue(p,"min_vertical_recovery"));
    units_["min_vertical_recovery"] = getUnit(p,"min_vertical_recovery");
    setit = true;
  }
  // Recovery parameters
  if (contains(p,"recovery_hdir")) {
    setRecoveryHorizontalDirectionBands(getBool(p,"recovery_hdir"));
    setit = true;
  }
  if (contains(p,"recovery_hs")) {
    setRecoveryHorizontalSpeedBands(getBool(p,"recovery_hs"));
    setit = true;
  }
  if (contains(p,"recovery_vs")) {
    setRecoveryVerticalSpeedBands(getBool(p,"recovery_vs"));
    setit = true;
  }
  if (contains(p,"recovery_alt")) {
    setRecoveryAltitudeBands(getBool(p,"recovery_alt"));
    setit = true;
  }
  // Collision avoidance
  if (contains(p,"ca_bands")) {
    setCollisionAvoidanceBands(getBool(p,"ca_bands"));
    setit = true;
  }
  if (contains(p,"ca_factor")) {
    setCollisionAvoidanceBandsFactor(getValue(p,"ca_factor"));
    setit = true;
  }
  if (contains(p,"horizontal_nmac")) {
    setHorizontalNMAC(getValue(p,"horizontal_nmac"));
    units_["horizontal_nmac"] = getUnit(p,"horizontal_nmac");
    setit = true;
  }
  if (contains(p,"vertical_nmac")) {
    setVerticalNMAC(getValue(p,"vertical_nmac"));
    units_["vertical_nmac"] = getUnit(p,"vertical_nmac");
    setit = true;
  }
  // Hysteresis and persistence parameters
  if (contains(p,"recovery_stability_time")) {
    setRecoveryStabilityTime(getValue(p,"recovery_stability_time"));
    units_["recovery_stability_time"] = getUnit(p,"recovery_stability_time");
    setit = true;
  }
  if (contains(p,"hysteresis_time")) {
    setHysteresisTime(getValue(p,"hysteresis_time"));
    units_["hysteresis_time"] = getUnit(p,"hysteresis_time");
    setit = true;
  }
  if (contains(p,"persistence_time")) {
    setPersistenceTime(getValue(p,"persistence_time"));
    units_["persistence_time"] = getUnit(p,"persistence_time");
    setit = true;
  }
  if (contains(p,"bands_persistence")) {
    setBandsPersistence(getBool(p,"bands_persistence"));
    setit = true;
  }
  if (contains(p,"persistence_preferred_hdir")) {
    setPersistencePreferredHorizontalDirectionResolution(getValue(p,"persistence_preferred_hdir"));
    units_["persistence_preferred_hdir"] = getUnit(p,"persistence_preferred_hdir");
    setit = true;
  }
  if (contains(p,"persistence_preferred_hs")) {
    setPersistencePreferredHorizontalSpeedResolution(getValue(p,"persistence_preferred_hs"));
    units_["persistence_preferred_hs"] = getUnit(p,"persistence_preferred_hs");
    setit = true;
  }
  if (contains(p,"persistence_preferred_vs")) {
    setPersistencePreferredVerticalSpeedResolution(getValue(p,"persistence_preferred_vs"));
    units_["persistence_preferred_vs"] = getUnit(p,"persistence_preferred_vs");
    setit = true;
  }
  if (contains(p,"persistence_preferred_alt")) {
    setPersistencePreferredAltitudeResolution(getValue(p,"persistence_preferred_alt"));
    units_["persistence_preferred_alt"] = getUnit(p,"persistence_preferred_alt");
    setit = true;
  }
  if (contains(p,"alerting_m")) {
    set_alerting_parameterM(getInt(p,"alerting_m"));
    setit = true;
  }
  if (contains(p,"alerting_n")) {
    set_alerting_parameterN(getInt(p,"alerting_n"));
    setit = true;
  }
  // Implicit Coordination
  if (contains(p,"conflict_crit")) {
    setConflictCriteria(getBool(p,"conflict_crit"));
    setit = true;
  }
  if (contains(p,"recovery_crit")) {
    setRecoveryCriteria(getBool(p,"recovery_crit"));
    setit = true;
  }
  // Sensor Uncertainty Mitigation
  if (contains(p,"h_pos_z_score")) {
    setHorizontalPositionZScore(getValue(p,"h_pos_z_score"));
    setit = true;
  }
  if (contains(p,"h_vel_z_score_min")) {
    setHorizontalVelocityZScoreMin(getValue(p,"h_vel_z_score_min"));
    setit = true;
  }
  if (contains(p,"h_vel_z_score_max")) {
    setHorizontalVelocityZScoreMax(getValue(p,"h_vel_z_score_max"));
    setit = true;
  }
  if (contains(p,"h_vel_z_distance")) {
    setHorizontalVelocityZDistance(getValue(p,"h_vel_z_distance"));
    units_["h_vel_z_distance"] = getUnit(p,"h_vel_z_distance");
    setit = true;
  }
  if (contains(p,"v_pos_z_score")) {
    setVerticalPositionZScore(getValue(p,"v_pos_z_score"));
    setit = true;
  }
  if (contains(p,"v_vel_z_score")) {
    setVerticalSpeedZScore(getValue(p,"v_vel_z_score"));
    setit = true;
  }
  // Contours
  if (contains(p,"contour_thr")) {
    setHorizontalContourThreshold(getValue(p,"contour_thr"));
    units_["contour_thr"] = getUnit(p,"contour_thr");
    setit = true;
  }
  // DAA Terminal Area (DTA)
  if (contains(p,"dta_logic")) {
    setDTALogic(getInt(p,"dta_logic"));
    setit = true;
  }
  if (contains(p,"dta_latitude")) {
    setDTALatitude(getValue(p,"dta_latitude"));
    units_["dta_latitude"] = getUnit(p,"dta_latitude");
    setit = true;
  }
  if (contains(p,"dta_longitude")) {
    setDTALongitude(getValue(p,"dta_longitude"));
    units_["dta_longitude"] = getUnit(p,"dta_longitude");
    setit = true;
  }
  if (contains(p,"dta_radius")) {
    setDTARadius(getValue(p,"dta_radius"));
    units_["dta_radius"] = getUnit(p,"dta_radius");
    setit = true;
  }
  if (contains(p,"dta_height")) {
    setDTAHeight(getValue(p,"dta_height"));
    units_["dta_height"] = getUnit(p,"dta_height");
    setit = true;
  }
  if (contains(p,"dta_alerter")) {
    setDTAAlerter(getInt(p,"dta_alerter"));
    setit = true;
  }
  // Alerting logic
  if (contains(p,"ownship_centric_alerting")) {
    setAlertingLogic(getBool(p,"ownship_centric_alerting"));
    setit = true;
  }
  bool daidalus_v1=false;
  // Corrective Region
  if (contains(p,"corrective_region")) {
    setCorrectiveRegion(BandsRegion::valueOf(getString(p,"corrective_region")));
    setit = true;
  } else if (contains(p,"conflict_level")) {
    daidalus_v1=true;
    setit = true;
  }
  // Alerters
  if (daidalus_v1) {
    Alerter alerter;
    alerter.setParameters(p);
    if (alerter.isValid()) {
      alerters_.clear();
      alerters_.push_back(alerter);
      int conflict_level=getInt(p,"conflict_level");
      if (1 <= conflict_level && conflict_level <= alerter.mostSevereAlertLevel()) {
        setCorrectiveRegion(alerter.getLevel(conflict_level).getRegion());
      }
    }
  } else {
    if (contains(p,"alerters")) {
      std::vector<std::string> alerter_list = getListString(p,"alerters");
      readAlerterList(alerter_list,p);
      setit = true;
    }
  }
  if (setit) {
    set_alerters_with_SUM_parameters();
  }
  return setit;
}

void DaidalusParameters::setParameters(const ParameterData& p) {
  setParameterData(p);
}

std::string DaidalusParameters::getUnitsOf(const std::string& key) const {
  std::map<std::string,std::string>::const_iterator find_ptr = units_.find(key);
  if (find_ptr != units_.end()) {
    return find_ptr->second;
  }
  aliasestype::const_iterator aliases_ptr;
  for (aliases_ptr = getAliases().begin(); aliases_ptr != getAliases().end(); ++aliases_ptr) {
    if (std::find(aliases_ptr->second.begin(),aliases_ptr->second.end(),key) != aliases_ptr->second.end()) {
      find_ptr = units_.find(aliases_ptr->first);
      return find_ptr != units_.end() ? find_ptr->second : "unspecified";
    }
  }
  return "unspecified";
}

bool DaidalusParameters::hasError() const {
  return error.hasError();
}

bool DaidalusParameters::hasMessage() const {
  return error.hasMessage();
}

std::string DaidalusParameters::getMessage() {
  return error.getMessage();
}

std::string DaidalusParameters::getMessageNoClear()  const {
  return error.getMessageNoClear();
}

}
