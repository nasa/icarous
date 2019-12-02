/* 
 * Bands interface
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef GENERICBANDS_H_
#define GENERICBANDS_H_

#include "Vect2.h"
#include "Vect3.h"
#include "BandsRegion.h"
//#include "BandsCore.h"
#include "Interval.h"
#include "IntervalSet.h"
#include <string>

namespace larcfm {

/**
 * This provides an interface to classes for various conflict
 * prevention bands. The input to these objects is the position and
 * velocity of the ownship, the output is ranges of guidance
 * maneuvers: track angles, ground speeds, and vertical speeds. <p>
 */
class GenericBands {

 public:

  virtual ~GenericBands() {};

	/** Returns whether the aircraft positions are specified in
	 * latitude/longitude or Euclidean coordinates. If the ownship
     * has not been set, then this value is undefined.
	 */
	virtual bool isLatLon() const = 0;



//  /** Sets the minimum horizontal separation distance [nmi].  When
//   * this parameter is set any existing band information is
//   * cleared. */
//  virtual void setDiameter(double d, const std::string& unit) = 0;
//  /** Returns the minimum horizontal separation distance [nmi] */
//  virtual double getDiameter(const std::string& uni) const = 0;
//
//  /** Sets the minimum vertical separation distance [feet]. When this
//   * parameter is set any existing band information is cleared. */
//  virtual void setHeight(double h, const std::string& unit) = 0;
//  /** Returns the minimum vertical separation distance. [feet] */
//  virtual double getHeight(const std::string& unit) const = 0;

	/** Set the lookahead time [s].  Any existing bands information is
	 * cleared.
     * If there are more than one lookahead times, this sets the nearest time. */
  virtual void setLookaheadTime(double t, const std::string& unit) = 0;
  /** Returns the lookahead time [s].
   * If there are more than one lookahead times, this gets the nearest time. */
  virtual double getLookaheadTime(const std::string& unit) const = 0;


//  virtual void setTimeRange(double b, double t, const std::string& unit) = 0;
//  virtual double getStartTime(const std::string& unit) const = 0;


  /** Sets the maximum ground speed that is output by Bands, the
   * minimum is 0 [knots]. When this parameter is set any existing
   * band information is cleared. */
  virtual void setMaxGroundSpeed(double gs, const std::string& unit) = 0;
  /** Returns the maximum ground speed that is output by Bands in knots. */
  virtual double getMaxGroundSpeed(const std::string& unit) const = 0;

  /** Sets the range of vertical speeds output by Bands, -max_vs to
   * max_vs [fpm]. When this parameter is set any existing band
   * information is cleared. */
  virtual void setMaxVerticalSpeed(double vs, const std::string& unit) = 0;
  /** 
   * Returns the range of vertical speeds output by Bands [fpm]. Any existing
   * bands information is cleared.
   */
  virtual double getMaxVerticalSpeed(const std::string& unit) const = 0;

  /** Clear all bands information. */
  virtual void clear() = 0;
  
  /** Return the number of bands of track angles */
  virtual int trackLength() = 0;

  /** Return the i-th interval of the track bands in the given unit.  */
  virtual Interval track(int i, const std::string& unit) = 0;
  /** Return the region type of the i-th track band. If a single time
   * version of bands is used, then [NEAR|NONE] is returned.  If a
   * version of bands with two times is used, then [NEAR|MID|NONE]
   * is returned. */
  virtual BandsRegion::Region trackRegion(int i) = 0;
  /** Return the region type of the given track angle [degrees,
   * clockwise from true north].  An angle outside of a 0..360 degree
   * range returns NEAR.  If a single time version of bands is used,
   * then [NEAR|NONE] is returned.  If a version of bands with two
   * times is used, then [NEAR|MID|NONE] is returned. */ 
  virtual BandsRegion::Region regionOfTrack(double trk, const std::string& unit) = 0;

  /** Return the number of bands of ground speeds */
  virtual int groundSpeedLength() = 0;

  /** Return i-th interval of the ground speed bands in the given unit.  */
  virtual Interval groundSpeed(int i, const std::string& unit) = 0;
  /** Return the region type of the i-th ground speed band. If a
   * single time version of bands is used, then [NEAR|NONE] is
   * returned.  If a version of bands with two times is used, then
   * [NEAR|MID|NONE] is returned. */ 
  virtual BandsRegion::Region groundSpeedRegion(int i) = 0;
  /** Return the region type of the given ground speed in knots, out
   * of range speeds are returned as NEAR. If a single time version of
   * bands is used, then [NEAR|NONE] is returned.  If a version of
   * bands with two times is used, then [NEAR|MID|NONE] is
   * returned. */ 
  virtual BandsRegion::Region regionOfGroundSpeed(double gs, const std::string& unit) = 0;

  /** Return the number of bands of vertical speeds */
  virtual int verticalSpeedLength() = 0;

  /** Return the i-th interval of the vertical speed band in the given unit.  */
  virtual Interval verticalSpeed(int i, const std::string& unit) = 0;
  /** Return the region type of the i-th vertical speed band. If a
   * single time version of bands is used, then [NEAR|NONE] is
   * returned.  If a version of bands with two times is used, then
   * [NEAR|MID|NONE] is returned. */ 
  virtual BandsRegion::Region verticalSpeedRegion(int i) = 0;
  /** Return the region type of the given vertical speed [feet/min],
   * out of range speeds are returned as red.  If a single time
   * version of bands is used, then [NEAR|NONE] is returned.  If a
   * version of bands with two times is used, then [NEAR|MID|NONE]
   * is returned. */
  virtual BandsRegion::Region regionOfVerticalSpeed(double vs, const std::string& unit) = 0;

  /** Return a string representing this object. */
  virtual std::string toString() const = 0;


};

}
#endif
