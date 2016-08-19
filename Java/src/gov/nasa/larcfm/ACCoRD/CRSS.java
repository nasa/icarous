/*
 * CRSS.java
 * Release: ACCoRDj-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Conflict resolution between an ownship and traffic aircraft
 * using state information.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import static gov.nasa.larcfm.ACCoRD.Consts.*;
import gov.nasa.larcfm.Util.*;

/**
 * Conflict resolution.<p>
 * 
 * Possible resolution values:
 * <ul>
 * <li> LoS -- Loss of separation resolution
 * <li> None -- No resolution available
 * <li> Conflict -- Conflict resolution
 * <li> Unnecessary -- Conflict resolution is not needed
 * </ul>
 *
 */
public class CRSS {

  /** The minimum horizontal distance */
  private double D;

  /** The minimum vertical distance */
  private double H;

  //  private double Th_los = -1.0;    // RWB   if negative not used; use CR3D rather than CR3D_RWB
  //  private double Tv_los = -1.0;    // RWB

  private double minHorizExitSpeedLoS = ACCoRDConfig.minHorizExitSpeedLoS_default;
  private double minVertExitSpeedLoS = ACCoRDConfig.minVertExitSpeedLoS_default;
  private double maxGs; //= Units.from("kn",700.0);
  private double minGs; //= Units.from("kn",150.0);       // must be greater than 0
  private double maxVs;

  private double vsDiscretization; //  = Units.from("fpm",1.0);

  private CR3D   cr;
  private double trk_only = 0;
  private double gs_only  = 0;
  private double opt_trk  = 0;
  private double opt_gs   = 0;
  private double vs_only  = 0;

  private double vel_gs = 0;
  private double vel_vs = 0;

  void init(double d, double h) {
    D = d;
    H = h;
    trk_only = gs_only = opt_trk = opt_gs = vs_only = 0;
    vel_gs = vel_vs = 0;
    minHorizExitSpeedLoS = ACCoRDConfig.minHorizExitSpeedLoS_default;
    minVertExitSpeedLoS = ACCoRDConfig.minVertExitSpeedLoS_default;
    maxGs = ACCoRDConfig.maxGs_default;
    minGs =  ACCoRDConfig.minGs_default;    // must be greater than 0
    maxVs = ACCoRDConfig.maxVs_default;
    vsDiscretization = ACCoRDConfig.vsDiscretization_default;
  }

  /**
   * Create a new state-based conflict detection object using internal units.
   * 
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   */
  public CRSS(double distance, double height) {
    cr = new CR3D();
    init(distance,height);
  }

  public static CRSS make(double distance, String dUnits, double height, String hUnits) {
    return new CRSS(Units.from(dUnits,distance), Units.from(hUnits,height));
  }

  public static CRSS mk(double distance, double height) {
    return new CRSS(distance, height);
  }


  /** copy constructor */
  public CRSS(CRSS crss) {
    D = crss.D;
    H =  crss.H;
    minHorizExitSpeedLoS = crss.minHorizExitSpeedLoS;
    minVertExitSpeedLoS = crss.minVertExitSpeedLoS;
    maxGs = crss.maxGs;
    minGs = crss.minGs;
    maxVs = crss.maxVs;
    vsDiscretization = crss.vsDiscretization;
    cr = new CR3D(crss.cr);
    trk_only = crss.trk_only;
    gs_only  = crss.gs_only;
    opt_trk  = crss.opt_trk;
    opt_gs   = crss.opt_gs;
    vs_only  = crss.vs_only;
    vel_gs = crss.vel_gs;
    vel_vs = crss.vel_vs;
  }

  public CR3D getCR3D() {   // getCore() ??
    return cr;
  } 

  /**
   * Returns the minimum horizontal separation distance in internal units [m].
   * 
   * @return the distance
   */
  public double getDistance() {
    return D;
  }

  /**
   * Returns the minimum vertical separation distance in internal units.
   * 
   * @return the height in internal units [m]
   */
  public double getHeight() {
    return H;
  }

