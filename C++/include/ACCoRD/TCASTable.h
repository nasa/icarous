/*
 * Copyright (c) 2012-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef TCASTABLE_H_
#define TCASTABLE_H_


#include "Units.h"
#include "ParameterTable.h"
#include "ParameterData.h"
#include <string>

namespace larcfm {

class TCASTable : public ParameterTable {
private:
  /* RA Tau in seconds (-1 if N/A) */
  static double RA_TAU[7];
  static double TA_TAU[7];
  double TAU[7];
  double TCOA[7];

  /* RA DMOD in internal units (-1 if N/A) */
  static double RA_DMOD[7];
  static double TA_DMOD[7];
  double DMOD[7];

  /* RA ZTHR in internal units (-1 if N/A) */
  static double RA_ZTHR[7];
  static double TA_ZTHR[7];
  double ZTHR[7];

  /* RA HMD in internal units (-1 if N/A) */
  static double RA_HMD[7];
  double HMD[7];

  bool HMDFilter;

public:
  TCASTable();

  TCASTable(bool ra);

  /** Copy constructor */
  TCASTable(const TCASTable& t);

  /* Return SL-2 from alt, which is provided in internal units */
  /** Return sensitivity level from alt, specified in internal units */
  static int getSensitivityLevel(double alt);

  /**
   * Copy values from t to this table.
   */
  void copyValues(const TCASTable& t);

  void setDefaultRAThresholds(bool ra);
  /**
   * Returns TAU threshold for sensitivity level sl in seconds
   */
  double getTAU(int sl) const;
  double getTAU(int sl, const std::string& u) const;
  /**
   * Returns TCOA threshold for sensitivity level sl in seconds
   */
  double getTCOA(int sl) const;
  double getTCOA(int sl, const std::string& u) const;
  /**
   * Returns DMOD for sensitivity level sl in internal units.
   */
  double getDMOD(int sl) const;
  /**
   * Returns DMOD for sensitivity level sl in u units.
   */
  double getDMOD(int sl, const std::string& u) const;
  /**
   * Returns Z threshold for sensitivity level sl in internal units.
   */
  double getZTHR(int sl) const;
  /**
   * Returns Z threshold for sensitivity level sl in u units.
   */
  double getZTHR(int sl, const std::string& u) const;
  /**
   * Returns HMD for sensitivity level sl in internal units.
   */
  double getHMD(int sl) const;
  /**
   * Returns HMD for sensitivity level sl in u units.
   */
  double getHMD(int sl, const std::string& u) const;
  /** Modify the value of Tau Threshold for a given sensitivity level (2-8)
   * Parameter val is given in seconds 
   */
  void setTAU(int sl, double val);
  void setTAU(int sl, double val, const std::string& u);
  void setTAU(double val, const std::string& u);

  /** Modify the value of TCOA Threshold for a given sensitivity level (2-8)
   * Parameter val is given in seconds 
   */
  void setTCOA(int sl, double val);
  void setTCOA(int sl, double val, const std::string& u);
  void setTCOA(double val, const std::string& u);
  /** Modify the value of DMOD for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  void setDMOD(int sl, double val);
  /** Modify the value of DMOD for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  void setDMOD(int sl, double val, const std::string& u);
  /** 
   * Modify the value of DMOD for all sensitivity levels 
   */
  void setDMOD(double val, const std::string& u);
  /** Modify the value of ZTHR for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  void setZTHR(int sl, double val);
  /** Modify the value of ZTHR for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  void setZTHR(int sl, double val, const std::string& u);
  /** 
   * Modify the value of ZTHR for all sensitivity levels 
   */
  void setZTHR(double val, const std::string& u);
  /** 
   * Modify the value of HMD for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  void setHMD(int sl, double val);
  /** 
   * Modify the value of HMD for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  void setHMD(int sl, double val, const std::string& u);
  /** 
   * Modify the value of HMD for all sensitivity levels 
   */
  void setHMD(double val, const std::string& u);
  void setHMDFilter(bool flag);
  bool getHMDFilter() const;
  /** Return true if the values in the table correspond to the standard RA values */
  bool isRAStandard() const;
  /** Return true if the values in the table correspond to the standard TA values */
  bool isTAStandard() const;

  ParameterData getParameters() const;
  void updateParameterData(ParameterData& p) const;
  void setParameters(const ParameterData& p);

  bool equals(const TCASTable& t2) const;

  std::string toString() const;

  std::string toPVS(int prec) const;

  bool contains(const TCASTable& tab) const;

};

}

#endif
