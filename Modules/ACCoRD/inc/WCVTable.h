/*
 * Copyright (c) 2012-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef WCVTABLE_H_
#define WCVTABLE_H_


#include "Units.h"
#include "ParameterTable.h"
#include "ParameterData.h"
#include <string>
#include <map>

namespace larcfm {

class WCVTable : public ParameterTable {

private:
  std::map<std::string,std::string> units_;

public:
  double DTHR; // Distance threshold
  double ZTHR; // Vertical threshold
  double TTHR; // Time threshold
  double TCOA; // Time to co-altitude threshold

  /**
   * Table containing specified values (internal units)
   */
  WCVTable(double dthr, double zthr, double tthr, double tcoa);

  /**
   * Table containing specified values (specified units)
   */
  WCVTable(double dthr, const std::string& udthr, double zthr, const std::string& uzthr,
      double tthr, const std::string& utthr, double tcoa, const std::string& utcoa);

  /**
   * WCV table SARP concept
   */
  WCVTable();

  /** Copy constructor */
  WCVTable(const WCVTable& tab);

  WCVTable copy() const;

  /** copy values from t into this object */
  void copyValues(const WCVTable& tab);

  /**
   * Return horizontal distance threshold DTHR in internal units
   */
  double getDTHR() const;
  /**
   * Return horizontal distance threshold DTHR in u units
   */
  double getDTHR(const std::string& u) const;

  /**
   * Return vertical distance threshold ZTHR in internal units
   */
  double getZTHR() const;
  /**
   * Return vertical distance threshold ZTHR in u units
   */
  double getZTHR(const std::string& u) const;

  /**
   * Return horizontal time threshold TTHR in seconds
   */
  double getTTHR() const;
  double getTTHR(const std::string& u) const;

  /**
   * Return vertical time threshold TCOA in seconds
   */
  double getTCOA() const;
  double getTCOA(const std::string& u) const;

  void setDTHR(double val);
  void setDTHR(double val, const std::string& u);

  void setZTHR(double val);
  void setZTHR(double val, const std::string& u);

  void setTTHR(double val);
  void setTTHR(double val, const std::string& u);

  void setTCOA(double val);
  void setTCOA(double val, const std::string& u);

  ParameterData getParameters() const;
  void updateParameterData(ParameterData& p) const;
  void setParameters(const ParameterData& p);

  std::string getUnits(const std::string& key) const;

  bool equals(const WCVTable& t2) const;

  std::string toString() const;

  std::string toPVS(int prec) const;

  bool contains(const WCVTable& tab) const;

};

}

#endif
