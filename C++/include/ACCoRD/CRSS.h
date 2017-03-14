/*
 * CRSS.h
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Conflict resolution between an ownship and traffic aircraft using state information.
 *   
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CRSS_H_
#define CRSS_H_

#include "Velocity.h"
#include "CDCylinder.h"
#include "CR3D.h"
#include <string>
#include "format.h"

namespace larcfm {

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
  class CRSS {

  public:


	 /**
	  * Create a new state-based conflict detection object using internal units.
	  *
     */
	 CRSS();

    /**
     * Create a new state-based conflict detection object using internal units.
     * 
     * @param distance the minimum horizontal separation distance [m]
     * @param height the minimum vertical separation height [m].
     */
    CRSS(const double distance, const double height);

    static CRSS make(double distance, const std::string& dStr, double height, const std::string& hStr);

    static CRSS mk(double distance, double height);


    /**
     * Returns the minimum horizontal separation distance in internal units [m].
     * 
     * @return the distance
     */
    double getDistance() const;

    /**
     * Returns the minimum vertical separation distance in internal units.
     * 
     * @return the height in internal units [m]
     */
    double getHeight() const;

    /**
     * Sets the minimum horizontal separation distance in internal units.
     * 
     * @param distance the distance in internal units [m]
     */
    void setDistance(const double distance);

    /**
     * Sets the minimum vertical separation distance in internal units.
     * 
     * @param height the height in internal units [m]
     */
    void setHeight(const double height);

    /*
     * sets the minimum ground speed used while searching for a loss of separation solution
     */
  /**
   * sets the minimum ground speed used while searching for a loss of separation solution
   */
    void setMinGs(double gs);

    /*
     * sets the maximum ground speed used while searching for a loss of separation solution
     */
  /**
   * sets the maximum ground speed used while searching for a loss of separation solution
   */
    void setMaxGs(double gs) ;

    /*
     * sets the maximum vertical speed used while searching for a loss of separation solution
     */
  /**
   * sets the maximum vertical speed used while searching for a loss of separation solution
   */
    void setMaxVs(double vs);

    /*
     * gets the minimum relative ground speed used while searching for a loss of separation solution
     */
  /**
   * sets the minimum  speed used while searching for a loss of separation solution
   */
    void setMinHorizExitSpeed(double gs);

    /*
     * gets the minimum relative vertical speed used while searching for a loss of separation solution
     */
  /**
   * sets the minimum  speed used while searching for a loss of separation solution
   */
    void setMinVertExitSpeed(double gs);

    /**
      * Sets the level of discretization for vertical speed, 10 = in units of 10 fpm, 100 = in units of 100 fpm
      *
      * @param vD
      */
     void setVsDiscretization(double vD);


    /*
     * returns the minimum ground speed used while searching for a loss of separation solution
     */
     /**
      * returns the minimum ground speed used while searching for a loss of separation solution
      */
     double getMinGs() ;

     /*
      * returns the maximum ground speed used while searching for a loss of separation solution
      */
  /**
   * returns the maximum ground speed used while searching for a loss of separation solution
   */
     double getMaxGs();

     /*
      * returns the maximum vertical speed used while searching for a loss of separation solution
      */
  /**
   * returns the maximum ground speed used while searching for a loss of separation solution
   */
     double getMaxVs();

     /**
      * gets the minimum ground speed used while searching for a loss of separation solution
      */
     double getMinHorizExitSpeed();


     /**
      * Computes resolution maneuvers for the ownship.
      *
      * @param s the relative position of the aircraft
      * @param vo the ownship's velocity
      * @param vi the intruder's velocity
      * @param epsh the horizontal coordination epsilon (e.g. CriteriaCore.horizontalCoordination(s,v))
      * @param epsv the vertical coordination epsilon (e.g. CriteriaCore.verticalCoordination(s,vo,vi,D,H,ownship,traffic))
      *
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
     int resolution(const Vect3& s, const Velocity& vo, const Velocity& vi, int epsh, int epsv);


  /**TODO
   * 
   * @param  s      the relative position of the aircraft
   * @param vo      the ownship's velocity
   * @param vi      the intruder's velocity
   * @param ownship the ownship id 
   * @param traffic the traffic id
   * @return type of resolution
   */
     int resolution(const Vect3& s, const Velocity& vo, const Velocity& vi, std::string ownship, std::string traffic);


    /**
     * Checks if a track maneuver was found.
     * 
     * @return true, if track-only maneuver is defined
     */
    bool hasTrkOnly() const;

    /**
     * Checks if a ground speed maneuver was found.
     * 
     * @return true, if ground speed-only maneuver is defined
     */
    bool hasGsOnly() const;

    /**
     * Checks if vertical speed maneuver was found.
     * 
     * @return true, if vertical speed-only maneuver was defined
     */
    bool hasVsOnly() const;

    /**
     * Checks if optimal track/ground speed maneuver was found.
     * 
     * @return true, if optimal track/ground speed maneuver was found
     */
    bool hasOptTrkGs() const; 

    /**
     * Track-only maneuver in internal unit.
     * 
     * @return the track maneuver [rad]
     */
    double trkOnly() const;

    /**
     * Ground speed-only maneuver in internal units.
     * 
     * @return the ground speed-only maneuver [m/s]
     */
    double gsOnly() const;

    /**
     * Optimal track maneuver in internal units.
     * 
     * @return the track maneuver [rad]
     */
    double optTrk() const; 

    /**
     * Optimal ground speed maneuver in internal units.
     * 
     * @return the ground speed maneuver [m/s]
     */
    double optGs() const;

  /**
   * Vertical speed-only maneuver in internal units.
   * It is recommended that the new velocity be constructed using Velocity.makeVxyz() instead of Velocity.makeTrkGsVs(). 
   * 
   * @return the vertical speed maneuver [m/s]
   */
    double vsOnly() const;

    std::string toString() const;

    // The following methods are deprecated now that Chorus is the preferred interface to ACCoRD functions
    CRSS(const double distance, const std::string& ud, const double height, const std::string& uh);
    double getDistance(const std::string& ud) const;
    double getHeight(const std::string& uh) const;
    void setDistance(const double distance, const std::string& ud);
    void setHeight(const double height, const std::string& uh);
    void setMinGs(double gs, const std::string& us);
    void setMaxGs(double gs, const std::string& us) ;
    void setMinHorizExitSpeed(double gs, const std::string& us);
    double getMinGs(const std::string& us) ;
    double getMaxGs(const std::string& us);
    double getMinHorizExitSpeed(const std::string& us);
  /**
   * gets the minimum vertical speed used while searching for a loss of separation solution
   */
    double getMinVertExitSpeed();
  /**
   * Sets the level of discretization for vertical speed, 10 = in units of 10 fpm, 100 = in units of 100 fpm
   * 
   * @param vD  
   */
    double getVsDiscretization();

    double trkOnly(const std::string& utrk) const;
    double gsOnly(const std::string& ugs) const;
    double vsOnly(const std::string& uvs) const;
    double optTrk(const std::string& utrk) const;
    double optGs(const std::string& ugs) const;

    std::string strResolutions(int res);

  private:

    /** The minimum horizontal distance */
    double D;
  
    /** The minimum vertical distance */
    double H;


    double vsDiscretization;

    CR3D   cr;
    double trk_only;
    double gs_only;
    double opt_trk;
    double opt_gs;
    double vs_only;

    double minHorizExitSpeedLoS; // = Units.from("kn",100.0);
    double minVertExitSpeedLoS; // = Units.from("kn",100.0);
    double maxGs; // = Units.from("kn",700.0);
    double minGs; // = Units.from("kn",150.0);       // must be greater than 0
    double maxVs;


    void init(const double d, const double h);

  };

}

#endif /* CRSS_H_ */