  /**
   * Sets the minimum horizontal separation distance in internal units.
   * 
   * @param distance the distance in internal units [m]
   */
  public void setDistance(double distance) {
    D = distance;
  }

  /**
   * Sets the minimum vertical separation distance in internal units.
   * 
   * @param height the height in internal units [m]
   */
  public void setHeight(double height) {
    H = height;
  }

  /**
   * sets the minimum ground speed used while searching for a loss of separation solution
   */
  public void setMinGs(double gs) {     
    minGs = gs;
  }

  /**
   * sets the maximum ground speed used while searching for a loss of separation solution
   */
  public void setMaxGs(double gs) {     
    maxGs = gs;
  }

  /**
   * sets the maximum vertical speed used while searching for a loss of separation solution
   */
  public void setMaxVs(double vs) {     
    maxVs = vs;
  }

  /**
   * sets the minimum  speed used while searching for a loss of separation solution
   */
  public void setMinHorizExitSpeed(double gs) {     
    minHorizExitSpeedLoS = gs;
  }

  /**
   * sets the minimum  speed used while searching for a loss of separation solution
   */
  public void setMinVertExitSpeed(double gs) {     
    minVertExitSpeedLoS = gs;
  }

  /**
   * Sets the level of discretization for vertical speed, 10 = in units of 10 fpm, 100 = in units of 100 fpm
   * 
   * @param vD  
   */
  public void setVsDiscretization(double vD) {     
    vsDiscretization = vD;
  }

  /**
   * returns the minimum ground speed used while searching for a loss of separation solution
   */
  public double getMinGs() {     
    return minGs;
  }

  /**
   * returns the maximum ground speed used while searching for a loss of separation solution
   */

  public double getMaxGs() {     
    return maxGs;
  }

  /**
   * returns the maximum ground speed used while searching for a loss of separation solution
   */

  public double getMaxVs() {     
    return maxVs;
  }

  /**
   * gets the minimum ground speed used while searching for a loss of separation solution
   */
  public double getMinHorizExitSpeed() {     
    return minHorizExitSpeedLoS;
  }

  /**
   * gets the minimum vertical speed used while searching for a loss of separation solution
   */
  public double getMinVertExitSpeed() {     
    return minVertExitSpeedLoS;
  }

  /**
   * Sets the level of discretization for vertical speed, 10 = in units of 10 fpm, 100 = in units of 100 fpm
   * 
   * @param vD  
   */
  public double getVsDiscretization() {     
    return vsDiscretization;
  }


  /**
   * Computes resolution maneuvers for the ownship.
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param epsh the horizontal coordination epsilon (e.g. CriteriaCore.horizontalCoordination(s,v))
   * @param epsv the vertical coordination epsilon (e.g. CriteriaCore.verticalCoordination(s,vo,vi,D,H,ownship,traffic))
   * 
   * @return type of resolution maneuvers as follows:
   * <ul>
   * <li> LoSDivg (3)-- Loss of separation resolution
   * <li> LoSConv (2) -- Loss of separation resolution
   * <li> None (-1)-- No resolution available
   * <li> Conflict (1) -- Conflict resolution
   * <li> Unnecessary (0) -- Conflict resolution is not needed
   * </ul>
   */
  public int resolution(Vect3 s, Velocity vo, Velocity vi, int epsh, int epsv) {
    trk_only = gs_only  = opt_trk  = opt_gs = vs_only = 0;
    vel_gs = vel_vs = 0;
    int resolution;
    resolution = cr.cr3d_repulsive(s,vo,vi,D,H,minHorizExitSpeedLoS,minVertExitSpeedLoS,minGs,maxGs,maxVs,epsh,epsv);
    if (!cr.trk.undef())
      trk_only = cr.trk.compassAngle();
    if (!cr.gs.undef()) 
      gs_only = cr.gs.norm();
    if (!cr.opt.undef()) {
      opt_trk = cr.opt.compassAngle();
      opt_gs  = cr.opt.norm();
    } 
    if (!cr.vs.undef()) {
      vs_only = Util.discretizeDir(vo.z,cr.vs.z,vsDiscretization);
    }
    if (!cr.vel.undef()) {
      vel_gs = cr.vel.vect2().norm();
      vel_vs = cr.vel.z;
    }
    return resolution;
  }

