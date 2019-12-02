/*
 * Copyright (c) 2012-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef TCASTABLE_H_
#define TCASTABLE_H_


#include "Units.h"
#include "Util.h"
#include "ParameterTable.h"
#include "ParameterData.h"
#include "format.h"
#include <string>
#include <vector>
#include <map>
//#include <numeric_limits>

namespace larcfm {

class TCASTable : public ParameterTable {

private:
  /* Default levels in internal units */
  static const std::vector<double>& default_levels(); // default 7
  //  = {
  //      Units::from(Units::ft,0),     // Upper bound of TCAS Level 1
  //      Units::from(Units::ft,1000),  // Upper bound of TCAS Level 2
  //      Units::from(Units::ft,2350),  // Upper bound of TCAS Level 3
  //      Units::from(Units::ft,5000),  // Upper bound of TCAS Level 4
  //      Units::from(Units::ft,10000), // Upper bound of TCAS Level 5
  //      Units::from(Units::ft,20000), // Upper bound of TCAS Level 6
  //      Units::from(Units::ft,42000)  // Upper bound of TCAS Level 7
  //  }; // Note that this array has only 7 entries.
  //It is understood that there is one additional level with an infinite upper bound.

  /* TA TAU Threshold in seconds */
  static const std::vector<double>& TA_TAU(); // default 8
  // 1  2  3  4  5  6  7  8 (0 i N/A)
  // = {0.0,20.0,25.0,30.0,40.0,45.0,48.0,48.0};

  /* RA TAU Threshold in seconds */
  static const std::vector<double>& RA_TAU(); // default 8
  // 1  2  3  4  5  6  7  8 (0 i N/A)
  // = {0.0,0.0,15.0,20.0,25.0,30.0,35.0,35.0};

  /* TA DMOD in internal units (0 if N/A) */
  static const std::vector<double>& TA_DMOD(); //default 8
  //  = {
  //      0,                                                       // 1
  //      Units::from(Units::NM,0.30),                       // 2
  //      Units::from(Units::NM,0.33),                       // 3
  //      Units::from(Units::NM,0.48),                       // 4
  //      Units::from(Units::NM,0.75),                       // 5
  //      Units::from(Units::NM,1.0),                        // 6
  //      Units::from(Units::NM,1.3),                        // 7
  //      Units::from(Units::NM,1.3)};                       // 8

  /* RA DMOD in internal units (0 if N/A) */
  static const std::vector<double>& RA_DMOD(); // default 8
  //  = {
  //      0,                                                    // 1
  //      0,                                                  // 2
  //      Units::from(Units::NM,0.2),               // 3
  //      Units::from(Units::NM,0.35),              // 4
  //      Units::from(Units::NM,0.55),              // 5
  //      Units::from(Units::NM,0.8),               // 6
  //      Units::from(Units::NM,1.1),               // 7
  //      Units::from(Units::NM,1.1)};              // 8

  /* TA ZTHR in internal units (0 if N/A) */
  static const std::vector<double>& TA_ZTHR(); // default 8
  //  = {
  //      0,                                                  // 1
  //      Units::from(Units::ft,850),               // 2
  //      Units::from(Units::ft,850),               // 3
  //      Units::from(Units::ft,850),               // 4
  //      Units::from(Units::ft,850),               // 5
  //      Units::from(Units::ft,850),               // 6
  //      Units::from(Units::ft,850),               // 7
  //      Units::from(Units::ft,1200)};             // 8

  /* RA ZTHR in internal units (0 if N/A) */
  static const std::vector<double>& RA_ZTHR(); // default 8
  //  = {
  //      0,                                                      // 1
  //      0,                                                      // 2
  //      Units::from(Units::ft,600),               // 3
  //      Units::from(Units::ft,600),               // 4
  //      Units::from(Units::ft,600),               // 5
  //      Units::from(Units::ft,600),               // 6
  //      Units::from(Units::ft,700),               // 7
  //      Units::from(Units::ft,800)};              // 8

  /* RA HMD in internal units (0 if N/A) */
  static const std::vector<double>& RA_HMD(); // default 8
  //  = {
  //      0,                                                      // 1
  //      0,                                                      // 2
  //      Units::from(Units::ft,1215),              // 3
  //      Units::from(Units::ft,2126),              // 4
  //      Units::from(Units::ft,3342),              // 5
  //      Units::from(Units::ft,4861),              // 6
  //      Units::from(Units::ft,6683),              // 7
  //      Units::from(Units::ft,6683)};             // 8

  bool HMDFilter_;
  std::map<std::string,std::string> units_;
  std::vector<double> TAU_;
  std::vector<double> TCOA_;
  std::vector<double> DMOD_;
  std::vector<double> ZTHR_;
  std::vector<double> HMD_;
  std::vector<double> levels_; // this is the upper end for each level, indexed from _1_.
  // This list has one less element than the other lists.

  void add_zeros();

  void default_units();

