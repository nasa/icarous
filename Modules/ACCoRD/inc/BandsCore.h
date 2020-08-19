/* 
 * Conflict Prevention Bands
 *
 * Contact: Jeff Maddalon, Rick Butler, Cesar Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef BANDSCORE_H_
#define BANDSCORE_H_

#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Interval.h"
#include "IntervalSet.h"
#include <string>

namespace larcfm {

/**
 * Objects of class "BandsCore" compute the conflict prevention bands
 * using linear state-based predictions of ownship and (multiple)
 * traffic aircraft positions.  This class presumes the use "internal" units
 * and Cartesian coordinates and it doesn't perform bookkeeping of its parameters.
 * For instance, if D, H, etc are set after traffic has been added, the method
 * clear() has to be explicitly called. Otherwise, the state of the bands object
 * is undetermined.  For all these reasons, this class is not intended for 
 * general use. It is most appropriate for building algorithms that include 
 * prevention band information. For a more "user-friendly" version, try the Bands class. <p>
 *
 * The bands consist of ranges of guidance maneuvers: track angles,
 * ground speeds, and vertical speeds. If a path is (immediately)
 * taken that is within one these bands and no traffic aircraft
 * maneuvers, then a loss of separation will occur within the
 * specified lookahead time.  Implicitly, any path that is not in a
 * band does not have a loss of separation within the lookahead
 * time.<p>
 *
 * Disclaimers: Only parts of these algorithms have been formally
 * verified.  We plan to advance the formal verification, but it is
 * currently not complete. Even with a 'complete' verification, there
 * will still be certain assumptions.  For instance, the formal proofs
 * use real numbers while these implementations use floating point
 * numbers, so numerical differences could result. In addition, the
 * geodesic computations include certain inaccuracies, especially near
 * the poles.<p>
 *
 *
 * The basic usages is
 * <pre>
 * Bands b(..with configuration parameters..);
 * b.clear();
 * b.addTraffic(relative position of ownship and traffic,
 *              velocity of ownship, 
 *              velocity of one traffic aircraft);
 * b.addTraffic(relative position of ownship and traffic,
 *              velocity of ownship, 
 *              velocity of one traffic aircraft);
 * ...add other traffic aircraft...
 * 
 * IntervalSet track_bands = b.trackBands();
 * IntervalSet ground_speed_bands = b.groundSpeedBands();
 * IntervalSet vertical_speed_bands = b.verticalSpeedBands();
 * </pre>
 *
 * When any "parameter" to this class is set (lookahead time, etc.),
 * any previous bands information is cleared; therefore, all traffic
 * aircraft must be added <em>after</em> the parameters are
 * configured.
 *
 */

class BandsCore {

public:

  /** Construct a BandsCore object with default values for configuration
   *  parameters.
   */
  BandsCore();

  /**
   * Construct a BandsCore object with the given configuration
   * parameters.  The bands always begin as "empty" bands: green
   * track bands from 0 to 2pi, green ground speed bands from 0 to
   * max_gs, and green vertical speed bands from -max_vs to max_vs.
   *
   * @param D   the minimum horizontal separation distance
   * @param H   the minimum vertical separation distance
   * @param T   the lookahead range end time (start time is 0)
   * @param max_gs the maximum ground speed that is output by BandsCore,
   * the minimum is 0.
   * @param max_vs the range of vertical speeds is -max_vs to max_vs [feet/min]
   */
  BandsCore(double D, double H, double T, double max_gs, double max_vs);

  /** 
   * Construct a BandsCore object with the given configuration
   * parameters.  The bands always begin as "empty" bands: green
   * track bands from 0 to 2pi, green ground speed bands from 0 to
   * max_gs, and green vertical speed bands from -max_vs to max_vs.
   *
   * @param D   the minimum horizontal separation distance 
   * @param H   the minimum vertical separation distance
   * @param B   the lookahead range start
   * @param T   the lookahead range end
   * @param max_gs the maximum ground speed that is output by BandsCore,
   * the minimum is 0.
   * @param max_vs the range of vertical speeds is -max_vs to max_vs [feet/min]
   */
  BandsCore(double D, double H, double B, double T, double max_gs, double max_vs);

  virtual ~BandsCore() {};

	/** Set the end lookahead time in internal units. The lookahead range start is set to 0. */
  void setTime(double t);

  void setStartTime(double t);

  /** Returns the end lookahead time in internal units. */
  double getTime() const;

  /** Returns the lookahead start time in internal units. */
  double getStartTime() const;

	/** Set the lookahead time range in internal units with start and end times. */
  void setTimeRange(double b, double t);  

	/** Sets the minimum horizontal separation distance in internal units. */
  void setDiameter(double d);
  /** Returns the minimum horizontal separation distance in internal
   * units. */

  double getDiameter() const;

	/** Sets the minimum vertical separation distance in internal
	 * units. */
  void setHeight(double h);
  /** Returns the minimum vertical separation distance in internal
      units. */

  double getHeight() const;

	/** Sets the maximum ground speed in internal units, the minimum
	 * is 0. */
  void setMaxGroundSpeed(double gs);

  /** Returns the maximum ground speed in internal units.  */
  double getMaxGroundSpeed() const;

