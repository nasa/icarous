/* 
 * Complete Bands for Conflict Prevention (single time version)
 *
 * Authors: Jeff Maddalon, Rick Butler, Cesar Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef INTENTBANDS_H_
#define INTENTBANDS_H_

#include "Vect2.h"
#include "Vect3.h"
#include "Position.h"
#include "Interval.h"
#include "IntervalSet.h"
#include "BandsRegion.h"
#include "Velocity.h"
#include "IntentBandsCore.h"
#include "GenericIntentBands.h"
#include "Plan.h"
#include "ErrorReporter.h"
#include "ErrorLog.h"
#include <string>

namespace larcfm {

/**
 * Objects of class "Bands" compute the 'no conflict' and near term
 * conflict prevention (CP) bands using linear state-based preditions
 * of ownship and (multiple) traffic aircraft positions.  The bands
 * consist of ranges of guidance maneuvers: track angles, ground
 * speeds, and vertical speeds. If the ownship immediately executes a
 * "NONE" guidance maneuver and no traffic aircraft maneuvers, then
 * the new path is conflict free within a lookahead time.  If the
 * ownship immediately executes a "NEAR" guidance maneuver and no
 * traffic aircraft maneuvers, then there will be a loss of separation
 * within a lookahead time. <p>
 *
 * Note that in the case of geodetic coordinates this version of bands
 * performs an internal projection of the coordinates and velocities
 * into the Euclidean frame (see Util/Projection).  Accuracy may be 
 * reduced if the traffic plans involve any segments longer than
 * Util.Projection.projectionConflictRange(lat,acc), and an error will
 * be logged if the distance between traffic and ownship exceeds 
 * Util.Projection.projectionMaxRange() at any point in the lookahead
 * range.<p>
 *
 * Disclaimer: Only the mathematical core of these algorithms have
 * been formally verified, certain book-keeping operations have not
 * been verified.  Even with a formal verification, there will still
 * be certain assumptions.  For instance, the formal proofs use real
 * numbers while these implementations use floating point numbers, so
 * numerical differences could result. In addition, the geodesic
 * computations include certain inaccuracies, especially near the
 * earth's poles.<p>
 *
 * The basic usage is
 * <pre>
 * Bands b(..with configuration parameters..);
 * b.setOwnship(position of ownship, velocity of ownship);
 * b.addTraffic(position of traffic, velocity of traffic);
 * b.addTraffic(position of traffic, velocity of traffic);
 * ...add other aircraft...
 *
 * for (int i = 0; i < b.trackLength(); i++) {
 *    use band information from b.track(i) and b.trackRegion(i);
 * }
 * ..similar for ground speed and vertical speed bands..
 * </pre>
 *
 * When any configuration parameter is set (horizontal separation
 * distance, etc.), any previous bands information is cleared out;
 * therefore, all configuration parameters must be set, before any
 * traffic aircraft information is added.  For more complete example
 * usage see the file <tt>Batch.cpp</tt>.
 *
 */

class IntentBands : public GenericIntentBands, ErrorReporter {

public:
  /**
   * Construct a Bands object with the given configuration
   * parameters.  The bands always begin as 'no conflict' bands:
   * track bands from 0 to 2pi, ground speed bands from 0 to
   * max_gs, and vertical speed bands from -max_vs to max_vs.
   * Both the positions and velocities of the aircraft are assumed
   * to be in Euclidean coordinates (see setLatLon() and
   * setTrackVelocity() to change this behavior).
   *
   * @param D      the minimum horizontal separation distance [nmi]
   * @param H      the minimum vertical separation distance [feet]
   * @param T      the near-term lookahead time [s]
   * @param max_gs the maximum ground speed that is output by Bands,
   * the minimum is 0. [knots]
   * @param max_vs the range of vertical speeds is -max_vs to max_vs [feet/min]
   */
  IntentBands(double D, const std::string& dunit, double H, const std::string& hunit, double B, double T, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit);
  /**
   * Construct a Bands object with default values for
   * configuration parameters.  These default values include: 5 nmi
   * horizontal separation, 1000 ft vertical separation, 3 minutes
   * for the lookahead time, 1000 knots max ground
   * speed, 5000 fpm max vertical speed.  The bands always begin as
   * 'no conflict' bands: track bands from 0 to 2pi, ground
   * speed bands from 0 to max_gs, and vertical speed bands
   * from -max_vs to max_vs. Both the positions and velocities of
   * the aircraft are assumed to be in Euclidean coordinates (see
   * setLatLon() and setTrackVelocity() to change this behavior).
   */
  IntentBands();

