/*
 * Copyright (c) 2013-2019 United States Government as represented by
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
#include "TrafficState.h"
#include "ConflictData.h"
#include "string_util.h"
#include "ParameterAcceptor.h"
#include <string>

namespace larcfm {

class Detection3D : public ParameterAcceptor {
public:
  virtual ~Detection3D() = 0;

  /* Note: this interface might be better (i.e. more efficient and internally consistent) if all parameters are Euclidean Vect3s.
   * Internally, doing things like taking the dot product of positions and velocities is somewhat iffy from a type-consistency point
   * of view, and also potentially less efficient in C++, due to various type conversions (needs testing).
   * Externally, we have semantically distinct types as inputs, even though they are actually all just Euclidean triples.
   */

  /**
   * This functional call returns true if there is a violation given the current states.
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder position
   * @param vi  intruder velocity
   * @return    true if there is a violation
   */
  bool violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const;

  /**
   * This functional call returns true if there will be a violation between times B and T from now (relative).
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder position
   * @param vi  intruder velocity
   * @param B   beginning of detection time (>=0)
   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
   * @return true if there is a conflict within times B to T
   */
  bool conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const;

  /**
   * This functional call returns a ConflictData object detailing the conflict between times B and T from now (relative), if any.
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder position
   * @param vi  intruder velocity
   * @param D   horizontal separation
   * @param H   vertical separation
   * @param B   beginning of detection time (>=0)
   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
   * @return a ConflictData object detailing the conflict
   */
  virtual ConflictData conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const = 0;

  /**
   * This functional call returns true if there is a violation at time t.
   * @param ownship   ownship state
   * @param intruder  intruder state
   * @param t      time in seconds
   * @return    true if there is a violation at time t
   */
  bool violationAtWithTrafficState(const TrafficState& ownship, const TrafficState& intruder, double t) const;

  /**
   * This functional call returns true if there will be a violation between times B and T from now (relative).
   * @param ownship   ownship state
   * @param intruder  intruder state
   * @param B   beginning of detection time (>=0)
   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
   * @return true if there is a conflict within times B to T
   */
  bool conflictWithTrafficState(const TrafficState& ownship, const TrafficState& intruder, double B, double T) const;

  /**
   * This functional call returns a ConflictData object detailing the conflict between times B and T from now (relative), if any.
   * @param ownship   ownship state
   * @param intruder  intruder state
   * @param D   horizontal separation
   * @param H   vertical separation
   * @param B   beginning of detection time (>=0)
   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
   * @return a ConflictData object detailing the conflict
   */
  virtual ConflictData conflictDetectionWithTrafficState(const TrafficState& ownship, const TrafficState& intruder, double B, double T) const;

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
  virtual std::string toPVS() const = 0;

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
