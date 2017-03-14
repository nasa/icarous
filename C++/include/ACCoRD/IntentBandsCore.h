/* 
 * Conflict Prevention Bands
 *
 * Contact: George Hagen, Jeff Maddalon, Rick Butler, Cesar Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef INTENTBANDSCORE_H_
#define INTENTBANDSCORE_H_

#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Interval.h"
#include "IntervalSet.h"
#include "Plan.h"
#include <string>

namespace larcfm {

/**
 * Objects of class "IntentBandsCore" compute the conflict prevention bands
 * using linear state-based predictions of ownship and (multiple)
 * traffic aircraft positions.  This class is closely related to
 * "Bands;" however, this class presumes the use of "internal" units
 * and Cartesian coordinates, therefore it is most appropriate for
 * building algorithms that include prevention band information.  For
 * a more "user-friendly" version, try the TripleBands class. <p>
 *                                              
 * The bands consist of ranges of guidance maneuvers: track angles,
 * ground speeds, and vertical speeds. If a path is (immediately)
 * taken that is within one these bands and no traffic aircraft
 * maneuvers, then a loss of separation will occur within the
 * specified lookahead time.  Implicitly, any path that is not in a
 * band does not have a loss of separation within the lookahead
 * time.<p>
 *
 * Note that in the case of geodetic coordinates this version of bands
 * performs an internal projection of the coordinates and velocities
 * into the Euclidean frame (see Util/Projection).  Accuracy may be 
 * reduced if the traffic plans involve any segments longer than
 * Util.Projection.projectionConflictRange(lat,acc), and an error will
 * be logged if the distance between traffic and ownship exceeds 
 * Util.Projection.projectionMaxRange() at any point in the lookahead
 * range. The output velocity values have NOT had the inverse projection 
 * performed on them!<p>
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
 * IntervalSet track_bands = b.trackBands(trk);
 * IntervalSet ground_speed_bands = b.groundSpeedBands(gs);
 * IntervalSet vertical_speed_bands = b.verticalSpeedBands(vs);
 * </pre>
 *
 * When any "parameter" to this class is set (lookahead time, etc.),
 * any previous bands information is cleared; therefore, all traffic
 * aircraft must be added <em>after</em> the parameters are
 * configured.
 *
 */

class IntentBandsCore {

 private:
  static bool allowVariableProtectionZones;
 public:

  /** 
   * Construct an IntentBandsCore object with the given configuration
   * parameters.  The bands always begin as "empty" bands: green
   * track bands from 0 to 2pi, green ground speed bands from 0 to
   * max_gs, and green vertical speed bands from -max_vs to max_vs.
   *
   * @param D   the minimum horizontal separation distance 
   * @param H   the minimum vertical separation distance
   * @param B   the lookahead range start
   * @param T   the lookahead range end
   * @param max_gs the maximum ground speed that is output by IntentBandsCore,
   * the minimum is 0.
   * @param max_vs the range of vertical speeds is -max_vs to max_vs [feet/min]
   */
  IntentBandsCore(double D, double H, double B, double T, double max_gs, double max_vs);
  /** Construct a IntentBandsCore object with default values for configuration
   *  parameters.
   */
  IntentBandsCore();
   
  virtual ~IntentBandsCore() {};

  /** Set the end lookahead time in internal units.  Any existing bands
   * information is cleared. The lookahead range start is set to 0. */
  void setTime(double t);
  /** Returns the end lookahead time in internal units. */
  double getTime() const;
  /** Returns the lookahead start time in internal units. */
  double getStartTime() const;
  /** Set the lookahead time range in internal units with start and 
   * end times.  Any existing bands information is cleared. */
  void setTimeRange(double b, double t);  

  /** Sets the minimum horizontal separation distance in internal units.  Any existing
   * bands information is cleared. */
  void setDiameter(double d);
  /** Returns the minimum horizontal separation distance in internal units */
  double getDiameter() const;

  /** Sets the minimum vertical separation distance in internal
   * units. Any existing bands information is cleared. */
  void setHeight(double h);
  /** Returns the minimum vertical separation distance in internal
      units. */
  double getHeight() const;

  /** Sets the maximum ground speed in internal units, the minimum
   * is 0. Any existing bands information is cleared. */
  void setMaxGroundSpeed(double gs);
  /** Returns the maximum ground speed in internal units.  */
  double getMaxGroundSpeed() const;

