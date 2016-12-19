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

#ifndef TARGETURGENCY_H_
#define TARGETURGENCY_H_

#include "Vect3.h"
#include "Velocity.h"
#include "Detection3D.h"

namespace larcfm {

/** 
 * 
 * This class is used to determine if one aircraft has priority over another aircraft.  It 
 * is an encapsulation of the "right of way" rules.<p>
 * 
 * Disclaimer: The current rules have been verified to have certain safety and
 * liveness properties, but they have not been validated, that is, they may not 
 * behave "naturally."  As we learn more about what is expected, these rules
 * may evolve in the future.
 * 
 */
class TargetUrgency  {

public:

static bool mostUrgent(Detection3D* cd, const Vect3& so, const Velocity& vo, const Vect3& si1, const Velocity& vi1, const Vect3& si2, const Velocity& vi2, double D, double H, double B, double T);

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
static double sortValue(double cdist, bool conflict, double tin, double tca, double dist_tca, double B, double T);

};

}

#endif
