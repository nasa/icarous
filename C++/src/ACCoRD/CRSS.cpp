/*
 * CRSS.cpp
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Conflict resolution between an ownship and traffic aircraft using state information.
 *   
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CRSS.h"
#include "CriteriaCore.h"
//#include "UnitSymbols.h"
#include "format.h"

namespace larcfm {

  void CRSS::init(const double d, const double h) {
    D = d;
    H = h;
    trk_only = gs_only = opt_trk = opt_gs = vs_only = 0;
    minHorizExitSpeedLoS = ACCoRDConfig::minHorizExitSpeedLoS_default;
    minVertExitSpeedLoS = ACCoRDConfig::minVertExitSpeedLoS_default;
    maxGs = ACCoRDConfig::maxGs_default;
    minGs =  ACCoRDConfig::minGs_default;    // must be greater than 0
    maxVs = ACCoRDConfig::maxVs_default;
    vsDiscretization = ACCoRDConfig::vsDiscretization_default;
    cr = CR3D();
  }

  /**
    * Create a new state-based conflict detection object using internal units.
    *
    * @param distance the minimum horizontal separation distance [m]
    * @param height the minimum vertical separation height [m].
    */
   CRSS::CRSS() {
     init(Units::from("nm",5),Units::from("ft",1000));
   }


  /**
   * Create a new state-based conflict detection object using internal units.
   * 
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   */
  CRSS::CRSS(const double distance, const double height) {
    init(distance,height);
  }

  CRSS CRSS::make(double distance, const std::string& dStr, double height, const std::string& hStr) {
    return CRSS(Units::from(dStr, distance), Units::from(hStr,height));
  }

  CRSS CRSS::mk(double distance, double height) {
    return CRSS(distance, height);
  }


  /**
   * Returns the minimum horizontal separation distance in internal units [m].
   * 
   * @return the distance
   */
  double CRSS::getDistance() const {
    return D;
  }


  /**
   * Returns the minimum vertical separation distance in internal units.
   * 
   * @return the height in internal units [m]
   */
  double CRSS::getHeight() const {
    return H;
  }

  /**
   * Sets the minimum horizontal separation distance in internal units.
   * 
   * @param distance the distance in internal units [m]
   */
  void CRSS::setDistance(const double distance) {
    D = distance;
  }

  /**
   * Sets the minimum vertical separation distance in internal units.
   * 
   * @param height the height in internal units [m]
   */
  void CRSS::setHeight(const double height) {
    H = height;
  }


  /*
   * sets the minimum ground speed used while searching for a loss of separation solution
   */
  void CRSS::setMinGs(double gs) {
	  minGs = gs;
  }



  /*
   * sets the maximum ground speed used while searching for a loss of separation solution
   */
  void CRSS::setMaxGs(double gs) {
	  maxGs = gs;
  }


  /*
   * sets the maximum vertical speed used while searching for a loss of separation solution
   */
  void CRSS::setMaxVs(double vs) {
	  maxVs = vs;
  }



  /*
   * sets the minimum  speed used while searching for a loss of separation solution
   */
  void CRSS::setMinHorizExitSpeed(double gs) {
	  minHorizExitSpeedLoS = gs;
  }


  /*
   * sets the minimum  speed used while searching for a loss of separation solution
   */
  void CRSS::setMinVertExitSpeed(double gs) {
	  minVertExitSpeedLoS = gs;
  }


  /**
   * Sets the level of discretization for vertical speed, 10 = in units of 10 fpm, 100 = in units of 100 fpm
   *
   * @param vD
   */
  void CRSS::setVsDiscretization(double vD) {
    vsDiscretization = vD;
  }


  /*
   * returns the minimum ground speed used while searching for a loss of separation solution
   */
  double CRSS::getMinGs() {
	  return minGs;
  }

  /*
   * returns the maximum ground speed used while searching for a loss of separation solution
   */

  double CRSS::getMaxGs() {
	  return maxGs;
  }

  /*
   * returns the maximum ground speed used while searching for a loss of separation solution
   */

  double CRSS::getMaxVs() {
      return maxVs;
  }


  /*
   * gets the minimum relative ground speed used while searching for a loss of separation solution
   */
  double CRSS::getMinHorizExitSpeed() {
	  return minHorizExitSpeedLoS;
  }

  /*
    * gets the minimum reative vertical speed used while searching for a loss of separation solution
    */
   double CRSS::getMinVertExitSpeed() {
 	  return minVertExitSpeedLoS;
   }


   /*
     * gets the minimum reative vertical speed used while searching for a loss of separation solution
     */
    double CRSS::getVsDiscretization() {
       return vsDiscretization;
    }


   /**
    * Computes resolution maneuvers for the ownship.
    *
    * @param s the relative position of the aircraft
    * @param vo the ownship's velocity
    * @param vi the intruder's velocity
    *
    * @return type of resolution maneuvers as follows
    *   None        : No resolution available
    *   Conflict    : Conflict resolution
    *   LoS         : Loss of separation resolution
    *   Unnecessary : The current maneuver is good to recover from LoS
    */
   int CRSS::resolution(const Vect3& s, const Velocity& vo, const Velocity& vi, int epsh, int epsv) {
     trk_only = gs_only  = opt_trk  = opt_gs = vs_only = 0;
     int resolution;
     //fpln(" CRSS.resolution:  maxVs = "+Units::str("fpm",maxVs));
     //	   if (ACCoRDConfig::LosRepulsiveCrit)
     //fpln("crss 196: "+Fm12(maxGs));
     resolution = cr.cr3d_repulsive(s,vo,vi,D,H,minHorizExitSpeedLoS,minVertExitSpeedLoS,minGs,maxGs,maxVs,epsh,epsv);
     //	   else
     //		   resolution = cr.cr3d_old(s,vo,vi,D,H,120,90,minGs,maxGs,maxVs,epsh,epsv);
     if (!cr.trk.undef())
       trk_only = cr.trk.compassAngle();
     if (!cr.gs.undef())
       gs_only = cr.gs.norm();
     if (!cr.opt.undef()) {
       opt_trk = cr.opt.compassAngle();
       opt_gs  = cr.opt.norm();
     }
     if (!cr.vs.undef())
       vs_only = Util::discretizeDir(vo.z,cr.vs.z,vsDiscretization);
     return resolution;
   }

   int CRSS::resolution(const Vect3& s, const Velocity& vo, const Velocity& vi, std::string ownship, std::string traffic) {
     Vect3 v = vo.Sub(vi);
      int epsh = CriteriaCore::horizontalCoordination(s,v);
      int epsv = CriteriaCore::verticalCoordination(s,vo,vi,D,H,ownship,traffic);

     return resolution(s,vo,vi,epsh,epsv);
   }

  /**
   * Checks if a track maneuver was found.
   * 
   * @return true, if track-only maneuver is defined
   */
  bool CRSS::hasTrkOnly() const {
    return !cr.trk.undef();
  }

  /**
   * Checks if a ground speed maneuver was found.
   * 
   * @return true, if ground speed-only maneuver is defined
   */
  bool CRSS::hasGsOnly() const {
    return !cr.gs.undef();
  }

  /**
   * Checks if vertical speed maneuver was found.
   * 
   * @return true, if vertical speed-only maneuver was defined
   */
  bool CRSS::hasVsOnly() const {
    return !cr.vs.undef();
  }

  /**
   * Checks if optimal track/ground speed maneuver was found.
   * 
   * @return true, if optimal track/ground speed maneuver was found
   */
  bool CRSS::hasOptTrkGs() const {
    return !cr.opt.undef();
  } 

  /**
   * Track-only maneuver in internal unit.
   * 
   * @return the track maneuver [rad]
   */
  double CRSS::trkOnly() const {
    return trk_only;
  }

  /**
   * Ground speed-only maneuver in internal units.
   * 
   * @return the ground speed-only maneuver [m/s]
   */
  double CRSS::gsOnly() const {
    return gs_only;
  }

  /**
   * Optimal track maneuver in internal units.
   * 
   * @return the track maneuver [rad]
   */
  double CRSS::optTrk() const {
    return opt_trk;
  } 

  /**
   * Optimal ground speed maneuver in internal units.
   * 
   * @return the ground speed maneuver [m/s]
   */
   double CRSS::optGs() const {
    return opt_gs;
  }

  /**
   * Vertical speed-only maneuver in internal units.
   * 
   * @return the vertical speed maneuver [m/s]
   */
  double CRSS::vsOnly() const {
    return vs_only;
  }




  std::string CRSS::toString() const {
	  std::string rtn = "D = "+Units::strX("NM",D)+" H = "+Units::strX("ft",H);
	         rtn = rtn + "  trk_only = "+Fm4(trk_only) + "  gs_only = "+Fm4(gs_only)+"  vs_only = "+Fm4(vs_only)+"\n";
	         rtn = rtn + " minHorizExitSpeedLoS="+Fm12(minHorizExitSpeedLoS);
	         rtn = rtn + " minVertExitSpeedLoS ="+Fm12(minVertExitSpeedLoS);
	         rtn = rtn + " maxGs="+Fm12(maxGs);
	         rtn = rtn + " minGs="+Fm12(minGs);
	         rtn = rtn + " maxVs="+Fm12(maxVs);
	         rtn = rtn + "\n";
	  return rtn;
  }


  // The following methods are deprecated now that Chorus is the preferred interface to ACCoRD functions


  CRSS::CRSS(const double distance, const std::string& ud, const double height, const std::string& uh) {
	  init(Units::from(ud,distance),Units::from(uh,height));
  }

  double CRSS::getDistance(const std::string& ud) const {
	  return Units::to(ud,D);
  }

  double CRSS::getHeight(const std::string& uh) const {
	  return Units::to(uh,H);
  }

  void CRSS::setDistance(const double distance, const std::string& ud) {
	  D = Units::from(ud,distance);
  }

  void CRSS::setHeight(const double height, const std::string& uh) {
	  H = Units::from(uh,height);
  }


  void CRSS::setMinGs(double gs, const std::string& us) {
	  minGs = Units::from(us,gs);
  }

  void CRSS::setMaxGs(double gs, const std::string& us) {
	  maxGs = Units::from(us,gs);
  }

  void CRSS::setMinHorizExitSpeed(double gs, const std::string& us) {
	  minHorizExitSpeedLoS = Units::from(us,gs);
  }

  double CRSS::getMinGs(const std::string& us) {
	  return Units::to(us,minGs);
  }

  double CRSS::getMaxGs(const std::string& us) {
	  return Units::to(us,maxGs);
  }

  double CRSS::getMinHorizExitSpeed(const std::string& us) {
	  return Units::to(us,minHorizExitSpeedLoS);
  }


  double CRSS::trkOnly(const std::string& utrk) const {
    return Units::to("deg",trk_only);
  }

  double CRSS::gsOnly(const std::string& ugs) const {
    return Units::to(ugs,gs_only);
  }

  double CRSS::optTrk(const std::string& utrk) const {
    return Units::to(utrk,opt_trk);
  }

  double CRSS::optGs(const std::string& ugs) const {
    return Units::to(ugs,opt_gs);
  }

  double CRSS::vsOnly(const std::string& uvs) const {
    return Units::to(uvs,vs_only);
  }



  std::string CRSS::strResolutions(int res)    {
  std::string rtn = "";
  switch (res) {
  case None:
      rtn = rtn + " CRSS failed to produce a resolution\n";
      break;
  case Unnecessary:
      // ************ The case where conflict is within   timeSwitchToStateBased but conflict is not on current leg
      // We must somehow create and artificial state projection for subsequent leg
      fpln("  No Resolution Necessary\n");
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
      rtn = rtn + ("  Track Only           = "+ Fm2(trkOnly("deg")) + " [deg]\n");
  }
  if (hasGsOnly()) {
      rtn = rtn + ("  Ground Speed Only    = " + Fm2(gsOnly("kn")) + " [kn]\n");
  }
  if (hasVsOnly()) {
      rtn = rtn + ("  Vertical Speed Only    = " + Fm2(vsOnly("fpm")) + " [fpm]\n");
  }
  if (hasOptTrkGs()) {
      rtn = rtn + ("  Optimal Track        = "+ Fm2(optTrk("deg")) + " [deg]\n");
      rtn = rtn + ("  Optimal Ground Speed = " + Fm2(optGs("kn"))  + " [kn]\n");
  }
//  if (ACCoRDConfig::allowSpeedOnlyResolutions && hasConstantAoA()) {
//    rtn = rtn + ("  cAoA GS        = "+ Fm2(speedOnlyGs("kn")) + " [kn]\n");
//    rtn = rtn + ("  cAoA VS        = " + Fm2(speedOnlyVs("fpm"))  + " [fpm]\n");
//  }
  return rtn;
}


}