  /** Sets the range of vertical speeds in internal units, -max_vs
   * to max_vs. Any existing bands information is cleared. */
  void setMaxVerticalSpeed(double vs);
  /** 
   * Returns the range of vertical speed in internal units. 
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
   * @param so the position of the ownship aircraft at time to
   * @param vo the velocity of the ownship at time to
   * @param to (absolute) time of ownship
   * @param fp intruder flight plan (in XYZ coords)
   */
  void addTraffic(const Vect3& so3, const Velocity& vo3, double to, const Plan& fp);

  /** 
   * Compute the conflict prevention band information for one
   * ownship/traffic aircraft pair and add this information to any
   * existing band information.  For a collection of "band"
   * information to make sense, all ownship aircraft must be the
   * same.  All parameters are in latlon coordinates in
   * "internal" units.
   *
   * @param lat latitude of the ownship aircraft at time to
   * @param lon longitude of the ownship aircraft at time to
   * @param alt altitude of the ownship aircraft at time to
   * @param vo the velocity of the ownship at time to
   * @param to (absolute) time of ownship
   * @param fp intruder flight plan (in XYZ coords)
   */
  void addTrafficLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp);

  
  /** 
   * Clear all bands to "empty" bands.
   */
  void clear();
  
  /** 
   * The track angle bands.  The angles are in
   * 'compass' angles in internal units, counter-clockwise from true
   * north. 
   */
  const IntervalSet& trackBands() const;
  /** The ground speed bands in internal units.  The range of
      ground speed bands is from 0 to max_gs. */
  const IntervalSet& groundSpeedBands() const;
  /** The vertical speed bands in internal units.  The
      range of vertical speeds is -max_vs to max_vs. */
  const IntervalSet& verticalSpeedBands() const;
  
    /** This method computes the conflict bands for track angles for
     * the given aircraft pair.  This method is nearly static, it only
     * relies on the configuration parameters for the class (diameter,
     * time, maximum gs, etc.) 
     */
  IntervalSet calcTrkBands(const Vect3& so3, const Velocity& vo3, double to, const Plan& fp) const;

  /** This method computes the conflict bands for track angles for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.) 
   */
  IntervalSet calcTrkBandsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp) const;
  
    /** This method computes the conflict bands for ground speed for
     * the given aircraft pair.  This method is nearly static, it only
     * relies on the configuration parameters for the class (diameter,
     * time, maximum gs, etc.) 
     */
  IntervalSet calcGsBands(const Vect3& so3, const Velocity& vo3, double to, const Plan& fp) const;

  /** This method computes the conflict bands for ground speed for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.) 
   */
  IntervalSet calcGsBandsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp) const;
  
  /** This method computes the conflict bands for vertical speed for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.) 
   */
  IntervalSet calcVsBands(const Vect3& so3, const Velocity& vo3, double to, const Plan& fp) const;

  /** This method computes the conflict bands for vertical speed for
   * the given aircraft pair.  This method is nearly static, it only
   * relies on the configuration parameters for the class (diameter,
   * time, maximum gs, etc.) 
   */
  IntervalSet calcVsBandsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp) const;
  
	/**
	 * Internally modify the bands to remove narrow bands (----X---- becomes ---------).
	 * This should be run after the bands have been populated (if at all).
	 * This should be used on "green" bands.  Tolerances define the filter sizes.
	 * trkTol in [rad], gsTol in [m/s], vsTol in [m/s]
	 */
	void clearNarrowBands();

	/**
	 * Internally modify the bands to remove narrow gaps (XXXX-XXXX becomes XXXXXXXXX).
	 * This should be run after the bands have been populated (if at all).
	 * This should be used on "red" bands.  Tolerances define the filter sizes.
	 * trkTol in [rad], gsTol in [m/s], vsTol in [m/s]
	 */
	void clearBreaks();


  /** Return a string representation of this object */
  std::string toString() const;

 private:
  IntervalSet trk_regions;
  IntervalSet gs_regions;
  IntervalSet vs_regions;
  
  double D;    
  double H;    
  double B;
  double T; 
  double max_gs;
  double max_vs;

  double trkTol;
  double gsTol;
  double vsTol;
  
  void init(double D, double H, double B, double T, double max_gs, double max_vs);
  
  void pln(std::string str) const;
  void p(std::string str) const;
  
};

}

#endif
