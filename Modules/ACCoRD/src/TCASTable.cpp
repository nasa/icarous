/*
 * Copyright (c) 2012-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "TCASTable.h"
#include "Units.h"
#include "format.h"
#include "ParameterData.h"
#include "string_util.h"
#include <cmath>
#include <sstream>

namespace larcfm {

/* Default levels in internal units */
const std::vector<double>& TCASTable::default_levels() {
  static std::vector<double> default_levels_;
  if (default_levels_.empty()) {
    default_levels_.resize(7); // use resize since we are then using subscript assignment
    default_levels_[0] = Units::from(Units::ft,0);     // Upper bound of SL 1
    default_levels_[1] = Units::from(Units::ft,1000);  // Upper bound of SL 2
    default_levels_[2] = Units::from(Units::ft,2350);  // Upper bound of SL 3
    default_levels_[3] = Units::from(Units::ft,5000);  // Upper bound of SL 4
    default_levels_[4] = Units::from(Units::ft,10000); // Upper bound of SL 5
    default_levels_[5] = Units::from(Units::ft,20000); // Upper bound of SL 6
    default_levels_[6] = Units::from(Units::ft,42000); // Upper bound of SL 7
  }
  return default_levels_;
} // Note that this array has only 7 entries.
//It is understood that there is one additional level with an infinite upper bound.

/* TA TAU Threshold in seconds */
const std::vector<double>& TCASTable::TA_TAU() {
  static std::vector<double> TA_TAU_;
  if (TA_TAU_.empty()) {
    TA_TAU_.resize(8); // use resize since we are then using subscript assignment
    TA_TAU_[0] = 0;   // SL 1
    TA_TAU_[1] = 20;  // SL 2
    TA_TAU_[2] = 25;  // SL 3
    TA_TAU_[3] = 30;  // SL 4
    TA_TAU_[4] = 40;  // SL 5
    TA_TAU_[5] = 45;  // SL 6
    TA_TAU_[6] = 48;  // SL 7
    TA_TAU_[7] = 48;  // SL 8
  }
  return TA_TAU_;
}

/* RA TAU Threshold in seconds */
const std::vector<double>& TCASTable::RA_TAU() {
  static std::vector<double> RA_TAU_;
  if (RA_TAU_.empty()) {
    RA_TAU_.resize(8); // use resize since we are then using subscript assignment
    RA_TAU_[0] = 0;  // SL 1
    RA_TAU_[1] = 0;  // SL 2
    RA_TAU_[2] = 15; // SL 3
    RA_TAU_[3] = 20; // SL 4
    RA_TAU_[4] = 25; // SL 5
    RA_TAU_[5] = 30; // SL 6
    RA_TAU_[6] = 35; // SL 7
    RA_TAU_[7] = 35; // SL 8
  }
  return RA_TAU_;
}

/* TA DMOD in internal units (0 if N/A) */
const std::vector<double>& TCASTable::TA_DMOD() {
  static std::vector<double> TA_DMOD_;
  if (TA_DMOD_.empty()) {
    TA_DMOD_.resize(8); // use resize since we are then using subscript assignment
    TA_DMOD_[0] = 0;                             // SL 1
    TA_DMOD_[1] = Units::from(Units::NM,0.30);   // SL 2
    TA_DMOD_[2] = Units::from(Units::NM,0.33);   // SL 3
    TA_DMOD_[3] = Units::from(Units::NM,0.48);   // SL 4
    TA_DMOD_[4] = Units::from(Units::NM,0.75);   // SL 5
    TA_DMOD_[5] = Units::from(Units::NM,1.0);    // SL 6
    TA_DMOD_[6] = Units::from(Units::NM,1.3);    // SL 7
    TA_DMOD_[7] = Units::from(Units::NM,1.3);    // SL 8
  }
  return TA_DMOD_;
}

