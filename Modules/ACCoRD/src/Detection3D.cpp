/*
 * Copyright (c) 2018-2020 United States Government as represented by
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

void Detection3D::add_blob(std::vector<std::vector<Position> >& blobs, std::vector<Position>& vin, std::vector<Position>& vout) {
  if (vin.empty() && vout.empty()) {
    return;
  }
  // Add conflict contour
  std::vector<Position> blob = vin;
  blob.insert(blob.end(), vout.begin(), vout.end());
  blobs.push_back(blob);
  vin.clear();
  vout.clear();
}

/**
 * Computes horizontal list of contours contributed by intruder aircraft. A contour is a
 * list of points in counter-clockwise direction representing a polygon.
 * Last point should be connected to first one.
 * @param thr This is a contour threshold in radians [0,pi]. This threshold indicates
 * how far from current direction to look for contours.  A value of 0 means only conflict contour.
 * A value of pi means all contours.
 * @param T Lookahead time in seconds
 *
 * NOTE: The computed polygon should only be used for display purposes since it's merely an
 * approximation of the actual contours defined by the violation and detection methods.
 */
void Detection3D::horizontalContours(std::vector<std::vector<Position> >& blobs, const TrafficState& ownship, const TrafficState& intruder,
    double thr, double T) const {
  std::vector<Position> vin;
  const Position& po = ownship.getPosition();
  const Velocity& vo = ownship.getVelocity();
  double current_trk = vo.trk();
  std::vector<Position> vout;
  /* First step: Computes conflict contour (contour in the current path of the aircraft).
   * Get contour portion to the right.  If los.getTimeIn() == 0, a 360 degree
   * contour will be computed. Otherwise, stops at the first non-conflict degree.
   */
  double right = 0; // Contour conflict limit to the right relative to current direction  [0-2pi rad]
  double step = Pi/180;
  double two_pi = 2*Pi;
  TrafficState own = ownship;
  for (; right < two_pi; right += step) {
    Velocity vop = vo.mkTrk(current_trk+right);
    own.setAirVelocity(vop);
    LossData los = conflictDetectionWithTrafficState(own,intruder,0.0,T);
    if ( !los.conflict() ) {
      break;
    }
    if (los.getTimeIn() != 0 ) {
      // if not in los, add position at time in (counter clock-wise)
      vin.push_back(po.linear(vop,los.getTimeIn()));
    }
    // in any case, add position ad time out (counter clock-wise)
    vout.insert(vout.begin(), po.linear(vop,los.getTimeOut()));
  }
  /* Second step: Compute conflict contour to the left */
  double left = 0;  // Contour conflict limit to the left relative to current direction [0-2pi rad]
  if (0 < right && right < two_pi) {
    /* There is a conflict contour, but not a violation */
    for (left = step; left < two_pi; left += step) {
      Velocity vop = vo.mkTrk(current_trk-left);
      own.setAirVelocity(vop);
      LossData los = conflictDetectionWithTrafficState(own,intruder,0.0,T);
      if ( !los.conflict() ) {
        break;
      }
      vin.insert(vin.begin(), po.linear(vop,los.getTimeIn()));
      vout.push_back(po.linear(vop,los.getTimeOut()));
    }
  }
  add_blob(blobs,vin,vout);
  // Third Step: Look for other blobs to the right within direction threshold
  if (right < thr) {
    for (; right < two_pi-left; right += step) {
      Velocity vop = vo.mkTrk(current_trk+right);
      own.setAirVelocity(vop);
      LossData los = conflictDetectionWithTrafficState(own,intruder,0.0,T);
      if (los.conflict()) {
        vin.push_back(po.linear(vop,los.getTimeIn()));
        vout.insert(vout.begin(), po.linear(vop,los.getTimeOut()));
      } else {
        add_blob(blobs,vin,vout);
        if (right >= thr) {
          break;
        }
      }
    }
    add_blob(blobs,vin,vout);
  }
  // Fourth Step: Look for other blobs to the left within direction threshold
  if (left < thr) {
    for (; left < two_pi-right; left += step) {
      Velocity vop = vo.mkTrk(current_trk-left);
      own.setAirVelocity(vop);
      LossData los = conflictDetectionWithTrafficState(own,intruder,0.0,T);
      if (los.conflict()) {
        vin.insert(vin.begin(), po.linear(vop,los.getTimeIn()));
        vout.push_back(po.linear(vop,los.getTimeOut()));
      } else {
        add_blob(blobs,vin,vout);
        if (left >= thr) {
          break;
        }
      }
    }
    add_blob(blobs,vin,vout);
  }
}

/**
 * Return a list of points (polygon) that approximates the horizontal hazard zone
 * around the ownship, with respect to a traffic aircraft.
 * A polygon is a list of points in counter-clockwise direction, where the last point is connected to the
 * first one.
 * @param T This time represents a time horizon in seconds. When T is 0,
 * the polygon represents the hazard zone. Otherwise, the are represents
 * the hazard zone with time horizon T.
 *
 * NOTE 1: This polygon should only be used for display purposes since it's merely an
 * approximation of the actual hazard zone defined by the violation and detection methods.
 *
 * NOTE 2: This method has to be redefined as appropriate for every specific
 * hazard zone.
 */
void Detection3D::horizontalHazardZone(std::vector<Position>& haz, const TrafficState& ownship, const TrafficState& intruder,
    double T) const {
  haz.clear();
}

}
