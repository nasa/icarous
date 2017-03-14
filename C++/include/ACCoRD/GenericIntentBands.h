/* 
 * Bands interface
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef GENERICINTENTBANDS_H_
#define GENERICINTENTBANDS_H_

#include "GenericBands.h"
#include "Plan.h"
#include <string>

namespace larcfm {

/**
 * This provides an interface to classes for various conflict
 * prevention bands. The input to these objects is the position and
 * velocity of the ownship, the output is ranges of guidance
 * maneuvers: track angles, ground speeds, and vertical speeds. <p>
 */
class GenericIntentBands : public GenericBands {

 public:

  virtual ~GenericIntentBands() {};

  /** 
   * Specify the ownship's position and velocity.  Once the ownship
   * is set, any existing 'band' information is cleared. This also
   * sets the coordinate frame for this object.
   *
   * @param s ownship position
   * @param v ownship velocity
   * @param time current time
   */
  virtual void setOwnship(const Position& s, const Velocity& v, double time, const std::string& tunit) = 0;
  
  /** 
   * Specify the ownship's position and velocity.  Once the ownship
   * is set, any existing 'band' information is cleared.  This also
   * sets the bands object to the lat/lon coordinate frame.
   *
   * @param lat north latitude [deg] of ownship
   * @param lon east longitude [deg] of ownship
   * @param alt altitude [feet] of ownship
   * @param trk track angle [degrees clockwise from true north] of ownship
   * @param gs ground speed [knot] of ownship
   * @param vs vertical speed [feet/min] of ownship
   * @param time current time
   */
  virtual void setOwnshipLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit, double time, const std::string& tunit) = 0;
  
  /** 
   * Specify the ownship's position and velocity.  Once the ownship
   * is set, any existing 'band' information is cleared.  This also
   * sets the bands object to the Euclidean coordinate frame.
   *
   * @param sx x position [nmi] of ownship
   * @param sy y position [nmi] of ownship
   * @param sz z position [feet] of ownship
   * @param vx x speed component [knot] of ownship
   * @param vy y speed component [knot] of ownship
   * @param vz z speed component [feet/min] of ownship
   * @param time current time
   */
  virtual void setOwnshipXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit, double time, const std::string& tunit) = 0;

  /**
   * Add a traffic aircraft to this set of the conflict prevention
   * bands. Whether the position are specified in Latitude/longitude
   * or cartesian and the velocities are specified in track, ground
   * speed, or cartesian depends on the setLatLon() and
   * setTrackVelocity() methods.  No traffic aircraft can be added
   * until the ownship is set (see setOwnship()).
   *
   * @param fp intent plan for traffic.  This may be either in nmi or latlong.
   * @return false if no ownship has been set or there is a latlon mismatch
   */
  virtual bool addTraffic(const Plan& fp) = 0;



};

}
#endif
