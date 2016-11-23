/*
 * CriteriaCore.h
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * State-based Implicit Criteria
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CRITERIACORE_H_
#define CRITERIACORE_H_

#include "Velocity.h"
#include "CDCylinder.h"
//#include "CR3D.h"

namespace larcfm {


/**
 * This class provides criteria for
 * 
 *      horizontal conflict               (See ACCoRD PVS theory: horizontal_criterion.pvs)
 *      vertical conflict                 (See ACCoRD PVS theory: vertical_criterion.pvs)
 *      horizontal loss of separation     (See ACCoRD PVS theory: repulsive.pvs)
 *      vertical loss of separation       (See ACCoRD PVS Theory: vertical_los_generic.pvs)
 */
class CriteriaCore {

	
public:

	/**
	 * Horizontal coordination.
	 *
	 * @param s the relative position
	 * @param v the relative velocity
	 *
	 * @return the horizontal coordination sign
	 *   -1 corresponds to turning right in the relative system
	 *   +1 corresponds to turing left in the relative system
	 */
	static int horizontalCoordination(const Vect2& s, const Vect2& v);

  /**
   * Horizontal coordination.
   * 
   * @param s the relative position
   * @param v the relative velocity
   * 
   * @return the horizontal coordination sign
   *   -1 corresponds to turning right in the relative system
   *   +1 corresponds to turning left in the relative system
   */
	static int horizontalCoordination(const Vect3& s, const Vect3& v);


   /**
    * Vertical coordination.
    * 
    * @param s the relative position
    * @param vo the ownship velocity
    * @param vi the traffic aircraft velocity
    * @param D the horizontal distance
    * @param H the vertical distance
    * @param ownship the name of the ownship
    * @param traffic the name of the traffic aircraft
    * 
    * @return the vertical coordination sign.
    *  -1 corresponds to reducing current vertical speed, 
    *  +1 corresponds to increasing current vertical speed
    */
	static int verticalCoordination(const Vect3& s, const Vect3& vo, const Vect3& vi,double D, double H, std::string ownship, std::string traffic);

	/** The fundamental horizontal criterion (Conflict Case)
	 * @param sp  relative position           assumed to be horizontally separated    (Sp_vect2  : TYPE = (horizontal_sep?))
	 * @param v   relative velocity
	 * @param D   protection zone diameter
	 * @param epsh +1 or -1
	 */
	static bool horizontal_criterion(const Vect2& sp, const  Vect2& v, double D, int epsh);

	/** vertical criterion  (Conflict Case) (currently only used in VisualCriteriaCore)
	 * @param epsv  vertical coordination parameter
	 * @param s  relative position
	 * @param v   relative velocity
	 * @param nv  new relative velocity
	 * @param D   protection zone diameter
	 * @param H   protection zone height
	 */
	static bool vertical_criterion(int eps, const Vect3& s, const Vect3& v, const Vect3& nv, double D, double H);

    /**
     * Checks whether a horizontal solution is repulsive or divergent in the turn direction indicated by nvo
     * (2D geometry)
     *    @param s      relative position of the ownship
     *    @param vo     velocity of the ownship aircraft
     *    @param vi     velocity of the traffic aircraft
     *    @param nvo    resolution velocity of the traffic aircraft
     *    @param eps    CR3D.horizontalCoordination
     */
	static bool horizontal_los_criterion(const Vect2& s, const Vect2& vo, const Vect2& vi,const Vect2& nvo, int eps);

	static void printRepulsiveCriteria2DTerms(const Vect2& s, const Vect2& vo, const Vect2& vi,const Vect2& nvo, int eps);
	static void printRepulsiveCriteriaTerms(const Vect3& s, const Vect3& vo, const Vect3& vi,const Vect3& nvo, int eps);


	/**
	 * Checks whether a horizontal solution is repulsive or divergent in the turn direction indicated by nvo3
	 *    @param s3      relative position of the ownship
	 *    @param vo3     velocity of the ownship aircraft
	 *    @param vi3     velocity of the traffic aircraft
	 *    @param nvo3    resolution velocity of the traffic aircraft
	 *    @param eps     CR3D.horizontalCoordination
	 */
	static bool horizontalRepulsiveCriteria(const Vect3& s3, const Velocity& vo3, const Velocity& vi3, const Velocity& nvo3, int eps);

	/** Checks whether a velocity vector satisfies the repulsive LoS criteria
	 * @param s         relative position of the ownship
	 * @param vo        velocity of the ownship aircraft
	 * @param vi        velocity of the traffic aircraft
	 * @param nvo       resolution velocity of the traffic aircraft
	 * @param H         protection zone height
	 * @param minrelvs  minimum relative exit speed
	 * @param epsv      vertical coordination parameter
	 */
	static bool verticalRepulsiveCriterion(const Vect3& s, const Vect3& vo, const Vect3& vi, const Vect3& nvo,
			double H, double minrelvs, int epsh, int epsv);

  /**
   *  true iff the velocity vector nvo satisfies the implicit coordination criteria
   *    @param s        relative position of the ownship
   *    @param vo       velocity of the ownship aircraft
   *    @param vi       velocity of the traffic aircraft
   *    @param nvo      velocity vector to be checked
   *    @param minRelVs desired minimum relative exit ground speed (used in LoS only)
   *    @param D        diameter of the protection zone
   *    @param H        height of the protection zone
   *
   */
	static bool criteria(const Vect3& s, const Velocity&  vo, const Velocity&  vi, const Velocity& nvo,
			double minRelVs, double D, double H, int epsh, int epsv);

