/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * Detection3D.h
 *
 *  Created on: Dec 12, 2012
 *      Author: ghagen
 */

#ifndef DETECTIONPOLYGON_H_
#define DETECTIONPOLYGON_H_

#include "ParameterAcceptor.h"
#include "Poly3D.h"
#include "Vect3.h"
#include "Velocity.h"
#include "ParameterData.h"
#include "MovingPolygon3D.h"

namespace larcfm {

class DetectionPolygon : public ParameterAcceptor {
public:
  virtual ~DetectionPolygon() = 0;
  virtual bool violation(const Vect3& so, const Velocity& vo, const Poly3D& si) const = 0;
  virtual bool conflict(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) const = 0;
  virtual bool conflictDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) = 0;
  virtual std::vector<double> getTimesIn() const = 0;
  virtual std::vector<double> getTimesOut() const = 0;
  virtual std::vector<double> getCriticalTimesOfConflict() const = 0;
  virtual std::vector<double> getDistancesAtCriticalTimes() const = 0;
  virtual ParameterData getParameters() const = 0;
  virtual void updateParameterData(ParameterData& p) const = 0;
  virtual void setParameters(const ParameterData& p) = 0;
  virtual DetectionPolygon* make() const = 0;
  virtual DetectionPolygon* copy() const = 0;

  virtual std::string getClassName() const = 0;
  virtual std::string toString() const = 0;

  virtual std::string getIdentifier() const = 0;
  virtual void setIdentifier(const std::string& s) = 0;

  /**
   * Return true if two instances have identical parameters (including identifiers).  Use address equality (&x == &y) to distinguish instances.
   */
  virtual bool equals(DetectionPolygon* o) const = 0;

  bool instanceof(const std::string& classname) const {
    return larcfm::equals(getClassName(), classname);
  }
};

inline DetectionPolygon::~DetectionPolygon(){}

} /* namespace larcfm */
#endif /* DETECTIONPOLYGON_H_ */