  /** Set the lookahead time [s].  Any existing bands information is
   * cleared. */
  void setLookaheadTime(double t, const std::string& unit);
  /** Set the lookahead time range [s].  Any existing bands information is
   * cleared. */
  void setTimeRange(double b, double t, const std::string& tunit);

  /** Returns the time of the lookahead time. [s] */
  double getLookaheadTime(const std::string& unit) const;

  /** Returns the time of the lookahead time. [s] */
  double getStartTime(const std::string& unit) const;

  //
  // Interface methods...
  //

  bool isLatLon() const;

  void setDiameter(double d, const std::string& unit);
  double getDiameter(const std::string& unit) const;

  void setHeight(double h, const std::string& unit);
  double getHeight(const std::string& unit) const;

  void setMaxGroundSpeed(double gs, const std::string& unit);
  double getMaxGroundSpeed(const std::string& unit);

  void setMaxVerticalSpeed(double vs, const std::string& unit);
  double getMaxVerticalSpeed(const std::string& unit);

  void setTrackTolerance(double trk, const std::string& unit);
  double getTrackTolerance(const std::string& unit) const;
  void setGroundSpeedTolerance(double gs, const std::string& unit);
  double getGroundSpeedTolerance(const std::string& unit) const;
  void setVerticalSpeedTolerance(double vs, const std::string& unit);
  double getVerticalSpeedTolerance(const std::string& unit) const;


  //  SPECIAL: in internal units
  void setOwnship(const Position& s, const Velocity& v, double t, const std::string& tunit);

  void setOwnshipLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit, double time, const std::string& tunit);

  void setOwnshipXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit, double time, const std::string& tunit);



  /**
   * Add a traffic aircraft flight plan to this set of conflict prevention band
   *
   * @param fp traffic flight plan (may be either latlon or Euclidean)
   */
  bool addTraffic(const Plan& fp);

  void clear();

  int trackLength();
  Interval track(int i, const std::string& unit);
  BandsRegion::Region trackRegion(int i);
  BandsRegion::Region regionOfTrack(double trk, const std::string& unit);

  int groundSpeedLength();
  Interval groundSpeed(int i, const std::string& unit);
  BandsRegion::Region groundSpeedRegion(int i);
  BandsRegion::Region regionOfGroundSpeed(double gs, const std::string& unit);

  int verticalSpeedLength();
  Interval verticalSpeed(int i, const std::string& unit);
  BandsRegion::Region verticalSpeedRegion(int i);
  BandsRegion::Region regionOfVerticalSpeed(double vs, const std::string& unit);

  std::string toString() const;
  // ErrorReporter Interface Methods

  bool hasError() const {
    return error.hasError();
  }
  bool hasMessage() const {
    return error.hasMessage();
  }
  std::string getMessage() {
    return error.getMessage();
  }
  std::string getMessageNoClear() const {
    return error.getMessageNoClear();
  }

private:
  IntentBandsCore red; 

  bool ownship;
  bool needCompute;

  Position so;
  Velocity vo;
  double to;

  static const int NUM_REGIONS = 400;
  Interval trackArray[NUM_REGIONS];
  BandsRegion::Region trackRegionArray[NUM_REGIONS];
  int trackSize;

  Interval groundArray[NUM_REGIONS];
  BandsRegion::Region groundRegionArray[NUM_REGIONS];
  int groundSize;

  Interval verticalArray[NUM_REGIONS];
  BandsRegion::Region verticalRegionArray[NUM_REGIONS];
  int verticalSize;

  mutable ErrorLog error;


  void init(double D, const std::string& dunit, double H, const std::string& hunit, double B, double T, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit);

  void recompute();
  /**
   * Provide a copy of the track angle bands.  The angles are in
   * 'compass' angles: 0 to 360 degrees, counter-clockwise from true
   * north.
   */
  void trackCompute();
  /** Provide a copy of the ground speed bands [knots].  The range of
	ground speed bands is from 0 to max_gs. */
  void groundCompute();
  /** Provide a copy of the vertical speed bands [feet/min].  The
	range of vertical speeds is -max_vs to max_vs. */
  void verticalCompute();

  Interval track(int i, double unit);
  Interval groundSpeed(int i, double unit);
  Interval verticalSpeed(int i, double unit);

  void toArrays(Interval interval[], BandsRegion::Region intRegion[], int& size,
      const IntervalSet& red, const IntervalSet& green);


  void add(Interval arr[], int i, const Interval& r);
  void add(BandsRegion::Region arr[], int i, BandsRegion::Region color);

  int order(Interval arr[], int size, const Interval& n);

};


}
#endif
