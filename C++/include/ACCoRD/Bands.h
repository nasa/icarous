/* 
 * Complete Bands for Conflict Prevention (single time version)
 *
 * Contact: Jeff Maddalon, Rick Butler, Cesar Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef BANDS_H_
#define BANDS_H_

#include "Vect2.h"
#include "Vect3.h"
#include "Position.h"
#include "Velocity.h"
#include "Interval.h"
#include "IntervalSet.h"
#include "BandsRegion.h"
#include "BandsCore.h"
#include "TrafficState.h"
#include "GenericDHStateBands.h"
#include "ErrorReporter.h"
#include "ErrorLog.h"
#include <string>

namespace larcfm {

/**
 * Objects of class "Bands" compute the conflict prevention 
 * bands using linear state-based preditions of ownship and (multiple)
 * traffic aircraft positions.  The bands consist of ranges of
 * guidance maneuvers: track angles, ground speeds, and vertical
 * speeds. <p>
 *
 * An assumption of the bands information is that the traffic aircraft
 * do not maneuver.  If the ownship immediately executes a NONE
 * guidance maneuver, then the new path is conflict free (within a
 * lookahead time of the parameter).  If the ownship immediately executes a
 * NEAR guidance maneuver and no traffic aircraft maneuvers, then
 * there will be a loss of separation within the lookahead time.<p>
 *
 * If bands are set to conflict bands only, e.g, setConflictBands,
 * only NEAR bands are computed. In this case, guidance maneuvers for avoiding 
 * conflicts, which are represented by NONE bands, are not computed. Furthermore,
 * bands can be set to certain types of maneuvers by using the methods:
 * setTrackBands, setGroundSpeedBands, and setVerticalSpeedBands. 
 * These methods have to be used before adding any traffic information.
 *
 * Note that in the case of geodetic coordinates, bands performs an 
 * internal projection of the coordinates and velocities into the
 * Euclidean frame (see Util/Projection).  An error will
 * be logged if the distance between traffic and ownship exceeds 
 * Util.Projection.projectionMaxRange().<p>
 * 
 * Disclaimers: Only parts of these algorithms have been formally
 * verified.  We plan to advance the formal verifcation, but it is
 * currently not complete. Even with a 'complete' verification, there
 * will still be certain assumptions.  For instance, the formal proofs
 * use real numbers while these implementations use floating point
 * numbers, so numerical differences could result. In addition, the
 * geodesic computations include certain inaccuracies, especially near
 * the poles.<p>
 *
 * The basic usages is
 * <pre>
 * Bands b(..with configuration parameters..);
 * b.setOwnship(position of ownship, velocity of ownship);
 * b.addTraffic(position of (one) traffic aircraft, velocity of traffic);
 * b.addTraffic(position of (another) traffic aircraft, velocity of traffic);
 * ... add other traffic aircraft ...
 *
 * for (int i = 0; i < b.trackLength(); i++ ) {  
 *    intrval = b.track(i);
 *    lower_ang = intrval.low;
 *    upper_ang = intrval.up;
 *    regionType = b.trackRegion(i);
 *    ..do something with this information..
 * } 
 *
 * ...similar for ground speed and vertical speed...
 * </pre>
 * <p>
 * The variable b may be reused for a different set of aircraft, but it has to be cleared first, i.e.,
 * <pre>
 * b.clear();
 * b.setOwnship(...);
 * b.addTraffic(...);
 * ... etc ...
 * </pre>
 *
 * When any "parameter" to this class is set (separation distance,
 * lookahead time, positions in latitude/longitude, etc.), all
 * previous bands information is cleared; therefore, all traffic
 * aircraft must be added <em>after</em> the parameters are
 * configured.  For more complete example usage see the file
 * <tt>Batch.java</tt>
 *
 */

class Bands : public GenericDHStateBands, ErrorReporter {

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
  Bands(double D, const std::string& dunit, double H, const std::string& hunit, double T, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit);

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
  Bands();

  Bands(const Bands& b);

  //
  // Interface methods...
  //

  /* Enable or disable the computation of conflict bands only. Should be called before adding traffic */
  void setConflictBands(bool cb);

  void doConflictBands();

  bool areConflictBands() const;

  /* Enable or disable the computation of track bands. Should be called before adding traffic */
  void setTrackBands(bool dotrk);