public:
  // Returns an empty TCASTable
  TCASTable(); //TODO make this private

  // TCASII RA Table
  static const TCASTable& TCASII_RA();

  // TCASII RA Table
  static const TCASTable& TCASII_TA();

  static TCASTable make_TCASII_Table(bool ra);

  /** Make empty TCASTable
   *
   * @return  This returns a zeroed table with one unbounded level.
   * That level has value 0 for DMOD,HMD,ZTHR,TAUMOD,TCOA
   */
  static TCASTable make_Empty_TCASTable();

  // Clear all inputs in TCASTable
  void clear();

  /**
   * Return sensitivity level from alt, specified in internal units.
   * Sensitivity levels are indexed from 1.
   */
  int getSensitivityLevel(double alt) const;

  /** Return sensitivity level from alt specified in u units */
  int getSensitivityLevel(double alt, const std::string& u) const;

  /**
   * Return true if the sensitivity level is between 1 and levels.size().
   */
  bool isValidSensitivityLevel(int sl) const;

  /**
   * Returns the maximum defined sensitivity level (indexed from 1).
   */
  int getMaxSensitivityLevel() const;

  /**
   * Returns altitude lower bound for a given sensitivity level sl, in internal units.
   * Note this is an open bound (sl is valid for altitudes strictly greater than the return value)
   * This returns a negative value if an invalid level is input.
   */
  double getLevelAltitudeLowerBound(int sl) const;

  /**
   * Returns altitude lower bound for a given sensitivity level sl, in given units.
   * Note this is an open bound (sl is valid for altitudes strictly greater than the return value)
   * This returns a negative value if an invalid level is input.
   */
  double getLevelAltitudeLowerBound(int sl, std::string u) const;

  /**
   * Returns altitude upper bound for a given sensitivity level sl, in internal units.
   * Note this is a closed bound (sl is valid for altitudes less than or equal to the return value)
   * This returns a negative value if an invalid level is input.
   */
  double getLevelAltitudeUpperBound(int sl) const;

  /**
   * Returns altitude upper bound for a given sensitivity level sl, in given units.
   * Note this is a closed bound (sl is valid for altitudes less than or equal to the return value)
   * This returns a negative value if an invalid level is input.
   */
  double getLevelAltitudeUpperBound(int sl, std::string u) const;

private:
  void setTCASIILevels();

