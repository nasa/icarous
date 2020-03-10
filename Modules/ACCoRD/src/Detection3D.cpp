/*
 * Copyright (c) 2018-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Detection3D.h"
#include "ConflictData.h"

namespace larcfm {

/**
 * This functional call returns true if there is a violation given the current states.
 * @param so  ownship position
 * @param vo  ownship velocity
 * @param si  intruder position
 * @param vi  intruder velocity
 * @return    true if there is a violation
 */
bool Detection3D::violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const {
  return conflict(so,vo,si,vi,0.0,0.0);
}

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
bool Detection3D::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  if (Util::almost_equals(B,T)) {
    LossData interval = conflictDetection(so,vo,si,vi,B,B+1);
    return interval.conflict() && Util::almost_equals(interval.getTimeIn(),B);
  }
  if (B>T) {
    return false;
  }
  return conflictDetection(so,vo,si,vi,B,T).conflict();
}

/**
 * This functional call returns true if there is a violation at time t.
 * @param ownship   ownship state
 * @param intruder  intruder state
 * @param t      time in seconds
 * @return    true if there is a violation at time t
 */
bool Detection3D::violationAtWithTrafficState(const TrafficState& ownship, const TrafficState& intruder, double t) const {
  return conflictWithTrafficState(ownship,intruder,t,t);
}

/**
 * This functional call returns true if there will be a violation between times B and T from now (relative).
 * @param ownship   ownship state
 * @param intruder  intruder state
 * @param B   beginning of detection time (>=0)
 * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
 * @return true if there is a conflict within times B to T
 */
bool Detection3D::conflictWithTrafficState(const TrafficState& ownship, const TrafficState& intruder, double B, double T) const {
  if (Util::almost_equals(B,T)) {
    LossData interval = conflictDetectionWithTrafficState(ownship,intruder,B,B+1);
    return interval.conflict() && Util::almost_equals(interval.getTimeIn(),B);
  }
  if (B > T) {
    return false;
  }
  return conflictDetectionWithTrafficState(ownship,intruder,B,T).conflict();
}

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
ConflictData Detection3D::conflictDetectionWithTrafficState(const TrafficState& ownship, const TrafficState& intruder, double B, double T) const {
  return conflictDetection(ownship.get_s(),ownship.get_v(),intruder.get_s(),intruder.get_v(),B,T);
}

}
