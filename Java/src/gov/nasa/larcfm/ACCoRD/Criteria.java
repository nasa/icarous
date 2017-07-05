/*
 * Criteria - determine implicit coordination criteria
 *
 * Contact: Cesar Munoz
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Pair;


/**
 * Determines if a given velocity vector is within the implicit coordination criteria,
 * both vertically and horizontally.
 *
 * If a resolution velocity v' only changes vertical speed, it is recommended that v' be constructed using Velocity.makeVxyz().
 * Velocity.makeTrkGsVs() may introduce obvious numeric instability in this case. 
 */
public class Criteria {
  
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
  public static boolean check(double sox, double soy, double soz, 
		  double vox, double voy, double voz, 
		  double six, double siy, double siz,
		  double vix, double viy, double viz,
		  double nvox, double nvoy, double nvoz,
		  double minRelVs,
		  double D, double H, int epsh, int epsv) {
	  
	  Position so = Position.makeXYZ(sox,soy,soz);
	  Velocity vo = Velocity.makeVxyz(vox, voy, "kn", voz, "fpm");
	  Position si = Position.makeXYZ(six,siy,siz);
	  Velocity vi = Velocity.makeVxyz(vix, viy, "kn", viz, "fpm");
	  Velocity nvo = Velocity.makeVxyz(nvox, nvoy, "kn", nvoz, "fpm");
		  
    return check(so, vo, si, vi, nvo, minRelVs, D, H, epsh, epsv);
  }

  
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
  public static boolean checkLL(
			double lato, double lono, double alto, 
			double trko, double gso, double vso,
			double lati, double loni, double alti,
			double trki, double gsi, double vsi,
			double ntrko, double ngso, double nvso,
            double minRelVs, double D, double H, int epsh, int epsv) {

		Position po = new Position(LatLonAlt.make(lato,lono,alto));
		Velocity vo = Velocity.makeTrkGsVs(trko,"deg", gso,"kn", vso,"fpm");
		Position pi = new Position(LatLonAlt.make(lati,loni,alti));
		Velocity vi = Velocity.makeTrkGsVs(trki,"deg", gsi,"kn", vsi,"fpm");
		Velocity nvo = Velocity.makeTrkGsVs(ntrko,"deg", ngso,"kn", nvso,"fpm");

		return check(po, vo, pi, vi, nvo, minRelVs, D, H, epsh, epsv);
	}

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
	public static boolean check(
			Position so, Velocity vo,
			Position si, Velocity vi,
			Velocity nvo,
            double minRelVs, double D, double H, int epsh, int epsv) {

		if (so.isLatLon() != si.isLatLon()) {
			return false; // if the dimensionality of the two positions does not agree, then always return false
		}
		double D_i = Units.from("nm",D);
		double H_i = Units.from("ft",H);

	    if (so.isLatLon()) {
			EuclideanProjection sp = Projection.createProjection(so.lla());  // this does NOT preserve altitudes (but criteria uses relative position)

			Pair<Vect3,Velocity> po = sp.project(so,vo);
			Pair<Vect3,Velocity> pi = sp.project(si,vi);
			Velocity nvop = sp.projectVelocity(so,nvo);
			
		    return CriteriaCore.criteria(po.first.Sub(pi.first), po.second, pi.second, 
		            nvop, minRelVs, D_i, H_i, epsh, epsv);
		} else {
		    return CriteriaCore.criteria(so.point().Sub(si.point()), 
		             vo, vi, nvo, minRelVs, D_i, H_i, epsh, epsv);
		}
	}

  
}