public:
  /**
   * Set table to TCASII Thresholds (RA Table when ra is true, TA Table when ra is false)
   */
  void setDefaultTCASIIThresholds(bool ra);

  /**
   * Returns TAU threshold for sensitivity level sl in seconds.
   * This returns a negative value if an invalid level is input.
   */
  double getTAU(int sl) const;

  /**
   * Returns TCOA threshold for sensitivity level sl in seconds
   * This returns a negative value if an invalid level is input.
   */
  double getTCOA(int sl) const;

  /**
   * Returns DMOD for sensitivity level sl in internal units.
   * This returns a negative value if an invalid level is input.
   */
  double getDMOD(int sl) const;

  /**
   * Returns DMOD for sensitivity level sl in u units.
   * This returns a negative value if an invalid level is input.
   */
  double getDMOD(int sl, std::string u) const;

  /**
   * Returns Z threshold for sensitivity level sl in internal units.
   * This returns a negative value if an invalid level is input.
   */
  double getZTHR(int sl) const;

  /**
   * Returns Z threshold for sensitivity level sl in u units.
   * This returns a negative value if an invalid level is input.
   */
  double getZTHR(int sl,std::string u) const;

  /**
   * Returns HMD for sensitivity level sl in internal units.
   * This returns a negative value if an invalid level is input.
   */
  double getHMD(int sl) const;

  /**
   * Returns HMD for sensitivity level sl in u units.
   * This returns a negative value if an invalid level is input.
   */
  double getHMD(int sl, std::string u) const;

  /** Modify the value of Tau Threshold for a given sensitivity level
   * Parameter val is given in seconds. Val is >= 0. Return true is value was set.
   */
  bool setTAU(int sl, double val);

  /** Modify the value of Tau Threshold for a given sensitivity level
   * Parameter val is given in given units. Val is >= 0
   */
  void setTAU(int sl, double val, std::string u);

  /** Modify the value of TCOA Threshold for a given sensitivity level
   * Parameter val is given in seconds. Val is >= 0. Return true is value was set.
   */
  bool setTCOA(int sl, double val);

  /** Modify the value of TCOA Threshold for a given sensitivity level
   * Parameter val is given in given units. Val is >= 0
   */
  void setTCOA(int sl, double val, std::string u);

  /** Modify the value of DMOD for a given sensitivity level
   * Parameter val is given in internal units. Val is >= 0. Return true is value was set.
   */
  bool setDMOD(int sl, double val);

  /** Modify the value of DMOD for a given sensitivity level
   * Parameter val is given in u units. Val is >= 0.
   */
  void setDMOD(int sl, double val, std::string u);

  /** Modify the value of ZTHR for a given sensitivity level
   * Parameter val is given in internal units. Val is >= 0. Return true is value was set.
   */
  bool setZTHR(int sl, double val);

  /** Modify the value of ZTHR for a given sensitivity level
   * Parameter val is given in u units. Val is >= 0.
   */
  void setZTHR(int sl, double val, std::string u);

  /**
   * Modify the value of HMD for a given sensitivity level
   * Parameter val is given in internal units. Val is >= 0. Return true is value was set.
   */
  bool setHMD(int sl, double val);


  /**
   * Modify the value of HMD for a given sensitivity level
   * Parameter val is given in u units. Val is >= 0.
   */
  void setHMD(int sl, double val, std::string u);

  /**
   * Add sensitivity level with upper bound altitude alt (in internal units).
   * Requires: alt > levels_.get(size(levels_)-1) or an empty table
   * Add value 0 to DMOD,HMD,ZTHR,TAUMOD,TCOA
   * Either returns index of new maximum sensitivity level or 0 (if requires is false)
   */
  int addSensitivityLevel(double alt);

  /**
   * Add empty sensitivity level
   * Add value 0 to DMOD,HMD,ZTHR,TAUMOD,TCOA
   * Either returns index of new maximum sensitivity level
   */
  int addSensitivityLevel();

  /**
   * Add sensitivity level with upper bound altitude alt (in given units).
   * Requires: alt > levels_.get(size(levels_)-1) or an empty table
   * Add value 0 to DMOD,HMD,ZTHR,TAUMOD,TCOA
   * Either returns new sensitivity level or 0 (if requires is false)
   */
  int addSensitivityLevel(double alt,std::string u);

  void setHMDFilter(bool flag);

  bool getHMDFilter() const;

  /** Return true if the values in the table correspond to the standard RA values */
  bool isRAStandard() const;

  /** Return true if the values in the table correspond to the standard TA values */
  bool isTAStandard() const;

private:
  static std::string list_units(const std::string& units, const std::vector<double>& v);

public:
  std::string toString() const;

private:
  std::string pvs_list(const std::vector<double>& v) const;

public:
  std::string toPVS() const;

  std::string getUnits(const std::string& key) const;

  bool equals(const TCASTable& t) const;

  bool contains(const TCASTable& tab) const;


  ParameterData getParameters() const;
  void updateParameterData(ParameterData& p) const;
  void setParameters(const ParameterData& p);

};

}

#endif
