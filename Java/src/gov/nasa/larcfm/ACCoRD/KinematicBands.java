/* 
 * Kinematic Bands
 *
 * Contact: Cesar Munoz
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.ACCoRD.CDCylinder;

/**
 * Objects of class "KinematicMultiBands" compute the conflict bands using 
 * kinematic single-maneuver projections of the ownship and linear preditions 
 * of (multiple) traffic aircraft positions. The bands consist of ranges of 
 * guidance maneuvers: track angles, ground speeds, vertical
 * speeds, and altitude. <p> 
 *
 * An assumption of the bands information is that the traffic aircraft
 * do not maneuver.  If the ownship immediately executes a NONE
 * guidance maneuver, then the new path is conflict free (within a
 * lookahead time of the parameter).  If the ownship immediately executes a
 * NEAR guidance maneuver and no traffic aircraft maneuvers, then
 * there will be a loss of separation within the lookahead time.<p>
 *
 * If recovery bands are set via setRecoveryBands() and the ownship is in
 * a violation path, loss of separation recovery bands and recovery times are
 * computed for each type of maneuver. If the ownship immediately executes a 
 * RECOVERY guidance maneuver, then the new path is conflict-free after the
 * recovery time. Furthermore, the recovery time is the minimum time for which 
 * there exists a kinematic conflict-free maneuver in the future. <p>
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
 * Disclaimers: The formal proofs of the core algorithms use real numbers,
 * however these implementations use floating point
 * numbers, so numerical differences could result. In addition, the
 * geodetic computations include certain inaccuracies, especially near
 * the poles.<p>
 *
 * The basic usage is
 * <pre>
 * KinematicBands b(..with configuration parameters..);
 * b.clear();
 * b.setOwnship(position of ownship, velocity of ownship);
 * b.addTraffic(position of (one) traffic aircraft, velocity of traffic);
 * b.addTraffic(position of (another) traffic aircraft, velocity of traffic);
 * ...add other traffic aircraft...
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
 *
 * When any "parameter" to this class is set (separation distance,
 * lookahead time, positions in latitude/longitude, etc.), all
 * previous bands information is cleared, though unlike 
 * instantaneous-maneuver bands, ownship and traffic state data is 
 * preserved in KineamaticBands unless explicitly cleared.  
 * For more complete example usage see the file <tt>Batch.java</tt>.  <p>
 *
 * Kinematic bands also have a set of "step size" parameters that determine 
 * the granularity of the search.  TrackStep indicates the maximum track 
 * resolution, GroundSpeedStep for ground speed, and VerticalSpeedStep for 
 * vertical speed.  These determine the output granularity, for 
 * example, if TrackStep is set to 1 degree, bands will be in 1 degree 
 * increments.<p>
 *
 * If the detection calculations discover a maneuver will cause the ownship to
 * enter an intruder's protected zone (e.g. a loss of separation, RA), then
 * all further maneuvers in that direction will be marked with a "near" band.
 * An example of this is if the ownship is moving with a track of 20 deg, and
 * a turn to the right would cause a loss of separation at the 50 deg point, 
 * then the "near" bands for track will at least contain the range of approximately 
 * 50-200 degrees (current track +180 deg, +/- the trackStep).<p>
 * 
 * Note that Bands outputs hold within a given (constant) frame of reference, with the default being
 * against a stationary Earth (i.e. GPS coordinates and velocities).  This means that
 * if bands are instead given wind-oriented input data (airspeed and  yaw-heading) for 
 * all aircraft, then the output maneuvers will be in the same frame of reference:
 * "groundSpeed" bands should then be read as "airSpeed" bands, and "track" should be
 * read as "heading".<p>
 *
 * Altitude bands assume assume an immediate maneuver to a given vertical speed, 
 * with a level off maneuver to various altitudes (based on altitudeStep, for example, 
 * every 500 ft). "NONE" bands here indicate no conflict during or immediately following 
 * such a maneuver, assuming all traffic aircraft continue at a constant velocity.  "NEAR"
 * bands indicate either a loss of separation during the climb/descent or a conflict after leveling 
 * off to that altitude.
 *
 */
public class KinematicBands extends KinematicMultiBands {

	/** 
	 * Construct a KinematicBands with default parameters. The initial state detector  
	 * is specified by the parameters.
	 * @param detector        State detector
	 */
	public KinematicBands(Detection3D detector) {
		setAlertor(AlertLevels.SingleBands(detector, 
				core_.parameters.getLookaheadTime(),
        core_.parameters.getLookaheadTime()));
	}

	/** 
	 * Construct a KinematicBands object with default values for
	 * configuration parameters. The default state detector is cylinder.  
	 */
	public KinematicBands() {
		this(new CDCylinder());
	}

	/**
	 * Construct a KinematicBands object from an existing KinematicBands object. This copies all traffic data.
	 */
	public KinematicBands(KinematicBands b) {
		super(b);
	}

	/** 
	 * Sets lookahead time in seconds. This is the time horizon used to compute bands.
	 */ 
	public void setLookaheadTime(double t) {
		int level = core_.parameters.alertor.conflictAlertLevel();
		if (level > 0) {
			super.setLookaheadTime(t);
			AlertThresholds athr = core_.parameters.alertor.getLevel(level);
			athr.setEarlyAlertingTime(t);
			core_.parameters.alertor.clear();
			core_.parameters.alertor.addLevel(athr);
			core_.parameters.alertor.setConflictAlertLevel(1);
			reset();
		}
	}

	/**
	 * @return alerting time in seconds. This is the first time prior to a violation when bands are
	 * computed.
	 */
	public double getAlertingTime() {
		int level = core_.parameters.alertor.conflictAlertLevel();
		if (level > 0) {
			return core_.parameters.alertor.getLevel(level).getAlertingTime();
		}
		return Double.NaN;
	}

	/**
	 * Sets alerting time in seconds. This is the first time prior to a violation when bands are
	 * computed. 
	 */
	public void setAlertingTime(double t) {
		int level = core_.parameters.alertor.conflictAlertLevel();
		if (level > 0) {
			AlertThresholds athr = core_.parameters.alertor.getLevel(level);
			athr.setAlertingTime(t);
			core_.parameters.alertor.clear();
			core_.parameters.alertor.addLevel(athr);
			core_.parameters.alertor.setConflictAlertLevel(1);
			reset();
		}
	}

	/** Experimental. Set the underlying Detection3D object that will be used to determine conflicts.
	 * This will also clear any results (but not traffic info).
	 */
	public void setCoreDetection(Detection3D detector) {
		int level = core_.parameters.alertor.conflictAlertLevel();
		if (level > 0) {
			AlertThresholds athr = core_.parameters.alertor.getLevel(level);
			athr.setDetector(detector);
			core_.parameters.alertor.clear();
			core_.parameters.alertor.addLevel(athr);
			core_.parameters.alertor.setConflictAlertLevel(1);
			reset();
		}
	}


}