  void enableTrackBands();

  /* Enable or disable the computation of ground speed bands. Should be called before adding traffic */
  void setGroundSpeedBands(bool dogs);

  void enableGroundSpeedBands();

  /* Enable or disable the computation of vertical speed bands. Should be called before adding traffic */
  void setVerticalSpeedBands(bool dovs);

  void enableVerticalSpeedBands();

  /* Enable the computation of track bands only. Should be called before adding traffic */
  void onlyTrackBands();

  /* Enable the computation of ground speed bands only. Should be called before adding traffic */
  void onlyGroundSpeedBands();

  /* Enable the computation of vertical speed bands only. Should be called before adding traffic */
  void onlyVerticalSpeedBands();

  void allBands();

  /**
   * Set the lookahead time [s].  When this parameter is
   * set any existing band information is cleared.
   */
  void setLookaheadTime(double t, const std::string& unit);

  void setTime(double t);

  /** Returns the time of the lookahead time. [s] */
  double getLookaheadTime(const std::string& unit) const;

  /** Returns the time of the lookahead time in seconds */
  double getTime() const;

  void setTimeRange(double b, double t);
  void setTimeRange(double b, double t, const std::string& tunit);
  double getStartTime() const;
  double getStartTime(const std::string& unit) const;


  bool isLatLon() const;

  void setDistance(double d, const std::string& unit);
  double getDistance(const std::string& unit) const;

  void setHeight(double h, const std::string& unit);
  double getHeight(const std::string& unit) const;

  void setMaxGroundSpeed(double gs, const std::string& unit);
  double getMaxGroundSpeed() const;
  double getMaxGroundSpeed(const std::string& unit);

  void setMaxVerticalSpeed(double vs, const std::string& unit);
  double getMaxVerticalSpeed() const;
  double getMaxVerticalSpeed(const std::string& unit);

  void setTrackTolerance(double trk, const std::string& unit);
  double getTrackTolerance(const std::string& unit) const;
  void setGroundSpeedTolerance(double gs, const std::string& unit);
  double getGroundSpeedTolerance(const std::string& unit) const;
  void setVerticalSpeedTolerance(double vs, const std::string& unit);
  double getVerticalSpeedTolerance(const std::string& unit) const;


  //  SPECIAL: in internal units

  void setOwnship(const TrafficState& o);

  void setOwnship(const std::string& id, const Position& p, const Velocity& v);

  void setOwnship(const Position& p, const Velocity& v);

  //  SPECIAL: in internal units
  void addTraffic(const TrafficState& ac);

  void addTraffic(const std::string& id, const Position& pi, const Velocity& vi);

  void addTraffic(const Position& pi, const Velocity& vi);

  void clear();

  // Return false if track bands cannot turned on
  bool turnTrackBandsOn();
  bool turnGroundSpeedBandsOn();
  bool turnVerticalSpeedBandsOn();

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

  /** Return true if track bands consist of a solid region of the given color  */
  bool solidTrackBand(BandsRegion::Region br);

  /** Return true if ground speed bands consist of a region band of the given color  */
  bool solidGroundSpeedBand(BandsRegion::Region br);

  /** Return true if vertical speed bands consist of a region band of the given color  */
  bool solidVerticalSpeedBand(BandsRegion::Region br);

  void mergeTrackBands(Bands& bands);

  void mergeGroundSpeedBands(Bands& bands);

  void mergeVerticalSpeedBands(Bands& bands);

  void mergeBands(Bands& bands);

  std::string toString() const;

  std::string strBands();

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

  TrafficState ownship;

protected:
  mutable ErrorLog error;

private:

  BandsCore red; 
  int traffic;

  std::vector<Interval> trackArray;
  std::vector<BandsRegion::Region> trackRegionArray;
  std::vector<Interval> groundArray;
  std::vector<BandsRegion::Region> groundRegionArray;
  std::vector<Interval> verticalArray;
  std::vector<BandsRegion::Region> verticalRegionArray;

  // [CAM] The following variables handle how stateless bands are recomputed
  bool conflictBands; // Only compute conflict bands
  bool doTrk; // Do compute track bands
  bool doGs;  // Do compute gs bands
  bool doVs;  // Do compute vs bands
  bool computeTrk; // Need compute track bands
  bool computeGs;  // Need compute gs bands
  bool computeVs;  // Need compute vs bands;