  /**TODO
   * 
   * @param  s      the relative position of the aircraft
   * @param vo      the ownship's velocity
   * @param vi      the intruder's velocity
   * @param ownship the ownship id 
   * @param traffic the traffic id
   * @return
   */
  public int resolution(Vect3 s, Velocity vo, Velocity vi, String ownship, String traffic) {
    Vect3 v = vo.Sub(vi);
    int epsh = CriteriaCore.horizontalCoordination(s,v);
    int epsv = CriteriaCore.verticalCoordination(s,vo,vi,D,H,ownship,traffic);
    return resolution(s,vo,vi,epsh,epsv);
  }

  /**
   * Checks if a track maneuver was found.
   * 
   * @return true, if track-only maneuver is defined
   */
  public boolean hasTrkOnly() {
    return !cr.trk.undef();
  }

  /**
   * Checks if a ground speed maneuver was found.
   * 
   * @return true, if ground speed-only maneuver is defined
   */
  public boolean hasGsOnly() {
    return !cr.gs.undef();
  }

  /**
   * Checks if vertical speed maneuver was found.
   * 
   * @return true, if vertical speed-only maneuver was defined
   */
  public boolean hasVsOnly() {
    return !cr.vs.undef();
  }

  /**
   * Checks if optimal track/ground speed maneuver was found.
   * 
   * @return true, if optimal track/ground speed maneuver was found
   */
  public boolean hasOptTrkGs() {
    return !cr.opt.undef();
  } 

  public boolean hasSpeedOnly() {
    return !cr.vel.undef();
  }

//  /**
//   * Checks if constant angle of attack (gs/vs change) maneuver was found. (EXPERIMENTAL)
//   * 
//   * @return true, if optimal track/ground speed maneuver was found
//   */
//  public boolean hasConstantAoA() {
//    return !cr.vel.undef();
//  }

  /**
   * Track-only maneuver in internal unit.
   * 
   * @return the track maneuver [rad]
   */
  public double trkOnly() {
    return trk_only;
  }

  /**
   * Ground speed-only maneuver in internal units.
   * 
   * @return the ground speed-only maneuver [m/s]
   */
  public double gsOnly() {
    return gs_only;
  }

  /**
   * Optimal track maneuver in internal units.
   * 
   * @return the track maneuver [rad]
   */
  public double optTrk() {
    return opt_trk;
  } 

  /**
   * Optimal ground speed maneuver in internal units.
   * 
   * @return the ground speed maneuver [m/s]
   */
  public double optGs() {
    return opt_gs;
  }

  /**
   * Vertical speed-only maneuver in internal units.
   * It is recommended that the new velocity be constructed using Velocity.makeVxyz() instead of Velocity.makeTrkGsVs(). 
   * 
   * @return the vertical speed maneuver [m/s]
   */
  public double vsOnly() {
    return vs_only;
  }

  /**
   * Ground speed for a maneuver that only modifies absolute speed (gs & vs change, trk and angle of attack remains the same).
   * 
   * @return the ground speed maneuver [m/s]
   */
  public double speedOnlyGs() {
    return vel_gs;
  }

  /**
   * Vertical speed for a maneuver that only modifies absolute speed (gs & vs change, trk and angle of attack remains the same).
   * 
   * @return the ground speed maneuver [m/s]
   */
  public double speedOnlyVs() {
    return vel_vs;
  }


