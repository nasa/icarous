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

#ifndef DETECTION3D_H_
#define DETECTION3D_H_

#include "Vect3.h"
#include "Velocity.h"
#include "ParameterData.h"
#include "ConflictData.h"
#include "string_util.h"
#include "ParameterAcceptor.h"
#include <string>

namespace larcfm {

class Detection3D : public ParameterAcceptor {
public:
  virtual ~Detection3D() = 0;

  virtual bool violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const = 0;
  virtual bool conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const = 0;
  virtual ConflictData conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const = 0;

  /** This returns a pointer to a new instance of this type of Detector3D.  You are responsible for destroying this instance when it is no longer needed. */
  virtual Detection3D* copy() const = 0;
  virtual Detection3D* make() const = 0;

  std::string getCanonicalClassName() const {
    return "gov.nasa.larcfm.ACCoRD."+getSimpleClassName();
  }

  std::string getCanonicalSuperClassName() const {
    return "gov.nasa.larcfm.ACCoRD."+getSimpleSuperClassName();
  }

  virtual std::string getSimpleClassName() const = 0;
  virtual std::string getSimpleSuperClassName() const {
    return getSimpleClassName();
  }
  virtual std::string toString() const = 0;
  virtual std::string toPVS(int prec) const {
    (void)prec;
    return "";
  }

  virtual std::string getIdentifier() const = 0;
  virtual void setIdentifier(const std::string& s) = 0;

  /**
   * Return true if two instances have identical parameters (including identifiers).  Use address equality (&x == &y) to distinguish instances.
   * A generic implementation, assuming the identifier is included in the ParameterData object, would be
   *   virtual bool equals(Detection3D* o) const {
   *      if (!larcfm::equals(getClassName(), o->getClassName())) return false;
   *      if (!getParameterData().equals(o->getParameterData())) return false;
   *      return true;
   *    }
   */
  virtual bool equals(Detection3D* o) const = 0;

  virtual bool contains(const Detection3D* cd) const = 0;

  bool instanceOf(const std::string& classname) const {
    return larcfm::equals(getCanonicalClassName(), classname);
  }

};

inline Detection3D::~Detection3D(){}

} /* namespace larcfm */
#endif /* DETECTION3D_H_ */
