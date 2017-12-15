/*
 * CR3D.h
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CR3D_H_
#define CR3D_H_

#include "CDCylinder.h"
#include "ACCoRDConfig.h"
#include "Horizontal.h"
#include "Vertical.h"
#include "Velocity.h"

namespace larcfm {

/**
 * CR3D is an algorithm for 3-D conflict *resolution* that provides
 * avoidance maneuvers for the ownship<p>
 *
 * Unit Convention<br>
 *
 * All units in this file are *internal*:
 * <ul>
 * <li> Units of distance are denoted [d]
 * <li> Units of time are denoted     [t]
 * <li> Units of speed are denoted    [d/t]
 * <li> Units of turn are radians     [rad]
 * </ul>
 *
 * REMARK: X points to East, Y points to North. Angles are in
 *         True North/clockwise convention.
 *
 * Naming Convention<br>
 * The intruder is supposed to be fixed at the origin of the coordinate
 * system.
 *<ul>
 *<li> D  : Diameter of the protected zone [d]
 *<li> H  : Height of the protected zone [d]
 *<li> s  : Relative position of the ownship [d,d,d]
 *<li> vo : Ownship velocity vector  [d/t,d/t,d/t]
 *<li> vi : Intruder velocity vector [d/t,d/t,d/t]
 *</ul>
 *
 * Output class variables<br>
 * <ul>
 * <li>trk [d/t,d/t] (track resolution)
 * <li>gs  [d/t,d/t] (ground speed resolution)
 * <li>opt [d/t,d/t] (optimal horizontal resolution)
 * <li>vs  [d/t]     (vertical speed resolution)
 * </ul>
 *
 */
  class CR3D {

  public:

    /** Track only resolution */
    Horizontal   trk;
    /** Ground speed only resolution */
    Horizontal   gs;
    /** Optimal track/ground speed resolution */
    Horizontal   opt;

    /** Vertical speed resolution */
    Vertical     vs;         


    /**
     * Instantiates a new CR3D object.
     */
    CR3D();
    /**
     * Conflict resolution algorithm
     * 
     * @param s the relative position of the aircraft
     * @param vo the ownship's velocity
     * @param vi the intruder's velocity
     * @param D the minimum horizontal distance
     * @param H the minimum vertical distance
     * 
     * If trk is not zero, trk satisfies
     *   || trk || = || vo ||, and
     * trk is a coordinated track only resolution for the ownship.
     *    
     * If gs is not zero, gs satisfies
     *   exists l > 0 such that l*gs = vo, and
     * gs is a coordinated ground speed only resolution for the ownship.
     *   
     * If opt is not zero, opt is a coordinated optimal horizontal resolution for the ownship.
     *
     * vs is defined ==>
     *   vs is a coordinated vertical only resolution for the ownship.
     * 
     * @return true, if at least one resolution was found.
     */
    bool cr(const Vect3& s,const Vect3& vo,const Vect3& vi,
	    const double D, const double H, int epsh, int epsv);
    
//  /**
//   * Loss of separation recovery algorithm
//   *
//   *
//   * If trk is not zero, trk satisfies
//   *   || trk || = || vo ||, and
//   * trk is a coordinated track only LoS recovery maneuver for the ownship.
//   *
//   * If gs is not zero, gs satisfies
//   *   exists l > 0 such that l*gs = vo, and
//   * gs is a coordinated ground speed only LoS recovery maneuver for the ownship.
//   *
//   * If opt is not zero, opt is a coordinated optimal horizontal LoS recovery maneuver for the ownship.
//   *
//   * vs is defined ==>
//   *   vs is a coordinated vertical only LoS recovery maneuver for the ownship.
//   *
//   * @param s the relative position of the aircraft
//   * @param vo the ownship's velocity
//   * @param vi the intruder's velocity
//   * @param D the minimum horizontal distance
//   * @param H the minimum vertical distance
//   *
//   * @return true, if at least one recovery maneuver was found.
//   */
//    bool losr_old(const Vect3& s, const Vect3& vo, const Vect3& vi,
//	      const double D, const double H, int epsv);

  /**
   * Loss of separation recovery algorithm
   * 
   * 
   * If trk is not zero, trk satisfies
   *   || trk || = || vo ||, and
   * trk is a coordinated track only LoS recovery maneuver for the ownship.
   *    
   * If gs is not zero, gs satisfies
   *   exists l > 0 such that l*gs = vo, and
   * gs is a coordinated ground speed only LoS recovery maneuver for the ownship.
   *   
   * No optimal solution
   *
   * vs is defined ==>
   *   vs is a coordinated vertical only LoS recovery maneuver for the ownship.
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param H the minimum vertical distance
   * 
   * @return true, if at least one recovery maneuver was found.
   */
    bool losr_repulsive(const Vect3& s, const Velocity& vo, const Velocity& vi, double NMAC_D, double NMAC_H,
        double minHorizExitSpeedLoS, double minVertExitSpeedLoS, double minGs, double maxGs, double maxVs, int epsh, int epsv);

//    static Horizontal losr_trk(const Vect3& s, const Vect3& vo, const Vect3& vi,
//	      const double D, const double H);
//
//    static Horizontal losr_gs(const Vect3& s, const Vect3& vo, const Vect3& vi,
// 	      const double D, const double H);
//
//    static Vertical  losr_vs(const Vect3& s, const Vect3& vo, const Vect3& vi,
//   	      const double D, const double H,int epsv);


    /**
     * Conflict resolution algorithm including loss of separation recovery maneuvers
     * 
     * @param s the relative position of the aircraft
     * @param vo the ownship's velocity
     * @param vi the intruder's velocity
     * @param D the minimum horizontal distance
     * @param H the minimum vertical distance
     *
     * @return the type of resolution maneuver:
     * -1 : None available
     *  0 : Conflict resolution
     *  1 : Loss of separation resolution
     */
    int cr3d(const Vect3& s, const Vect3& vo, const Vect3& vi,
	     const double D, const double H, std::string ownship, std::string traffic);

//    /**
//     * Conflict resolution algorithm including loss of separation recovery maneuvers
//     *
//     * @param s the relative position of the aircraft
//     * @param vo the ownship's velocity
//     * @param vi the intruder's velocity
//     * @param D the minimum horizontal distance
//     * @param H the minimum vertical distance
//     * &param Th the minimum time to recover from loss of separation horizontally
//     * &param Tv the minimum time to recover from loss of separation vertically
//     *
//     * @return the type of resolution maneuver:
//     * -1 : None available
//     *  0 : Conflict resolution
//     *  1 : Loss of separation resolution
//     */
//
//        int cr3d(const Vect3& s, const Vect3& vo, const Vect3& vi,
//     	     const double D, const double H, const double Tv, const double Th);
//
//
//    int cr3d_old(const Vect3& s, const Velocity& vo, const Velocity& vi,
//	     const double D, const double H, const double TLos_h, const double TLos_v,
//           const double minGs, const double maxGs, double maxVs, int epsh, int epsv);

  /**
   * Conflict resolution algorithm including loss of separation recovery maneuvers 
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   *
   * @return the type of resolution maneuver:
   * <li> LoSDivg (3)-- Loss of separation resolution
   * <li> LoSConv (2) -- Loss of separation resolution
   * <li> None (-1)-- No resolution available
   * <li> Conflict (1) -- Conflict resolution
   * <li> Unnecessary (0) -- Conflict resolution is not needed
   */
    int cr3d_repulsive(const Vect3& s, const Velocity& vo, const Velocity& vi,
 	     const double D, const double H,
          const double minHorizExitSpeedLoS, double minVertExitSpeedLoS,
          const double minGs, const double maxGs, double maxVs, int epsh, int epsv);

    int cr3d_repulsive_los(Vect3 s, Velocity vo, Velocity vi, double NMAC_D, double NMAC_H,
        double minHorizExitSpeedLoS, double minVertExitSpeedLoS, double minGs, double maxGs, double maxVs,
        int epsh, int epsv);

	// static int trk_repulsive_dir(const Vect2& s, const Vect2& vo, const Vect2& vi);



  /**
   * Iteratively compute an ABSOLUTE track-only loss of separation recovery maneuver for the ownship
   *    @param s        relative position of the ownship
   *    @param vo       velocity of the ownship aircraft
   *    @param vi       velocity of the traffic aircraft
   *    @param minrelgs minimum relative ground speed
   *    @param maxtrk   maximum track [rad]
   *    @param step     track step [rad] (positive)
   *    @param epsh     horizontal epsilon
   *    
   */
    static Horizontal losr_trk_iter(const Vect3& s, const Vect3& vo, const Vect3& vi, double minrelgs, double maxtrk, double step, int epsh);

    static Horizontal losr_gs_iter(const Vect3& s, const Vect3& vo, const Vect3& vi,
                                      double minrelgs, double mings, double maxgs, double step, int epsh);


  /**
   * Compute an ABSOLUTE vertical-only loss of separation recovery maneuver for the ownship
   *    @param s        relative position of the ownship
   *    @param vo       velocity of the ownship aircraft
   *    @param vi       velocity of the traffic aircraft
   *    @param H        height of protected zone, e.g., 1000ft
   *    @param minrelvs minimum relative vertical speed
   *    @param maxvs    maximum absolute vertical speed
   *    @param caD      diameter of collision avoidance area, e.g., 1000 ft
   *    @param caH      height of collision avoidance area, e.g., 200ft
   *    @param epsv     The epsilon for vertical maneuvers (+1 or -1)
   *    
   */
    static Vertical losr_vs_new(const Vect3& s, const Velocity& vo, const Velocity& vi, double minrelvs, double maxvs,
    		double caD, double caH, int epsv);


//    /** returns CR3D track solution via static function
//    * Warning: returns zero vector if no solution present
//    * @param s the relative position of the aircraft
//    * @param vo the ownship's velocity
//    * @param vi the intruder's velocity
//    * @param D the minimum horizontal distance
//    * @param H the minimum vertical distance
//   */
//      static Velocity  old_trk_solution(const Vect3& s, const Vect3& vo, const Vect3& vi, double D, double H, int epsh, int epsv);
//
//    /** returns CR3D ground speed solution via static function
//    * Warning: returns zero vector if no solution present
//    * @param s the relative position of the aircraft
//    * @param vo the ownship's velocity
//    * @param vi the intruder's velocity
//    * @param D the minimum horizontal distance
//    * @param H the minimum vertical distance
//   */
//      static Velocity  old_gs_solution(const Vect3& s, const Vect3& vo, const Vect3& vi, double D, double H, int epsh, int epsv);
//
//    /**
//     * Returns CR3D vertical speed solution via static function
//     * **Warning: returns zero vector if no solution present
//     * @param s the relative position of the aircraft
//     * @param vo the ownship's velocity
//     * @param vi the intruder's velocity
//     * @param D the minimum horizontal distance
//     * @param H the minimum vertical distance
//     */
//      static Velocity  old_vs_solution(const Vect3& s, const Vect3& vo, const Vect3& vi, double D, double H, int epsv);
//


  private:

//	  static int trk_los_iter_dir(const Vect2& s, const Vect2& vo, const Vect2& vi, double step, int eps);  // ??????
	  static Vect2 incr_vect(const Vect2& nvo, double step, int dir);




  };

}

#endif /* CR3D_H_ */
