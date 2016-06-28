/*
 * TrajectoryCriteriaCore - computes if there is implicit coordination between two trajectories
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov)
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Projection;
import gov.nasa.larcfm.Util.EuclideanProjection;

/**
 * These functions compute if there is implicit coordination between two trajectories.  Currently, 
 * it only computes coordination in the horizontal dimension.  This
 * software is very experimental.  It may change in dramatic ways in the future.
 */
public class IntentCriteriaCore {

	/** 
	 * Compute the horizontal trajectory criteria from the given instantaneous information 
	 * about a pair of aircraft.
	 *   
	 * @param soOrig the ownship's current position
	 * @param soNew the ownship's proposed position
	 * @param si the traffic aircraft's current position
	 * @param D the minimum horizontal separation distance in internal units
	 * @param H the minimum vertical separation distance in internal units
	 * @return true, if the criteria is satisfied
	 * */
	public static boolean horizontal_trajectory_criteria(Vect3 soOrig, Vect3 soNew, Vect3 si, double D, double H) {
		Vect3 v = soOrig.Sub(si);
		Vect3 w = soNew.Sub(si);
		double lhs =  v.dot(w);
		double rhs = Math.max(v.norm()*(D/2 + v.norm()/2), v.norm()*D);
		return (lhs >= rhs);
	}	

  /** 
   * Compute the horizontal trajectory criteria for one instant in time for a pair of aircraft. 
   * @param original the ownship's original plan
   * @param o the ownship's proposed plan
   * @param i the traffic aircraft's plan
   * @param t the time to check for satisfaction of the criteria in internal units
   * @param D the minimum horizontal separation distance in internal units
   * @param H the minimum vertical separation distance in internal units
   * @return true, if the criteria is satisfied
   * */
  public static boolean horizontal_trajectory_criteria(Plan original, Plan o, Plan i, double t, double D, double H) {
      if (t >= o.getFirstTime() && t >= i.getFirstTime() && t >= original.getFirstTime() && 
          t <= o.getLastTime() && t <= i.getLastTime() && t <= original.getLastTime()) {
        EuclideanProjection sp = Projection.createProjection(original.position(t).lla());  // this does NOT preserve altitudes (but criteria uses relative position)
        Vect3 so = sp.project(original.position(t));
        Vect3 si = sp.project(i.position(t));
        Vect3 so1 = sp.project(o.position(t));
        return horizontal_trajectory_criteria(so, so1, si, D, H);
      } else {
        return true;
      }
  }
  
  /** 
   * Compute the horizontal trajectory criteria for a range of time for a pair of aircraft. 
   * @param original the ownship's original plan
   * @param o the ownship's proposed plan
   * @param i the traffic aircraft's plan
   * @param t_start the time to start checking for satisfaction of the criteria in internal units
   * @param t_end the time to end checking for satisfaction of the criteria in internal units
   * @param inc the time increment to iterate between steps in internal units
   * @param D the minimum horizontal separation distance in internal units
   * @param H the minimum vertical separation distance in internal units
   * @return a value between 0.0 and 1.0.  1.0 means the plan is coordinated.  This value 
   *         represents, as a percentage, the time of first lack of coordination over the 
   *         total time (from t_start to t_end)
   * */
  public static double horizontal_trajectory_criteria_iterative(Plan original, Plan o, Plan i, double t_start, double t_end, double inc, double D, double H) {
    double totalTime = t_end-t_start;
    double firstFail = totalTime;
//    double numPts = 0.0;
//    double numOk = 0.0;
    for (double t = t_start; t <= t_end; t = t + inc) {
//      numPts++;
      if (!horizontal_trajectory_criteria(original, o, i, t, D, H)) {
        if (firstFail == totalTime) {
          firstFail = t - t_start;
        }
//      } else {
//        numOk++;
      }
    }
    return firstFail/totalTime;
  }
  
}
