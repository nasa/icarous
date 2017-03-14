/* 
 * Conflict Prevention Bands (two time version)
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CONFLICTBANDS2_H_
#define CONFLICTBANDS2_H_

#include "Vect2.h"
#include "Vect3.h"
#include "Position.h"
#include "Velocity.h"
#include "Interval.h"
#include "IntervalSet.h"
#include "BandsRegion.h"
#include "GenericDHStateBands.h"
#include "BandsCore.h"
#include "ErrorReporter.h"
#include "ErrorLog.h"
#include <string>

namespace larcfm {
  
  /**
   * Objects of class "ConflictBandsTwoTimes" compute the near term, and
   * mid term conflict prevention (CP) bands using linear state-based
   * preditions of ownship and (multiple) traffic aircraft positions.
   * The bands consist of ranges of guidance maneuvers: track angles,
   * ground speeds, and vertical speeds. If the ownship immediately
   * executes an "MID" guidance maneuver and no traffic aircraft
   * maneuvers, then the new path is conflict free within the near term
   * lookahead time, but there is a conflict before mid term lookahead
   * time.  If the ownship immediately executes a "NEAR" guidance
   * maneuver and no traffic aircraft maneuvers, then there will be a
   * loss of separation within the near term lookahead time. <p>
   *
   * Note that in the case of geodetic coordinates, bands performs an 
   * internal projection of the coordinates and velocities into the
   * Euclidean frame (see Util/Projection).  An error will
   * be logged if the distance between traffic and ownship exceeds 
   * Util.Projection.projectionMaxRange().<p>
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
   * ConflictBandsTwoTimes b(..with configuration parameters..);
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
   */
  
  class ConflictBandsTwoTimes : public GenericDHStateBands, ErrorReporter {
    
  public:
	/** 
	 * Construct a ConflictBandsTwoTimes object with the given
	 * configuration parameters.  A new ConflictBandsTwoTimes object
	 * always begins with empty band information.  Both the positions
	 * and velocities of the aircraft are assumed to be in Euclidean
	 * coordinates (see setLatLon() and setTrackVelocity() to change
	 * this behavior).
	 *
	 * @param D      the minimum horizontal separation distance [nmi]
	 * @param H      the minimum vertical separation distance [feet]
	 * @param Tnear  the near-term lookahead time [s]
	 * @param Tmid   the mid-term lookahead time [s]
	 * @param max_gs the maximum ground speed that is output by Bands,
	 * the minimum is 0. [knots]
	 * @param max_vs the range of vertical speeds is -max_vs to max_vs [feet/min]
	 */
    ConflictBandsTwoTimes(double D, const std::string& dunit, double H, const std::string& hunit, double Tnear, double Tmid, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit);
	/** 
	 * Construct a ConflictBandsTwoTimes object with default values
	 * for configuration parameters.  These default values include: 5
	 * nmi horizontal separation, 1000 ft vertical separation, 3
	 * minutes for a near term lookahead time, 5 minutes for a mid
	 * term lookahead time, 1000 knots max ground speed, 5000 fpm max
	 * vertical speed.  A new ConflictBandsTwoTimes object always
	 * begins empty.  Both the positions and velocities of the
	 * aircraft are assumed to be in Euclidean coordinates (see
	 * setLatLon() and setTrackVelocity() to change this behavior).
	 */
    ConflictBandsTwoTimes();
    
	/** Set the near-term lookahead time [s].  Any existing bands
	 * information is cleared. */
    void setTimeNear(double t, const std::string& unit);
    /** Returns the time of the near-term lookahead time. [s] */
    double getTimeNear(const std::string& unit) const;
    
	/** 
	 * Sets the mid-term lookahead time [s]. It is assumed that the
	 * mid-term lookahead time is greater than or equal to the
	 * near-term lookahead time, then it is ignored. Any
	 * existing bands information is cleared. */
    void setTimeMid(double t, const std::string& unit);
	/** Returns the mid-term lookahead time [s] */
    double getTimeMid(const std::string& unit) const;
    
	/**
	 * Interface method that behaves the same as setTimeNear().
	 */
    void setLookaheadTime(double t, const std::string& unit);
	/**
	 * Interface method that behaves the same as getTimeNear().
	 */
    double getLookaheadTime(const std::string& unit) const;

    /**
     * Interface method that sets minimum and maximum time range for near bands (only)
     */
    void setTimeRange(double b, double t, const std::string& tunit);
       /**
        * Interface method that get minimum time for near bands
        */
    double getStartTime(const std::string& unit) const;

    //
    // Interface Methods
    //
    
    bool isLatLon() const;
    
    void setDistance(double d, const std::string& unit);
    double getDistance(const std::string& unit) const;
    
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
    void setOwnship(const std::string& id, const Position& s, const Velocity& v);

    void setOwnshipLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit);

    void setOwnshipXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit);

      //  SPECIAL: in internal units
    void addTraffic(const std::string& id, const Position& p, const Velocity& v);

    void addTrafficLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit);

    void addTrafficXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit);
    
    
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
    BandsCore red; 
    BandsCore amber;
    
    bool ownship;
    bool needCompute;
    
    Position so;
    Velocity vo;
    
    mutable ErrorLog error;
    
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
    
    mutable IntervalSet tmp_amber; // for performance
    
    
    void init(double D, const std::string& dunit, double H, const std::string& hunit, double Tnear, double Tmid, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit);
    
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
                  const IntervalSet& red, const IntervalSet& amber);
    
    
    void add(Interval arr[], int size, int i, const Interval& r);
    void add(BandsRegion::Region arr[], int size, int i, BandsRegion::Region color);
    
    int order(Interval arr[], int size, const Interval& n);

  };
  

}
#endif