	/** Sets the range of vertical speeds in internal units, -max_vs
	 * to max_vs. */
  void setMaxVerticalSpeed(double vs);

  /** 
   * Returns the range of vertical speed in internal units output by
   * BandsCore. 
   */
  double getMaxVerticalSpeed() const;

  /** Sets a minimum size for green/no conflict bands to be allowed, to avoid
   * "eye of the needle" situations. Any green bands smaller than this will not be reported.
   * @param trk minimum acceptable track angle
   */
  virtual void setTrackTolerance(double trk);

  /** Returns minimum acceptable track angle */
  virtual double getTrackTolerance() const;

  /** Sets a minimum size for green/no conflict bands to be allowed, to avoid
   * "eye of the needle" situations. Any green bands smaller than this will not be reported.
   * @param gs minimum acceptable ground speed
   */
  virtual void setGroundSpeedTolerance(double gs);

  /** Returns minimum acceptable ground speed */
  virtual double getGroundSpeedTolerance() const;

  /** Sets a minimum size for green/no conflict bands to be allowed, to avoid
   * "eye of the needle" situations. Any green bands smaller than this will not be reported.
   * @param vs minimum acceptable vertical speed
   */
  virtual void setVerticalSpeedTolerance(double vs);

  /** Returns minimum acceptable ground speed */
  virtual double getVerticalSpeedTolerance() const;


  /**
   * Compute the conflict prevention band information for one
   * ownship/traffic aircraft pair and add this information to any
   * existing band information.  For a collection of "band"
   * information to make sense, all ownship aircraft must be the
   * same.  All parameters are in cartesian coordinates in
   * "internal" units.
   *
   * @param s the relative position of the traffic and ownship aircraft
   * @param vo the velocity of the ownship
   * @param vi the velocity of the traffic
   */
  void addTraffic(const Vect3& s3, const Velocity& vo3, const Velocity& vi3, bool do_trk, bool do_gs, bool do_vs);

  void addTraffic(const Vect3& s3, const Velocity& vo3, const Velocity& vi3);

  /**
   * Clear all bands to "empty" bands.
   */
  void clearTrackRegions();

  void clearGroundSpeedRegions();

  void clearVerticalSpeedRegions();

  void clear(bool do_trk, bool do_gs, bool do_vs);

  void clear();

  /** 
   * The track angle bands.  The angles are in
   * 'compass' angles in internal units, counter-clockwise from true
   * north. 
   */
  const IntervalSet& trackBands() const;

  /** Is there a conflict (within the given lookahead time) for this track angle? */ 
  bool trackBands(double trk) const;

  int trackSize() const;

  /** The ground speed bands in internal units.  The range of
      ground speed bands is from 0 to max_gs. */
  const IntervalSet& groundSpeedBands() const;

  /** Is there a conflict (within the given lookahead time) for this ground speed? */ 
  bool groundSpeedBands(double gs) const; 

  int groundSpeedSize() const;

  /** The vertical speed bands in internal units.  The
      range of vertical speeds is -max_vs to max_vs. */
  const IntervalSet& verticalSpeedBands() const;

  /** Is there a conflict (within the given lookahead time) for this vertical speed? */ 
  bool verticalSpeedBands(double vs) const;

  int verticalSpeedSize() const;

  /**
   * Internally modify the bands to remove narrow bands (----X---- becomes ---------).
   * This should be run after the bands have been populated (if at all).
   * This should be used on "green" bands.  Tolerances define the filter sizes.
   * trkTol in [rad], gsTol in [m/s], vsTol in [m/s]
   */
  void clearNarrowBands(bool do_trk, bool do_gs, bool do_vs);

  /**
   * Internally modify the bands to remove narrow gaps (XXXX-XXXX becomes XXXXXXXXX).
   * This should be run after the bands have been populated (if at all).
   * This should be used on "red" bands.  Tolerances define the filter sizes.
   * trkTol in [rad], gsTol in [m/s], vsTol in [m/s]
   */
  void clearTrackBreaks();
  void clearGroundSpeedBreaks();
  void clearVerticalSpeedBreaks();
  void clearBreaks(bool do_trk, bool do_gs, bool do_vs);
  void clearBreaks();

private:
  /** This method computes the conflict bands for track angles for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.)
   */
  void calcTrkBands(Vect3 s3, Vect3 vo3, Vect3 vi3) const;
  /** This method computes the conflict bands for ground speed for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.)
   */
  void calcGsBands(Vect3 s3, Vect3 vo3, Vect3 vi3) const;
  /** This method computes the conflict bands for vertical speed for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.)
   */
  void calcVsBands(Vect3 s3, Vect3 vo3, Vect3 vi3) const;


public:
  /** Return a string representation of this object */
  std::string toString() const;

private:

  double D;
  double H;
  double B;
  double T;
  double max_gs;
  double max_vs;

  double trkTol;
  double gsTol;
  double vsTol;

  IntervalSet trk_regions;
  IntervalSet gs_regions;
  IntervalSet vs_regions;

  mutable IntervalSet regions; // a temporary variable, for performance

  void init(double D, double H, double B, double T, double max_gs, double max_vs);

};

}

#endif
