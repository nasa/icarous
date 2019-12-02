/*
 * Copyright (c) 2012-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CD3DTABLE_H_
#define CD3DTABLE_H_


#include "Units.h"
#include "ParameterData.h"
#include "ParameterTable.h"
#include <string>
#include <map>

namespace larcfm {

class CD3DTable : public ParameterTable {

private:
  std::map<std::string,std::string> units_;

public:
  double D;
  double H;

  CD3DTable();

  CD3DTable(double d, double h);

  CD3DTable(double d, const std::string& dunit, double h, const std::string& hunit);

  double getHorizontalSeparation() const;

  void setHorizontalSeparation(double d);

  double getVerticalSeparation() const;

  void setVerticalSeparation(double h);

  double getHorizontalSeparation(const std::string& u) const;

  void setHorizontalSeparation(double d, const std::string& u);

  double getVerticalSeparation(const std::string& u) const;

  void setVerticalSeparation(double h, const std::string& u);

  bool equals(const CD3DTable& t2) const;

  ParameterData getParameters() const;

  void updateParameterData(ParameterData& p) const;

  void setParameters(const ParameterData& p);

  std::string getUnits(const std::string& key) const;

  std::string toString() const;

  std::string toPVS() const;

};

}

#endif