/* RA DMOD in internal units (0 if N/A) */
const std::vector<double>& TCASTable::RA_DMOD() {
  static std::vector<double> RA_DMOD_;
  if (RA_DMOD_.empty()) {
    RA_DMOD_.resize(8); // use resize since we are then using subscript assignment
    RA_DMOD_[0] = 0;                               // SL 1
    RA_DMOD_[1] = 0;                               // SL 2
    RA_DMOD_[2] = Units::from(Units::NM,0.2);      // SL 3
    RA_DMOD_[3] = Units::from(Units::NM,0.35);     // SL 4
    RA_DMOD_[4] = Units::from(Units::NM,0.55);     // SL 5
    RA_DMOD_[5] = Units::from(Units::NM,0.8);      // SL 6
    RA_DMOD_[6] = Units::from(Units::NM,1.1);      // SL 7
    RA_DMOD_[7] = Units::from(Units::NM,1.1);      // SL 8
  }
  return RA_DMOD_;
}

/* TA ZTHR in internal units (0 if N/A) */
const std::vector<double>& TCASTable::TA_ZTHR() {
  static std::vector<double> TA_ZTHR_;
  if (TA_ZTHR_.empty()) {
    TA_ZTHR_.resize(8); // use resize since we are then using subscript assignment
    TA_ZTHR_[0] = 0;                               // SL 1                                          // 1
    TA_ZTHR_[1] = Units::from(Units::ft,850);      // SL 2
    TA_ZTHR_[2] = Units::from(Units::ft,850);      // SL 3
    TA_ZTHR_[3] = Units::from(Units::ft,850);      // SL 4
    TA_ZTHR_[4] = Units::from(Units::ft,850);      // SL 5
    TA_ZTHR_[5] = Units::from(Units::ft,850);      // SL 6
    TA_ZTHR_[6] = Units::from(Units::ft,850);      // SL 7
    TA_ZTHR_[7] = Units::from(Units::ft,1200);     // SL 8
  }
  return TA_ZTHR_;
}

/* RA ZTHR in internal units (0 if N/A) */
const std::vector<double>& TCASTable::RA_ZTHR() {
  static std::vector<double> RA_ZTHR_;
  if (RA_ZTHR_.empty()) {
    RA_ZTHR_.resize(8); // use resize since we are then using subscript assignment
    RA_ZTHR_[0] = 0;                             // SL1
    RA_ZTHR_[1] = 0;                             // SL 2
    RA_ZTHR_[2] = Units::from(Units::ft,600);    // SL 3
    RA_ZTHR_[3] = Units::from(Units::ft,600);    // SL 4
    RA_ZTHR_[4] = Units::from(Units::ft,600);    // SL 5
    RA_ZTHR_[5] = Units::from(Units::ft,600);    // SL 6
    RA_ZTHR_[6] = Units::from(Units::ft,700);    // SL 7
    RA_ZTHR_[7] = Units::from(Units::ft,800);    // SL 8
  }
  return RA_ZTHR_;
}

/* RA HMD in internal units (0 if N/A) */
const std::vector<double>& TCASTable::RA_HMD() {
  static std::vector<double> RA_HMD_;
  if (RA_HMD_.empty()) {
    RA_HMD_.resize(8); // use resize since we are then using subscript assignment
    RA_HMD_[0] = 0;                                // SL 1
    RA_HMD_[1] = 0;                                // SL 2
    RA_HMD_[2] = Units::from(Units::ft,1215);      // SL 3
    RA_HMD_[3] = Units::from(Units::ft,2126);      // SL 4
    RA_HMD_[4] = Units::from(Units::ft,3342);      // SL 5
    RA_HMD_[5] = Units::from(Units::ft,4861);      // SL 6
    RA_HMD_[6] = Units::from(Units::ft,6683);      // SL 7
    RA_HMD_[7] = Units::from(Units::ft,6683);      // SL 8
  }
  return RA_HMD_;
}

void TCASTable::add_zeros() {
  DMOD_.push_back(0.0);
  HMD_.push_back(0.0);
  ZTHR_.push_back(0.0);
  TAU_.push_back(0.0);
  TCOA_.push_back(0.0);
}

void TCASTable::default_units() {
  units_["TCAS_DMOD"] = "nmi";
  units_["TCAS_HMD"] = "ft";
  units_["TCAS_ZTHR"] = "ft";
  units_["TCAS_TAU"] = "s";
  units_["TCAS_TCOA"] = "s";
  units_["TCAS_level"] = "ft";
}

