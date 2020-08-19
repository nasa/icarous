/* Priority Rules Computation  
 * Authors: Alwyn Goodloe, Cesar Munoz, and Anthony Narkawicz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "TargetUrgency.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Detection3D.h"
#include "Util.h"
#include <limits>
#include <float.h>

namespace larcfm {

bool TargetUrgency::mostUrgent(Detection3D* cd, const Vect3& so, const Velocity& vo, const Vect3& si1, const Velocity& vi1, const Vect3& si2, const Velocity& vi2, double B, double T) {
  double cdist = so.distanceH(si1);
  ConflictData conflict = cd->conflictDetection(so, vo, si1, vi1, B, T);
  double tin = conflict.getTimeIn();
  double tca = conflict.getCriticalTimeOfConflict();
  double dist_tca = conflict.getDistanceAtCriticalTime();
  double p1 = sortValue(cdist,conflict.conflict(),tin,tca,dist_tca,B,T);

  cdist = so.distanceH(si1);
  conflict = cd->conflictDetection(so, vo, si2, vi2, B, T);
  tin = conflict.getTimeIn();
  tca = conflict.getCriticalTimeOfConflict();
  dist_tca = conflict.getDistanceAtCriticalTime();
  double p2 = sortValue(cdist,conflict.conflict(),tin,tca,dist_tca,B,T);

  return p1 < p2;
}

double TargetUrgency::sortValue(double cdist, bool conflict, double tin, double tca, double dist_tca, double B, double T) {
  if (T <= 0 || T < B) {
    // no data
    return DBL_MAX;
  }
  double ptime;
  if (!conflict) {
    ptime = cdist + T; // ptime > T
  } else {
    ptime = tin; // 0 < ptime <= T
    if (Util::almost_equals(ptime,  0,0)) {
      // we are in conflict.  If we are converging (positive tca), give this a low negative value
      // if we are diverging, this gives a nonzero value based on current distance (closer = more urgent)
      // rank this based on the (cylindrical) distance at closest approach (scaled to 1.0)
      if (Util::almost_greater(tca, 0.0, PRECISION13)) {
        // converging
        ptime = (-1.0 / (dist_tca+1.0)) - 2.0; //  -2.0 < ptime < -3.0, add 1 to avoid div by zero
      } else {
        // if we are diverging, priority goes to the one that is currently (horizontally) closest
        ptime = -1.0 / (cdist+1.0); // 0 < ptime < -1.0, add 1 to avoid div by zero
      }
    }
  }

  return ptime;
}

}
