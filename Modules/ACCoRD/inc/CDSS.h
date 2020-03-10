/*
 * CDSS.h
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Conflict detection between an ownship and traffic aircraft using state information.
 *   
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CDSS_H_
#define CDSS_H_

#include "Velocity.h"
#include "CD3D.h"
#include "CDSSCore.h"
#include "CDCylinder.h"
#include "WCV_TAUMOD.h"
#include "string_util.h"

namespace larcfm {

/**
 * The Class CDSS.  This is a dedicated class that only uses the CD3D (CDCylinder) detection algorithm.
 * The more general form is CDSSCore, which may utilize arbitrary Detection3D detection algorithms.
 * 
 * Usage:  CDSS cdss = new CDSS();
 *         cdss.setDistance(5,"NM")
 *         cdss.setHeight(950,"ft")
 *         boolean det = cdss.detection(so,vo,si,vi,T);
 */
class CDSS {

 public:


  CDSS();

  CDSS(CDSSCore& cdsscore);

  /** Create a cylindrical CDSS object from a CDCylinder object
     *
     * @param cdCyl   CDCylinder with detection parameters
     */
  CDSS(CDCylinder& cdCyl);

  CDSS(CDCylinder& cdCyl, double filterTm, const std::string& ut);

  /** Create a cylindrical CDSS with D,H parameters
   *
   * @param D      width of the protection zone
   * @param ud     units of D
   * @param H      height of the protection zone
   * @param uh     units of H
   */
  CDSS(double D, const std::string& ud, double H, const std::string& uh);

  CDSS(WCV_TAUMOD& wcv_TAUMOD);


  static CDSS makeCyl(const std::string& cyid, double D, const std::string& ud, double H, const std::string& uh);


  static CDSS mkCyl(const std::string& cyid, double D, double H, double filter);


  static CDSS mkTauMOD(const std::string& id, double DTHR, double ZTHR, double TTHR, double TCOA);

  void setFilterTime(double filter);

  // needed because of pointer
  /** Copy constructor */
  CDSS(const CDSS& cdss);

  ~CDSS();

  // needed because of pointer
  CDSS& operator=(const CDSS& cdss);

  /**
   * Determines if two aircraft are in loss of separation.
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo velocity of ownship
   * @param si the position of the intruder
   * @param vi velocity of intruder
   * 
   * @return true, if the aircraft are in loss of separation.
   */
  bool lossOfSeparation(const Position& sop, const Velocity&vo, const Position& sip, const Velocity& vi) const;

  bool detection(const Position& sop, const Velocity&vo, const Position& sip, const Velocity& vi, double T, const std::string& ut);

  /**
   * Determines if two aircraft are in conflict within a given lookahed time (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  lookahead time in internal units [s] (T > 0)
   * 
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  bool conflict(const Position& so, const Velocity& vo, const Position& si, const Velocity& vi, double B, double T) const;


  /**
   * Time to loss of separation in explicit units.
   *
   * @param ut the explicit units of time
   *
   * @return the time to loss of separation in explicit units [ut].
   * If time is negative then there is no conflict.
   */
  double getTimeIn(const std::string& ut) const;

  double getCriticalTime(const std::string& ut) const;
  /**
   * Time to recovery of loss of separation in explicit units.
   *
   * @param ut the explicit units of time
   *
   * @return the time to recovery of loss of separation in explicit units [ut].
    */
  double getTimeOut(const std::string& ut) const;

  CDSSCore* getCorePtr();


  std::string toString();

private:

  CDSSCore cdsscore;

};

}

#endif /* CDSS_H_ */
