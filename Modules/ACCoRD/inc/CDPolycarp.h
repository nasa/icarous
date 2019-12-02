/*
 * Copyright (c) 2016-2018 United States Government as represented by
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
  /**
   * Polycarp only produces valid answers for "nice" polygons.
   * These are simple polygons that have vertices in a counterclockwise ordering.
   * If all polygons are assured to be "nice" before input, setCheckNice() can be set to FALSE,
   * increasing performance.
   * @param mp static polygon to check
   * @return true if the polygon is "nice", false otherwise.
   */
  bool isNicePolygon(const MovingPolygon3D& mp) const;

  double getBuff() const;
  void setBuff(double x);
  double getFac() const;
  void setFac(double x);
  /**
   * Set the flag for Polycarp to check that polygons are "nice" as part of the detection step.
   * A non-nice polygon is considered bad input to the algorithm.
   * This check is really only needed once for each polygon, so it may be redundant if detection
   * is called repeatedly on an unchanging polygon.
   * If on, this check will also attempt to automatically reverse the order of clockwise polygons.
   * @param b
   */
  void setCheckNice(bool b);
  bool isCheckNice();

  /**
   * Return true if ownship is definitely inside a polygon (fence)
   * @param so ownship position
   * @param si polygon
   * @return true if definitely inside
   */
  bool definitelyInside(const Vect3& so, const Poly3D& si) const;

  /**
   * Return true if ownship is definitely outside a polygon (fence)
   * @param so ownship position
   * @param si polygon
   * @return true if definitely outside
   */
  bool definitelyOutside(const Vect3& so, const Poly3D& si) const;

  /**
   * Return true if ownship is near an edge
   * @param so ownship position
   * @param si polygon
   * @param h approximate horizontal buffer distance from edge
   * @param v approximate vertical buffer distance from edge
   * @return true if ownship is within d of an edge (including top and bottom)
   */
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
