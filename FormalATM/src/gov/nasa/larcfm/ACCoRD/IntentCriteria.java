/*
 * TrajectoryCriteria - computes if there is implicit coordination between two trajectories
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov)
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Plan;

/**
 * These functions compute if there is implicit coordination between two trajectories.  Currently, 
 * it only computes coordination in the horizontal dimension.  This
 * software is very experimental.  It may change in dramatic ways in the future.
 */
public class IntentCriteria {

  /** 
   * Compute the horizontal trajectory criteria for one instant in time for a pair of aircraft. 
   * @param original the ownship's original plan
   * @param o the ownship's proposed plan
   * @param i the traffic aircraft's plan
   * @param t the time to check for satisfaction of the criteria [s]
   * @param D the minimum horizontal separation distance [nmi]
   * @param H the minimum vertical separation distance [ft] 
   * @return true, if the criteria is satisfied
   * */
  public static boolean horizontal_trajectory_criteria(Plan original, Plan o, Plan i, double t, double D, double H) {
	  return IntentCriteriaCore.horizontal_trajectory_criteria(
			  original, 
			  o, 
			  i, 
			  Units.from(Units.s,t), 
			  Units.from(Units.NM,D), 
			  Units.from(Units.ft,H));
  }
  
  /** 
   * Compute the horizontal trajectory criteria for a range of time for a pair of aircraft. 
   * @param original the ownship's original plan
   * @param o the ownship's proposed plan
   * @param i the traffic aircraft's plan
   * @param t_start the time to start checking for satisfaction of the criteria [s]
   * @param t_end the time to end checking for satisfaction of the criteria [s]
   * @param inc the time increment to iterate between steps [s]
   * @param D the minimum horizontal separation distance [nmi]
   * @param H the minimum vertical separation distance [ft] 
   * @return a value between 0.0 and 1.0.  1.0 means the plan is coordinated.  This value 
   *         represents, as a percentage, the time of first lack of coordination over the 
   *         total time (from t_start to t_end)
   * */
  public static double horizontal_trajectory_criteria_iterative(Plan original, Plan o, Plan i, double t_start, double t_end, double inc, double D, double H) {
	return IntentCriteriaCore.horizontal_trajectory_criteria_iterative(
			original, 
			o, 
			i, 
			Units.from(Units.s, t_start), 
			Units.from(Units.s, t_end),
			Units.from(Units.s, inc), 
			Units.from(Units.NM, D), 
			Units.from(Units.ft, H));
  }
  

  /** 
   * Compute the horizontal trajectory criteria for a range of time for a pair of aircraft. 
   * @param original the ownship's original plan
   * @param o the ownship's proposed plan
   * @param i the traffic aircraft's plan
   * @param t_start the time to start checking for satisfaction of the criteria [s]
   * @param t_end the time to end checking for satisfaction of the criteria [s]
   * @param inc the time increment to iterate between steps [s]
   * @param D the minimum horizontal separation distance [nmi]
   * @param H the minimum vertical separation distance [ft] 
   * @return true, the the plan is coordinated from time t_start to t_end.
   * */
  public static boolean horizontal_trajectory_criteria_holds(Plan original, Plan o, Plan i, double t_start, double t_end, double inc, double D, double H) {
    return horizontal_trajectory_criteria_iterative(original, o, i, t_start, t_end, inc, D, H) == 1.0;
  }

  
}
