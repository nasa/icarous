/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * CDPolycarp.h
 *
 *  Created on: Dec 23, 2015
 *      Author: ghagen
 */

#ifndef CDPOLYCARP_H_
#define CDPOLYCARP_H_

#include "DetectionPolygon.h"

#include "Poly2D.h"
#include "Poly3D.h"
#include "Vect2.h"
#include "Horizontal.h"
#include "ParameterData.h"
#include "MovingPolygon2D.h"
#include "MovingPolygon3D.h"

namespace larcfm {

class CDPolycarp: public DetectionPolygon {
public:
private:

  std::vector<double> tin;
  std::vector<double> tout;
  std::vector<double> tca;
  std::vector<double> tdist;

  double buff;
  double fac;
  std::string id;
  bool checkNice;


public:

  CDPolycarp();
  CDPolycarp(double b, double f, bool check);

  bool isNicePolygon(const Poly3D& mp) const;
  bool isNicePolygon(const MovingPolygon3D& mp) const;

  double getBuff() const;
  void setBuff(double x);
  double getFac() const;
  void setFac(double x);
  void setCheckNice(bool b);
  bool isCheckNice();

  bool definitelyInside(const Vect3& so, const Poly3D& si) const;
  bool nearEdge(const Vect3& so, const Poly3D& si, double h, double v) const;

  ParameterData getParameters() const;
  void updateParameterData(ParameterData& p) const;
  void setParameters(const ParameterData& p);
  bool violation(const Vect3& so, const Velocity& vo, const Poly3D& si) const;
  bool conflict(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) const;
  bool conflictDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) ;
  std::vector<double> getTimesIn() const ;
  std::vector<double> getTimesOut() const ;
  std::vector<double> getCriticalTimesOfConflict() const ;
  std::vector<double> getDistancesAtCriticalTimes() const ;
  DetectionPolygon* make() const;
  DetectionPolygon* copy() const;
  std::string toString() const;
  std::string getClassName() const;
  std::string getIdentifier() const;
  void setIdentifier(const std::string& s);
  bool equals(DetectionPolygon* d) const;
};

} /* namespace larcfm */

#endif /* SRC_CDPOLYCARP_H_ */