  void init(double D, const std::string& dunit, double H, const std::string& hunit, double T, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit);

  void needComputeBands();

  void recompute();

  void resetRegions();

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

  static int order(const std::vector<Interval>& arr, const Interval& n);

  /*
   * Find first band that is equal/not equal to a given bands region and
   * whose size is greater than or equal to a given tolerance. It returns -1
   * if no such band exists.
   */
  static int find_first_explicit_band(bool eq,
      BandsRegion::Region br, double tolerance,
      const std::vector<Interval>& arraylist,
      const std::vector<BandsRegion::Region>& regions);

  /*
   * Find first band that is equal/not equal to an implicit NONE and whose size is
   * greater than or equal to a given tolerance. Returned index i is such that
   * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition.
   * It returns -1 if no such band exists.
   * CAVEAT: This function returns 0, if arraylist is empty. In this case the whole
   * band is NONE.
   *
   */
  static int find_first_implicit_none(double tolerance,
      const std::vector<Interval>& arraylist, const std::vector<BandsRegion::Region>& regions,
      double lb, double ub);

  /*
   * Find first band that is equal/not equal to a given bands region and whose size is
   * greater than or equal to a given tolerance. It returns -1 if no such band exists.
   * If finding a band equal to NONE and the bands is a conflict bands, i.e., it has been
   * set through setConflictBands(), the returned value corresponds to index i such that
   * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition.
   * The parameters lb and up are upper and lower bound for the whole band.
   * CAVEAT: This function returns 0, if arraylist is empty. In this case the whole
   * band is NONE.
   */
  int find_first_band(bool eq, BandsRegion::Region br, double tolerance,
      const std::vector<Interval>& arraylist, const std::vector<BandsRegion::Region>& regions,
      double lb, double ub);

  /*
   * Find first track band that is equal/not equal to a given bands region and whose size is
   * greater than or equal to a given tolerance [rad]. It returns -1 if no such band exists.
   * If finding a band equal to NONE and the bands is a conflict bands, i.e., it has been
   * set through setConflictBands(), the returned value corresponds to index i such that
   * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition.
   * CAVEAT: This function returns 0 when when the band is empty. In this case the whole
   * band is NONE.
   */
  int firstTrackBand(bool eq, BandsRegion::Region br, double trk);

  /*
   * Find first ground speed band that is equal/not equal to a given bands region and
   * whose size is greater than or equal to a given tolerance [m/s]. It returns -1 if
   * no such band exists. If the bands region is NONE and the bands is a conflict bands,
   * i.e., it has been set through setConflictBands(), the returned value corresponds to
   * index i such that (i-1).up and i.low is an implicit NONE that satisfies the tolerance
   * condition.
   * CAVEAT: This function returns 0 when the band is empty. In this case the whole
   * band is NONE.
   */
  int firstGroundSpeedBand(bool eq, BandsRegion::Region br, double gs);

  /*
   * Find first vertical speed band that is equal/not equal to a given bands region and
   * whose size is greater than or equal to a given tolerance [m/s]. It returns -1 if
   * no such band exists. If the bands region is NONE and the bands is a conflict bands,
   * i.e., it has been set through setConflictBands(), the returned value corresponds to
   * index i such that (i-1).up and i.low is an implicit NONE that satisfies the tolerance
   * condition.
   * CAVEAT: This function returns 0 when the band is empty. In this case the whole
   * band is NONE.
   */
  int firstVerticalSpeedBand(bool eq, BandsRegion::Region br, double vs);

  static void toIntervalSet_fromIntervalArray(IntervalSet& intervalset,
      const std::vector<Interval>& intervalarray, const std::vector<BandsRegion::Region> regions,
      BandsRegion::Region br);

  static void toArrays(std::vector<Interval>& intervalarray, std::vector<BandsRegion::Region>& regions,
      const IntervalSet& red, const IntervalSet& green);

  static void toIntervalArray_fromIntervalSet(std::vector<Interval>& intervalarray,
      std::vector<BandsRegion::Region>& regions, const IntervalSet& intervalset);

  void toIntervalArray_fromIntervalSet(std::vector<Interval>& intervalarray,
      std::vector<BandsRegion::Region>& regions, const IntervalSet& intervalset, double lowBound, double upBound);

};

}
#endif