// Returns an empty TCASTable
TCASTable::TCASTable() {
  HMDFilter_ = false;
  add_zeros();
  default_units();
}

// TCASII RA Table
const TCASTable& TCASTable::TCASII_RA() {
  static TCASTable tab;
  if (tab.levels_.empty()) {
    tab.setDefaultTCASIIThresholds(true);
  }
  return tab;
}

// TCASII RA Table
const TCASTable& TCASTable::TCASII_TA() {
  static TCASTable tab;
  if (tab.levels_.empty()) {
    tab.setDefaultTCASIIThresholds(false);
  }
  return tab;
}

TCASTable TCASTable::make_TCASII_Table(bool ra) {
  TCASTable tcas_table;
  tcas_table.setDefaultTCASIIThresholds(ra);
  return tcas_table;
}

/** Make empty TCASTable
 *
 * @return  This returns a zeroed table with one unbounded level.
 * That level has value 0 for DMOD,HMD,ZTHR,TAUMOD,TCOA
 */
TCASTable TCASTable::make_Empty_TCASTable() {
  TCASTable tcas_table;
  return tcas_table;
}

// Clear all inputs in TCASTable
void TCASTable::clear() {
  levels_.clear();
  TAU_.clear();
  TCOA_.clear();
  DMOD_.clear();
  ZTHR_.clear();
  HMD_.clear();
  add_zeros();
  default_units();
}

/**
 * Return sensitivity level from alt, specified in internal units.
 * Sensitivity levels are indexed from 1.
 */
int TCASTable::getSensitivityLevel(double alt) const {
  int i;
  for (i = 0; i < (int) levels_.size(); i++) {
    if (alt <= levels_[i]) {
      return i+1;
    }
  }
  return i+1; // Returns last sensitivity level
}

/** Return sensitivity level from alt specified in u units */
int TCASTable::getSensitivityLevel(double alt, const std::string& u) const {
  return getSensitivityLevel(Units::from(u,alt));
}

/**
 * Return true if the sensitivity level is between 1 and levels.size().
 */
bool TCASTable::isValidSensitivityLevel(int sl) const {
  return 1 <= sl && sl <= (int) levels_.size()+1 &&
      (sl > (int) levels_.size() || (int) levels_.size() != 0.0);
}

/**
 * Returns the maximum defined sensitivity level (indexed from 1).
 */
int TCASTable::getMaxSensitivityLevel() const {
  return levels_.size()+1;
}

/**
 * Returns altitude lower bound for a given sensitivity level sl, in internal units.
 * Note this is an open bound (sl is valid for altitudes strictly greater than the return value)
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getLevelAltitudeLowerBound(int sl) const {
  if (isValidSensitivityLevel(sl)) {
    for (--sl; sl > 0 && levels_[sl-1] == 0.0; --sl);
    if (sl > 0) {
      return levels_[sl-1];
    } else {
      return 0;
    }
  }
  return -1;
}

/**
 * Returns altitude lower bound for a given sensitivity level sl, in given units.
 * Note this is an open bound (sl is valid for altitudes strictly greater than the return value)
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getLevelAltitudeLowerBound(int sl, std::string u) const {
  return Units::to(u,getLevelAltitudeLowerBound(sl));
}

/**
 * Returns altitude upper bound for a given sensitivity level sl, in internal units.
 * Note this is a closed bound (sl is valid for altitudes less than or equal to the return value)
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getLevelAltitudeUpperBound(int sl) const {
  if (isValidSensitivityLevel(sl)) {
    if (sl == getMaxSensitivityLevel()) {
      return INFINITY;
    } else {
      return levels_[sl-1];
    }
  }
  return -1;
}

/**
 * Returns altitude upper bound for a given sensitivity level sl, in given units.
 * Note this is a closed bound (sl is valid for altitudes less than or equal to the return value)
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getLevelAltitudeUpperBound(int sl, std::string u) const {
  return Units::to(u,getLevelAltitudeUpperBound(sl));
}


void TCASTable::setTCASIILevels() {
  clear();
  for (int i=0; i < 7; ++i) {
    addSensitivityLevel(default_levels()[i]);
  }
}


/**
 * Set table to TCASII Thresholds (RA Table when ra is true, TA Table when ra is false)
 */
