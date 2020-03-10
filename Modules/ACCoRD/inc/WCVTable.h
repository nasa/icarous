/*
 * Copyright (c) 2012-2019 United States Government as represented by
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

  ~WCVTable();

  /**
   * @return DO-365 preventive thresholds, i.e., DTHR=0.66nmi, ZTHR=700ft,
   * TTHR=35s, TCOA=0.
   */
  static const WCVTable& DO_365_Phase_I_preventive();

  /**
   * @return DO-365 Well-Clear thresholds, i.e., DTHR=0.66nmi, ZTHR=450ft,
   * TTHR=35s, TCOA=0.
   */
  static const WCVTable& DO_365_DWC_Phase_I();

  /**
   * @return buffered preventive thresholds, i.e., DTHR=1nmi, ZTHR=750ft,
   * TTHR=35s, TCOA=20.
   */
  static const WCVTable& Buffered_Phase_I_preventive();

  /**
   * @return buffered Well-Clear thresholds, i.e., DTHR=1.0nmi, ZTHR=450ft,
   * TTHR=35s, TCOA=20.
   */
  static const WCVTable& Buffered_DWC_Phase_I();

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

  std::string toPVS() const;

  bool contains(const WCVTable& tab) const;

  std::string toPVS_() const;

};

}

#endif