	static Vect2 incr_trk_vect(const Vect2& vo, double step, int dir);

	static int losr_trk_iter_dir(const Vect2& s, const Vect2& vo, const Vect2& vi, double step, int eps);


	/** Return an absolute repulsive track search direction (or none)
	 *  Assumes velocity vo is non-zero.
	 * @return +1 search is increasing, -1 search is decreasing
	 */
	static int trkSearchDirection(const Vect3& s, const Vect3& vo, const Vect3& vi, int eps);

	// Compute a new ground speed only vector that is one step to the dir of vo
	// dir = 1 is increasing. Velocity vo is non-zero.
	static Vect2 incr_gs_vect(const Vect2& vo, double step, int dir);

	// Compute an absolute repulsive ground speed direction (or none)
	// dir = 1 is increasing. Velocity vo is non-zero.
	static int losr_gs_iter_dir(const Vect2& s, const Vect2& vo, const Vect2& vi, double mings, double maxgs, double step, int eps);

	/** Return an absolute repulsive ground speed search direction (or none)
	 *  Assumes Velocity vo is non-zero.
	 * @return +1 search is increasing, -1 search is decreasing
	 */
	static int gsSearchDirection(const Vect3& s, const Vect3& vo, const Vect3& vi, int eps);

    /** Return an absolute repulsive vertical speed search direction (or none)
     *  Assumes Velocity vo is non-zero.
     * @return +1 search is increasing, -1 search is decreasing
     */
	static int vsSearchDirection(int epsv);

	/** Return the horizontal epsilon that corresponds to the direction the traffic aircraft is currently turning indicated by sign of trackRate
	 *
	 * @param s             relative position
	 * @param vo            velocity of ownship
	 * @param vi            velocity of intruder
	 * @param epsh          horizontal epsilon
	 * @param trackRate     trackRate of traffic
	 * @return
	 */
	static int dataVsRateEpsilon(const Vect3& s, const Velocity& vo, const Velocity& vi, int epsv, double vsRate);


	/** Return the horizontal epsilon that corresponds to the direction the traffic aircraft is currently turning indicated by sign of trackRate
	 *
	 * @param s             relative position
	 * @param vo            velocity of ownship
	 * @param vi            velocity of intruder
	 * @param epsh          horizontal epsilon
	 * @param trackRate     trackRate of traffic
	 * @return
	 */
	static int dataTurnEpsilon(const Vect3& s, const Velocity& vo, const Velocity& vi, int epsh, double trackRate);

  /** 
   * [CAM] This method replaces horizontal_old_repulsive_criterion. This definition is independent 
   * and coordinated for both state-based and iterative algorithms.  This method is intended to be used
   * with kinematic solvers, that is it is called iteratively with nvo progressing with each iteration.
   * 
   * Checks whether a horizontal solution is repulsive or divergent in the turn direction indicated by nvo 
   * (2D geometry). Derived from PVS predicate: repulsive.repulsive_criteria(s,v,eps)(nv): bool
   *    @param s      relative position of the ownship
   *    @param vo     velocity of the ownship aircraft
   *    @param vi     velocity of the traffic aircraft
   *    @param nvo    resolution velocity of the traffic aircraft
   *    @param eps    CR3D.horizontalCoordination
   */
    static bool horizontal_new_repulsive_criterion(const Vect3& s, const Vect3& vo, const Vect3& vi, const Vect3& nvo, int eps);

  /** 
   * [CAM] This method replaces vertical_old_repulsive_criterion. It's intended to be used for kinematic, 
   * iterative maneuvers. This criterion is like vs_bound_crit? but removes the else branch that restricts 
   * some vertical maneuvers.
   * 
   *    @param s      relative position of the ownship
   *    @param vo     velocity of the ownship aircraft
   *    @param vi     velocity of the traffic aircraft
   *    @param nvo    resolution velocity of the traffic aircraft
   *    @param eps    Vertical coordination
   */
	static bool vertical_new_repulsive_criterion(const Vect3& s, const Vect3& vo, const Vect3& vi, const Vect3& nvo, int eps);

    static int verticalCoordinationLoS(const Vect3& s, const Vect3& vo, const Vect3& vi, std::string ownship, std::string traffic);

private:

	static bool horizontal_criterion_0(const Vect2& sp, int eps, const Vect2& v, double D);

	static int verticalCoordinationConflict(const Vect3& s, const Vect3& v, double D, std::string ownship, std::string traffic);

//	static int sign_vz(const Vect3& s, const double voz, const double viz, std::string ownship, std::string traffic);

	static bool criterion_3D(const Vect3& sp, const Velocity&  v, int epsH, int epsV, const Velocity&  nv, double D, double H);

    // caD and caH are the diameter and height of a collision avoidance zone, e.g., 350ft x 100 ft
    static Vect3 vertical_decision_vect(const Vect3& s, const Vect3& vo, const Vect3& vi, double caD, double caH);

    // Compute an absolute repulsive vertical direction
    static int losr_vs_dir(const Vect3& s, const Vect3& vo, const Vect3& vi, double caD, double caH,  std::string ownship, std::string traffic);

    static bool vs_bound_crit(const Vect3& s, const Vect3& v, const Vect3& nv, int eps);

    static double min_rel_vert_speed(double sz, double vz, int eps, double minrelvs);

    static bool vertical_los_criterion(const Vect3& s, const Vect3& v, const Vect3& nv, int eps, double H, double minrelvs);

    /** Perform a symmetry calculation */
    static int breakSymmetry(const Vect3& s, std::string ownship, std::string traffic);

};

}

#endif