void TCASTable::setDefaultTCASIIThresholds(bool ra) {
  HMDFilter_ = ra;
  setTCASIILevels();
  for (int i=0; i < 8; ++i) {
    if (ra) {
      TAU_[i] = RA_TAU()[i];
      TCOA_[i] = RA_TAU()[i];
      DMOD_[i] = RA_DMOD()[i];
      ZTHR_[i] = RA_ZTHR()[i];
      HMD_[i] = RA_HMD()[i];
    } else {
      TAU_[i] = TA_TAU()[i];
      TCOA_[i] = TA_TAU()[i];
      DMOD_[i] = TA_DMOD()[i];
      ZTHR_[i] = TA_ZTHR()[i];
      HMD_[i] = TA_DMOD()[i];
    }
  }
  default_units();
}

/**
 * Returns TAU threshold for sensitivity level sl in seconds.
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getTAU(int sl) const  {
  if (isValidSensitivityLevel(sl)) {
    return TAU_[sl-1];
  }
  return -1;
}

/**
 * Returns TCOA threshold for sensitivity level sl in seconds
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getTCOA(int sl) const  {
  if (isValidSensitivityLevel(sl)) {
    return TCOA_[sl-1];
  }
  return -1;
}

/**
 * Returns DMOD for sensitivity level sl in internal units.
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getDMOD(int sl) const  {
  if (isValidSensitivityLevel(sl)) {
    return DMOD_[sl-1];
  }
  return -1;
}

/**
 * Returns DMOD for sensitivity level sl in u units.
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getDMOD(int sl, std::string u) const  {
  return Units::to(u,getDMOD(sl));
}

/**
 * Returns Z threshold for sensitivity level sl in internal units.
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getZTHR(int sl) const  {
  if (isValidSensitivityLevel(sl)) {
    return ZTHR_[sl-1];
  }
  return -1;
}

/**
 * Returns Z threshold for sensitivity level sl in u units.
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getZTHR(int sl,std::string u) const  {
  return Units::to(u,getZTHR(sl));
}

/**
 * Returns HMD for sensitivity level sl in internal units.
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getHMD(int sl) const  {
  if (isValidSensitivityLevel(sl)) {
    return HMD_[sl-1];
  }
  return -1;
}

/**
 * Returns HMD for sensitivity level sl in u units.
 * This returns a negative value if an invalid level is input.
 */
double TCASTable::getHMD(int sl, std::string u) const  {
  return Units::to(u,getHMD(sl));
}

/** Modify the value of Tau Threshold for a given sensitivity level
 * Parameter val is given in seconds. Val is >= 0. Return true is value was set.
 */
bool TCASTable::setTAU(int sl, double val) {
  if (isValidSensitivityLevel(sl)) {
    TAU_[sl-1] = Util::max(0.0,val);
    return true;
  }
  return false;
}

/** Modify the value of Tau Threshold for a given sensitivity level
 * Parameter val is given in given units. Val is >= 0
 */
void TCASTable::setTAU(int sl, double val, std::string u) {
  if (setTAU(sl,Units::from(u,val))) {
    units_["TCAS_TAU"]=u;
  }
}

/** Modify the value of TCOA Threshold for a given sensitivity level
 * Parameter val is given in seconds. Val is >= 0. Return true is value was set.
 */
bool TCASTable::setTCOA(int sl, double val) {
  if (isValidSensitivityLevel(sl)) {
    TCOA_[sl-1] = Util::max(0.0,val);
    return true;
  }
  return false;
}

/** Modify the value of TCOA Threshold for a given sensitivity level
 * Parameter val is given in given units. Val is >= 0
 */
void TCASTable::setTCOA(int sl, double val, std::string u) {
  if (setTCOA(sl,Units::from(u,val))) {
    units_["TCAS_TCOA"]=u;
  }
}

/** Modify the value of DMOD for a given sensitivity level
 * Parameter val is given in internal units. Val is >= 0. Return true is value was set.
 */