  public String strResolutions(int res)    { 
    String rtn = "";
    switch (res) {
    case None:
      rtn = rtn + " CRSS failed to produce a resolution\n";
      break;
    case Unnecessary:
      // ************ The case where conflict is within   timeSwitchToStateBased but conflict is not on current leg
      // We must somehow create and artificial state projection for subsequent leg
      f.pln("  No Resolution Necessary\n");
      break;
    case Conflict:
      rtn = rtn + "  Conflict Resolutions\n";
      break;
    case LoSConv:
    case LoSDivg:
      rtn = rtn + "  Loss of Separation Resolutions\n";
      break;
    }
    if (hasTrkOnly()) {
      rtn = rtn + ("  Track Only           = "+ trkOnly("deg") + " [deg]\n");
    }
    if (hasGsOnly()) {
      rtn = rtn + ("  Ground Speed Only    = " + gsOnly("kn") + " [kn]\n");
    }
    if (hasVsOnly()) {
      rtn = rtn + ("  Vertical Speed Only    = " + vsOnly("fpm") + " [fpm]\n");                    
    }
    if (hasOptTrkGs()) {
      rtn = rtn + ("  Optimal Track        = "+ optTrk("deg") + " [deg]\n");
      rtn = rtn + ("  Optimal Ground Speed = " + optGs("kn")  + " [kn]\n");
    }
    if (ACCoRDConfig.allowSpeedOnlyResolutions && hasSpeedOnly()) {
      rtn = rtn + ("  cAoA GS        = "+ speedOnlyGs("kn") + " [kn]\n");
      rtn = rtn + ("  cAoA VS        = " + speedOnlyVs("fpm")  + " [fpm]\n");      
    }
    return rtn;
  }


  public String toString() {
    String rtn = "CRSS: D = "+Units.strX("NM",D)+" H = "+Units.strX("ft",H);
    rtn = rtn + "  trk_only = "+f.Fm4(trk_only) + "  gs_only = "+f.Fm4(gs_only)+"  vs_only = "+f.Fm4(vs_only)+"\n";
    rtn = rtn + " minHorizExitSpeedLoS="+minHorizExitSpeedLoS;
    rtn = rtn + " minVertExitSpeedLoS ="+minVertExitSpeedLoS;
    rtn = rtn + " maxGs="+maxGs;
    rtn = rtn + " minGs="+minGs;
    rtn = rtn + " maxVs="+maxVs;
    rtn = rtn + "\n";
    return rtn;
  }

  // The following methods are deprecated now that Chorus is the preferred interface to ACCoRD functions 


  public CRSS(double distance, String ud, 
      double height, double uh) {
    this(Units.from(ud,distance),Units.from(uh,height));
  }

  public double getDistance(String ud) {
    return Units.to(ud,D);
  }


  public double getHeight(String uh) {
    return Units.to(uh,H);
  }

  public void setDistance(double distance, String ud) {
    D = Units.from(ud,distance);
  }


  public void setHeight(double height, String uh) {     
    H = Units.from(uh,height);
  }

  public void setMinGs(double gs, String us) {     
    minGs = Units.from(us,gs);
  }


  public void setMaxGs(double gs, String us) {     
    maxGs = Units.from(us,gs);
  }

  public void setMinHorizExitSpeed(double gs, String us) {     
    minHorizExitSpeedLoS = Units.from(us,gs);
  }


  public void setMinVertExitSpeed(double gs, String us) {     
    minVertExitSpeedLoS = Units.from(us,gs);
  }

  public double getMinGs(String us) {     
    return Units.to(us,minGs);
  }

  public double getMaxGs(String us) {     
    return Units.to(us,maxGs);
  }

  public double getMinHorizExitSpeed(String us) {     
    return Units.to(us,minHorizExitSpeedLoS);
  }


  public double getMinVertExitSpeed(String us) {     
    return Units.to(us,minVertExitSpeedLoS);
  }


  public double trkOnly(String utrk) {
    return Units.to("deg",trk_only);
  }


  public double gsOnly(String ugs) {
    return Units.to(ugs,gs_only);
  }


  public double optTrk(String utrk) {
    return Units.to(utrk,opt_trk);
  }


  public double optGs(String ugs) {
    return Units.to(ugs,opt_gs);
  } 

  public double vsOnly(String uvs) {
    return Units.to(uvs,vs_only);
  } 

  public double speedOnlyGs(String uvs) {
    return Units.to(uvs,vel_gs);
  } 

  public double speedOnlyVs(String uvs) {
    return Units.to(uvs,vel_vs);
  } 

}
