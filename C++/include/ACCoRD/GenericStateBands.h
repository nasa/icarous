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

#ifndef GENERICSTATEBANDS_H_
#define GENERICSTATEBANDS_H_

#include "Vect2.h"
#include "Vect3.h"
#include "BandsRegion.h"
//#include "BandsCore.h"
#include "Interval.h"
#include "IntervalSet.h"
#include "GenericBands.h"
#include "Position.h"
#include "Velocity.h"
#include <string>

namespace larcfm {

/**
 * This provides an interface to classes for various conflict
 * prevention bands. The input to these objects is the position and
 * velocity of the ownship, the output is ranges of guidance
 * maneuvers: track angles, ground speeds, and vertical speeds. <p>
 */
class GenericStateBands : public GenericBands {

 public:

  virtual ~GenericStateBands() {};

  /** 
   * Specify the ownship's position and velocity.  Once the ownship
   * is set, any existing 'band' information is cleared. This also
   * sets the coordinate frame for this object.
   *
   * @param s ownship position
   * @param v ownship velocity
   */
  virtual void setOwnship(const std::string& id, const Position& s, const Velocity& v) = 0;
  
//  /**
//   * Specify the ownship's position and velocity.  Once the ownship
//   * is set, any existing 'band' information is cleared.  This also
//   * sets the bands object to the lat/lon coordinate frame.
//   *
//   * @param lat north latitude [deg] of ownship
//   * @param lon east longitude [deg] of ownship
//   * @param alt altitude [feet] of ownship
//   * @param trk track angle [degrees clockwise from true north] of ownship
//   * @param gs ground speed [knot] of ownship
//   * @param vs vertical speed [feet/min] of ownship
//   */
//  virtual void setOwnshipLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit) = 0;
//
//  /**
//   * Specify the ownship's position and velocity.  Once the ownship
//   * is set, any existing 'band' information is cleared.  This also
//   * sets the bands object to the Euclidean coordinate frame.
//   *
//   * @param sx x position [nmi] of ownship
//   * @param sy y position [nmi] of ownship
//   * @param sz z position [feet] of ownship
//   * @param vx x speed component [knot] of ownship
//   * @param vy y speed component [knot] of ownship
//   * @param vz z speed component [feet/min] of ownship
//   */
//  virtual void setOwnshipXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit) = 0;
  
  /** 
   * Add a traffic aircraft to this set of the conflict prevention
   * bands. No traffic aircraft can be added
   * until the ownship is set (see setOwnship()).
   * The coordinate frame of the traffic position must be the same as that of the object.
   *
   * @param si position of the traffic
   * @param vi velocity of the traffic
   */
  virtual void addTraffic(const std::string& id, const Position& si, const Velocity& vi) = 0;
  
//  /**
//   * Add a traffic aircraft to this set of the conflict prevention
//   * bands. No traffic aircraft can be added
//   * until the ownship is set (see setOwnship()), and traffic coordinates must be in the same frams as the ownship.
//   *
//   * @param lat north latitude [deg] of the traffic
//   * @param lon east longitude [deg] of the traffic
//   * @param alt altitude [feet] of the traffic
//   * @param trk track angle [degrees clockwise from true north] of the traffic
//   * @param gs ground speed [knot] of the traffic
//   * @param vs vertical speed [feet/min] of the traffic
//   * @return false if no ownship has been set.
//   */
//  virtual bool addTrafficLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit) = 0;
//
//  /**
//   * Add a traffic aircraft to this set of the conflict prevention
//   * bands.  No traffic aircraft can be added
//   * until the ownship is set (see setOwnship()), and traffic coordinates must be in the same frams as the ownship.
//   *
//   * @param sx x position [nmi] of the traffic
//   * @param sy y position [nmi] of the traffic
//   * @param sz z position [feet] of the traffic
//   * @param vx x speed component [knot] of the traffic
//   * @param vy y speed component [knot] of the traffic
//   * @param vz z speed component [feet/min] of the traffic
//   * @return false if no ownship has been set.
//   */
//  virtual bool addTrafficXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit) = 0;

};

}
#endif