bool TCASTable::setDMOD(int sl, double val) {
  if (isValidSensitivityLevel(sl)) {
    DMOD_[sl-1] = Util::max(0.0,val);
    return true;
  }
  return false;
}

/** Modify the value of DMOD for a given sensitivity level
 * Parameter val is given in u units. Val is >= 0.
 */
void TCASTable::setDMOD(int sl, double val, std::string u) {
  if (setDMOD(sl,Units::from(u,val))) {
    units_["TCAS_DMOD"]=u;
  }
}

/** Modify the value of ZTHR for a given sensitivity level
 * Parameter val is given in internal units. Val is >= 0. Return true is value was set.
 */
bool TCASTable::setZTHR(int sl, double val) {
  if (isValidSensitivityLevel(sl)) {
    ZTHR_[sl-1] = Util::max(0.0,val);
    return true;
  }
  return false;
}

/** Modify the value of ZTHR for a given sensitivity level
 * Parameter val is given in u units. Val is >= 0.
 */
void TCASTable::setZTHR(int sl, double val, std::string u) {
  if (setZTHR(sl,Units::from(u,val))) {
    units_["TCAS_ZTHR"]=u;
  }
}

/**
 * Modify the value of HMD for a given sensitivity level
 * Parameter val is given in internal units. Val is >= 0. Return true is value was set.
 */
bool TCASTable::setHMD(int sl, double val) {
  if (isValidSensitivityLevel(sl)) {
    HMD_[sl-1] = Util::max(0.0,val);
    return true;
  }
  return false;
}

/**
 * Modify the value of HMD for a given sensitivity level
 * Parameter val is given in u units. Val is >= 0.
 */
void TCASTable::setHMD(int sl, double val, std::string u) {
  if (setHMD(sl,Units::from(u,val))) {
    units_["TCAS_HMD"]=u;
  }
}

/**
 * Add sensitivity level with upper bound altitude alt (in internal units).
 * Requires: alt > levels_.get(size(levels_)-1) or an empty table
 * Add value 0 to DMOD,HMD,ZTHR,TAUMOD,TCOA
 * Either returns index of new maximum sensitivity level or 0 (if requires is false)
 */
int TCASTable::addSensitivityLevel(double alt) {
  if (levels_.empty() || alt > levels_[levels_.size()-1]) {
    levels_.push_back(alt);
    add_zeros();
    return levels_.size()+1;
  }
  return 0;
}

/**
 * Add empty sensitivity level
 * Add value 0 to DMOD,HMD,ZTHR,TAUMOD,TCOA
 * Either returns index of new maximum sensitivity level
 */
int TCASTable::addSensitivityLevel() {
  levels_.push_back(0.0);
  add_zeros();
  return levels_.size()+1;
}

/**
 * Add sensitivity level with upper bound altitude alt (in given units).
 * Requires: alt > levels_.get(size(levels_)-1) or an empty table
 * Add value 0 to DMOD,HMD,ZTHR,TAUMOD,TCOA
 * Either returns new sensitivity level or 0 (if requires is false)
 */
int TCASTable::addSensitivityLevel(double alt,std::string u) {
  int sl=addSensitivityLevel(Units::from(u,alt));
  if (sl > 0) {
    units_["TCAS_level"]=u;
  }
  return sl;
}

void TCASTable::setHMDFilter(bool flag) {
  HMDFilter_ = flag;
}

bool TCASTable::getHMDFilter() const {
  return HMDFilter_;
}

/** Return true if the values in the table correspond to the standard RA values */
bool TCASTable::isRAStandard() const {
  if (levels_.size() != 7) return false;
  bool ra = HMDFilter_;
  for (int i=0; ra && i < 8; ++i) {
    ra &= (i == 7 || levels_[i] == default_levels()[i]) && TAU_[i]==RA_TAU()[i] && TCOA_[i]==RA_TAU()[i] &&
        DMOD_[i]==RA_DMOD()[i] && ZTHR_[i]==RA_ZTHR()[i] && HMD_[i]==RA_HMD()[i];
  }
  return ra;
}

