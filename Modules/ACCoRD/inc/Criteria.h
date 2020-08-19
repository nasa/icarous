/*
 * Criteria.h - determine implicit coordination criteria
 *
 * Contact: Cesar Munoz
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CRITERIA_H_
#define CRITERIA_H_

#include "Vect3.h"
#include "Velocity.h"
#include "Position.h"
#include "Projection.h"
#include "EuclideanProjection.h"

namespace larcfm {

/**
 * Determines if a given velocity vector is within the implicit coordination criteria,
 * both vertically and horizontally.
 *
 * If a resolution velocity v' only changes vertical speed, it is recommended that v' be constructed using Velocity.makeVxyz().
 * Velocity.makeTrkGsVs() may introduce obvious numeric instability in this case. 
 */
class Criteria {
  
public:

  /**
   * Criteria (covers both conflict and loss of separation cases).
   * 
   * @param sox    The ownship position components in (nmi)  [Euclidean]
   * @param soy    The ownship position components in (nmi)  [Euclidean]
   * @param soz    The ownship position components in (ft)  [Euclidean]
   * @param vox    The ownship velocity components in (knots)  [Euclidean]
   * @param voy    The ownship velocity components in (knots)  [Euclidean]
   * @param voz    The ownship velocity components in (fpm)  [Euclidean]
   * @param six    The traffic position components in (nmi)  [Euclidean]
   * @param siy    The traffic position components in (nmi)  [Euclidean]
   * @param siz    The traffic position components in (ft)  [Euclidean]
   * @param vix    The traffic velocity components in (knots)  [Euclidean]
   * @param viy    The traffic velocity components in (knots)  [Euclidean]
   * @param viz    The traffic velocity components in (fpm)  [Euclidean]
   * @param nvox    The NEW ownship velocity components in (knots)  [Euclidean]
   * @param nvoy    The NEW ownship velocity components in (knots)  [Euclidean]
   * @param nvoz    The NEW ownship velocity components in (fpm)  [Euclidean]
   * @param minRelVs minimum relative vertical speed
   * @param D                The diameter of the protection zone in (nm)
   * @param H                The height of the protection zone (ft)
   * @param epsh      The epsilon for horizontal (+1 or -1)
   * @param epsv      The epsilon for vertical (+1 or -1)
   * @return true if the proposed velocity satisfies the criteria
   */
  static bool check(double sox, double soy, double soz,
          double vox, double voy, double voz,
          double six, double siy, double siz,
          double vix, double viy, double viz,
          double nvox, double nvoy, double nvoz,
          double minRelVs,
          double D, double H, int epsh, int epsv);


  /**
   * Criteria (covers both conflict and loss of separation cases).
   * 
   * @param lato    The ownship latitude [deg, north]
   * @param lono    The ownship longitude [deg, east]
   * @param alto    The ownship altitude [ft]
   * @param trko    The ownship track, in [deg] from true north
   * @param gso     The ownship ground speed [knot]
   * @param vso     The ownship vertical speed
   * @param lati    The traffic latitude [deg, north]
   * @param loni    The traffic longitude [deg, east]
   * @param alti    The traffic altitude [ft]
   * @param trki    The traffic track, in [deg] from true north
   * @param gsi     The traffic ground speed [knot]
   * @param vsi     The traffic vertical speed
   * @param ntrko    The NEW ownship track, in [deg] from true north
   * @param ngso     The NEW ownship ground speed [knot]
   * @param nvso     The NEW ownship vertical speed
   *    @param minRelVs minimum relative vertical speed
   * @param D                The diameter of the protection zone in (nmi)
   * @param H                The height of the protection zone (ft)
   * @param epsh      The epsilon for horizontal (+1 or -1)
   * @param epsv      The epsilon for vertical (+1 or -1)
   * @return true if the proposed velocity satisfies the criteria
   */
  static bool checkLL(
            double lato, double lono, double alto,
            double trko, double gso, double vso,
            double lati, double loni, double alti,
            double trki, double gsi, double vsi,
            double ntrko, double ngso, double nvso,
            double minRelVs, double D, double H, int epsh, int epsv);

	/**
	 * Criteria (covers both conflict and loss of separation cases).
	 * 
	 * @param so position of the ownship
	 * @param vo velocity of the ownship
	 * @param si position of the traffic aircraft
	 * @param vi velocity of the traffic aircraft
	 * @param nvo NEW velocity of the ownship
	 * @param minRelVs minimum relative vertical speed
 	 * @param D                The diameter of the protection zone in (nmi)
	 * @param H                The height of the protection zone (ft)
	 * @param epsh      The epsilon for horizontal (+1 or -1)
	 * @param epsv      The epsilon for vertical (+1 or -1)
	 * @return true if the proposed velocity satisfies the criteria
	 */
    static bool check(
            const Position& so, const Velocity& vo,
            const Position& si, const Velocity& vi,
            const Velocity& nvo,
            double minRelVs,  double D, double H, int epsh, int epsv);


};

}
#endif // CRITERIA_H_
