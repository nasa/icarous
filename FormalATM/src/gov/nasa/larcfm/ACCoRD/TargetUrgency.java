/*
 * TargetUrgency
 * 
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * 
 * Determines which traffic aircraft is the highest priority target for avoidance. 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class TargetUrgency {

  /**
   * This will rank to intruders for urgency based on 
   * @param cd detection algorithm to be used -- this object may suffer side effects from the conflictDetection call, so a copy should be used  
   * @param so ownship position
   * @param vo ownship velocity
   * @param si1 intruder1 position
   * @param vi1 intruder1 velocity
   * @param si2 intruder2 position
   * @param vi2 intruder2 velocity
   * @param B detection start time (generally 0)
   * @param T detection lookahead time, t <= 0 is "no data"
   * @return true if intruder 1 is more urgent than intruder 2
   */
  public static boolean mostUrgent(Detection3D cd, Vect3 so, Velocity vo, Vect3 si1, Velocity vi1, Vect3 si2, Velocity vi2, double B, double T) {
    double cdist = so.distanceH(si1);
    ConflictData conflict = cd.conflictDetection(so, vo, si1, vi1, B, T);
    double tin = conflict.getTimeIn();
    double tca = conflict.getCriticalTimeOfConflict();
    double dist_tca = conflict.getDistanceAtCriticalTime(); 
    double p1 = sortValue(cdist,conflict.conflict(),tin,tca,dist_tca,B,T);

    cdist = so.distanceH(si1);
    conflict = cd.conflictDetection(so, vo, si2, vi2, B, T);
    tin = conflict.getTimeIn();
    tca = conflict.getCriticalTimeOfConflict();
    dist_tca = conflict.getDistanceAtCriticalTime(); 
    double p2 = sortValue(cdist,conflict.conflict(),tin,tca,dist_tca,B,T);

    return p1 < p2;
  }

  // calculate the value to sort on.  smaller (or more negative) have priority over larger values.
  // so, ranking values (ptime):
  // ptime > Tdet : indicates we are not in conflict with the traffic, ranked by current horiz. distance
  // 0 > ptime > Tdet : indicates we are in conflict (but not LoS), ranked by time in
  // -1 > ptime > 0 : indicates we are in LoS, but are divergent, ranked by current horiz. distance
  // ptime > -1 : indicates we are in LoS and convergent, ranked by "critical distance"
  // The lowest value should get priority (be sorted into the first position)
  /**
   * Return a priority ranking value far an aircraft s.t. if two ranking values are compared, the lower (possibly negative)
   * value will represent the most urgent aircraft.
   * @param cdist current distance between aircraft, in meters
   * @param conflict true if a conflict was detected
   * @param tin time in for the conflict, in seconds (if conflict)
   * @param tca critical time of conflict, in seconds (if conflict)
   * @param dist_tca distance at critical time, >= 0.0 (if conflict)
   * @param B detection start time (generally 0)
   * @param T detection lookahead time, t <= 0 is "no data"
   * @return urgency sorting value, with positive infinity being least urgent and negative infinity being most urgent
   */
  public static double sortValue(double cdist, boolean conflict, double tin, double tca, double dist_tca, double B, double T) {
//    boolean conflict = cd.conflictDetection(so, vo, si, vi, B, T);
//    double cdist = so.distanceH(si) + 1.0; // add 1 to avoid divide by zero
    
    if (T <= 0 || T < B) {
      // no data
      return Double.MAX_VALUE;
    }
    double ptime;
    // no conflict
    if (!conflict) {
      ptime = cdist + T; // ptime > T
    } else {
      // in conflict (or los)
//      double tin = cd.getTimeIn();
      ptime = tin; // 0 < ptime <= T
      
      if (Util.almost_equals(ptime,  0,0)) {
        // we are in violation.  If we are converging (positive tca), give this a low negative value
        // if we are diverging, this gives a nonzero value based on current distance (closer = more urgent)
//        double tca = cd.getCriticalTimeOfConflict();
        if (Util.almost_greater(tca, 0.0, Util.PRECISION13)) {
          // converging, priority goes to closest distanceAtCriticalTime
//          double dist_tca = cd.getDistanceAtCriticalTime(); 
          ptime = (-1.0 / (dist_tca+1.0)) - 2.0; //  -2.0 < ptime < -3.0, add 1 to avoid div by zero
        } else {
          // if we are diverging, priority goes to the one that is currently (horizontally) closest, use the negative of the current distance fraction
          ptime = -1.0 / (cdist+1.0); // 0 < ptime < -1.0, add 1 to avoid div by zero
        }
      }
    }

    return ptime;
  }

}