/** Return true if the values in the table correspond to the standard TA values */
bool TCASTable::isTAStandard() const {
  bool ta = !HMDFilter_;
  if (levels_.size() != 7) return false;
  for (int i=0; ta && i < 8; ++i) {
    ta &= (i == 7 || levels_[i] == default_levels()[i]) && TAU_[i]==TA_TAU()[i] && TCOA_[i]==TA_TAU()[i] &&
        DMOD_[i]==TA_DMOD()[i] && ZTHR_[i]==TA_ZTHR()[i] && HMD_[i]==TA_DMOD()[i];
  }
  return ta;
}

std::string TCASTable::list_units(const std::string& units, const std::vector<double>& v) {
  std::string s="";
  bool comma = false;
  for (int i=0; i<(int)v.size(); ++i) {
    if (comma) {
      s+=", ";
    } else {
      comma = true;
    }
    s+=Units::str(units,v[i]);
  }
  return s;
}

std::string TCASTable::toString() const {
  std::string s = "HMDFilter: "+Fmb(HMDFilter_);
  if (isRAStandard()) s = s+"; (RA vals) ";
  else if (isTAStandard()) s= s+"; (TA vals) ";
  std::string unit1 =
      s= s+"; levels: "+list_units(units_.at("TCAS_level"),levels_)+
      "; TAU: "+list_units(units_.at("TCAS_TAU"),TAU_)+"; TCOA: "+
      list_units(units_.at("TCAS_TCOA"),TCOA_)+
      "; DMOD: "+list_units(units_.at("TCAS_DMOD"),DMOD_)+"; ZTHR: "+
      list_units(units_.at("TCAS_ZTHR"),ZTHR_)+
      "; HMD: "+list_units(units_.at("TCAS_HMD"),HMD_);
  return s;
}

std::string TCASTable::pvs_list(const std::vector<double>& v) const {
  std::string s="(: ";
  bool comma = false;
  for (int i=0; i<(int)v.size(); ++i) {
    if (comma) {
      s+=", ";
    } else {
      comma = true;
    }
    s+=FmPrecision(v[i]);
  }
  s += " :)";
  return s;
}

std::string TCASTable::toPVS() const {
  std::string s = "(# ";
  s += "levels := "+pvs_list(levels_);
  s += ", TAU := "+pvs_list(TAU_);
  s += ", TCOA := "+pvs_list(TCOA_);
  s += ", DMOD := "+pvs_list(DMOD_);
  s += ", HMD := "+pvs_list(HMD_);
  s += ", ZTHR := "+pvs_list(ZTHR_);
  std::string ss = (HMDFilter_ ? "TRUE" : "FALSE");
  s += ", HMDFilter := " + ss;
  return s + " #)";
}

ParameterData TCASTable::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

std::string TCASTable::getUnits(const std::string& key) const {
  std::string u = units_.at(key);
  if (larcfm::equals(u, "")) {
    return "unspecified";
  }
  return u;
}

void TCASTable::updateParameterData(ParameterData& p) const {
  p.setBool("TCAS_HMDFilter",HMDFilter_);
  std::string u = getUnits("TCAS_level");
  for (int i = 0; i < (int) levels_.size(); ++i) {
    p.setInternal("TCAS_level_"+Fmi(i+1),levels_[i],u);
  }
  u = getUnits("TCAS_TAU");
  for (int i = 0; i < (int) TAU_.size(); ++i) {
    p.setInternal("TCAS_TAU_"+Fmi(i+1),TAU_[i],u);
  }
  u = getUnits("TCAS_TCOA");
  for (int i = 0; i < (int) TCOA_.size(); ++i) {
    p.setInternal("TCAS_TCOA_"+Fmi(i+1),TCOA_[i],u);
  }
  u = getUnits("TCAS_DMOD");
  for (int i = 0; i < (int) DMOD_.size(); ++i) {
    p.setInternal("TCAS_DMOD_"+Fmi(i+1),DMOD_[i],u);
  }
  u = getUnits("TCAS_ZTHR");
  for (int i = 0; i < (int) ZTHR_.size(); ++i) {
    p.setInternal("TCAS_ZTHR_"+Fmi(i+1),ZTHR_[i],u);
  }
  u = getUnits("TCAS_HMD");
  for (int i = 0; i < (int) HMD_.size(); ++i) {
    p.setInternal("TCAS_HMD_"+Fmi(i+1),HMD_[i],u);
  }
}

void TCASTable::setParameters(const ParameterData& p) {
  if (p.contains("TCAS_HMDFilter")) {
    HMDFilter_ = p.getBool("TCAS_HMDFilter");
  }
  ParameterData subp = p.extractPrefix("TCAS_level_");
  // determine maximum level
  int max_level = 0;
  std::vector<std::string> keys = subp.getKeyList();
  for (int i = 0; i < (int) keys.size(); i++) {
    int j;
    std::istringstream(keys[i]) >> j; // C++11 uses std::stoi()
    max_level = std::max(j,max_level);
  }
  clear();
  for (int sl = 1; sl <= max_level; ++sl) {
    if (p.contains("TCAS_level_"+Fmi(sl))) {
      addSensitivityLevel(p.getValue("TCAS_level_"+Fmi(sl)));
      units_["TCAS_level"]=p.getUnit("TCAS_level_"+Fmi(sl));
    } else {
      addSensitivityLevel();
    }
  }
  for (int sl = 1; sl <= getMaxSensitivityLevel(); ++sl) {
    if (p.contains("TCAS_TAU_"+Fmi(sl))) {
      setTAU(sl,p.getValue("TCAS_TAU_"+Fmi(sl)));
      units_["TCAS_TAU"]=p.getUnit("TCAS_TAU_"+Fmi(sl));
    }
    if (p.contains("TCAS_TCOA_"+Fmi(sl))) {
      setTCOA(sl,p.getValue("TCAS_TCOA_"+Fmi(sl)));
      units_["TCAS_TCOA"]=p.getUnit("TCAS_TCOA_"+Fmi(sl));
    }
    if (p.contains("TCAS_DMOD_"+Fmi(sl))) {
      setDMOD(sl,p.getValue("TCAS_DMOD_"+Fmi(sl)));
      units_["TCAS_DMOD"]=p.getUnit("TCAS_DMOD_"+Fmi(sl));
    }
    if (p.contains("TCAS_HMD_"+Fmi(sl))) {
      setHMD(sl,p.getValue("TCAS_HMD_"+Fmi(sl)));
      units_["TCAS_HMD"]=p.getUnit("TCAS_HMD_"+Fmi(sl));
    }
    if (p.contains("TCAS_ZTHR_"+Fmi(sl))) {
      setZTHR(sl,p.getValue("TCAS_ZTHR_"+Fmi(sl)));
      units_["TCAS_ZTHR"]=p.getUnit("TCAS_ZTHR_"+Fmi(sl));
    }
  }
}

bool TCASTable::contains(const TCASTable& tab) const {
  if (levels_.size() != tab.levels_.size()) return false;
  if (HMDFilter_ != tab.HMDFilter_) return false;
  for (int i=0; i <= (int) levels_.size(); i++) {
    if (i < (int) levels_.size() && !Util::almost_equals(levels_[i],tab.levels_[i])) return false;
    if (!(TAU_[i] >= tab.TAU_[i] && TCOA_[i] >= tab.TCOA_[i] &&
        DMOD_[i] >= tab.DMOD_[i] && ZTHR_[i] >= tab.ZTHR_[i] && HMD_[i] >= tab.HMD_[i]))
      return false;
  }
  return true;
}

bool TCASTable::equals(const TCASTable& t) const {
  if (levels_.size() != t.levels_.size()) return false;
  for (int i=0; i <= (int) levels_.size(); i++) {
    if (i < (int) levels_.size() && levels_[i] != t.levels_[i]) return false;
    if (TAU_[i] != t.TAU_[i]) return false;
    if (TCOA_[i] != t.TCOA_[i]) return false;
    if (DMOD_[i] != t.DMOD_[i]) return false;
    if (ZTHR_[i] != t.ZTHR_[i]) return false;
    if (HMD_[i] != t.HMD_[i]) return false;
  }
  return HMDFilter_ == t.HMDFilter_;
}

}
