/* 
 * Kinematic Multi Bands
 *
 * Contact: Cesar Munoz
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.util.List;

/**
 * Objects of class "KinematicMultiBands" compute the conflict bands using 
 * kinematic single-maneuver projections of the ownship and linear preditions 
 * of (multiple) traffic aircraft positions. The bands consist of ranges of 
 * guidance maneuvers: track angles, ground speeds, vertical
 * speeds, and altitude.<p> 
 * 
 * An assumption of the bands information is that the traffic aircraft
 * do not maneuver. If the ownship immediately executes a NONE
 * guidance maneuver, then the new path is conflict free (within a
 * lookahead time of the parameter).  If the ownship immediately executes a
 * NEAR/MID/FAR guidance maneuver and no traffic aircraft maneuvers, then
 * there will corresponding alert within the corresponding alerting level thresholds.<p>
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
 * KinematicMultiBands b = new KinematicMultiBands();
 * ...
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
public class KinematicMultiBands implements GenericStateBands {

	public KinematicBandsCore core_;
	public KinematicTrkBands  trk_band_; 
	public KinematicGsBands   gs_band_;  
	public KinematicVsBands   vs_band_;  
	public KinematicAltBands  alt_band_; 

	protected ErrorLog error = new ErrorLog("KinematicMultiBands");

	/** 
	 * Construct a KinematicMultiBands object with initial parameters and an empty list of detectors. 
	 */
	public KinematicMultiBands(KinematicBandsParameters parameters) {
		core_ = new KinematicBandsCore(parameters);
		trk_band_ = new KinematicTrkBands(parameters);
		gs_band_ = new KinematicGsBands(parameters);
		vs_band_ = new KinematicVsBands(parameters);
		alt_band_ = new KinematicAltBands(parameters);
	}

	/** 
	 * Construct a KinematicMultiBands object with the default parameters and an empty list of detectors. 
	 */
	public KinematicMultiBands() {
		this(new KinematicBandsParameters());
	}

	/**
	 * Construct a KinematicMultiBands object from an existing object. This copies all traffic data.
	 */
	public KinematicMultiBands(KinematicMultiBands b) {
		core_ = new KinematicBandsCore(b.core_);
		trk_band_ = new KinematicTrkBands(b.trk_band_);
		gs_band_ = new KinematicGsBands(b.gs_band_);
		vs_band_ = new KinematicVsBands(b.vs_band_);
		alt_band_ = new KinematicAltBands(b.alt_band_);
	}

	/** Ownship and Traffic **/

	public TrafficState getOwnship() {
		return core_.ownship;
	}

	// This function clears the traffic
	public void setOwnship(TrafficState own) {
		clear();
		core_.ownship = own;
	}

	public void setOwnship(String id, Position p, Velocity v) {
		setOwnship(TrafficState.makeOwnship(id,p,v));
	}

	public void setOwnship(Position p, Velocity v) {
		setOwnship("Ownship",p,v);
	}

	public void setTraffic(List<TrafficState> traffic) {
		core_.traffic.clear();
		core_.traffic.addAll(traffic);
		reset();    
	}

	public List<TrafficState> getTraffic() {
		return core_.traffic;
	}

	public boolean hasOwnship() {
		return core_.hasOwnship();
	}

	public boolean hasTraffic() {
		return core_.hasTraffic();
	}

	public void addTraffic(TrafficState ac) {
		if (!ac.isValid()) {
			error.addError("addTraffic: invalid aircraft.");
			return;
		} else if (!hasOwnship()) {
			error.addError("addTraffic: setOwnship must be called before addTraffic.");
			return;
		} else if (ac.isLatLon() != isLatLon()) {
			error.addError("addTraffic: inconsistent use of lat/lon and Euclidean data.");
			return;
		}
		core_.traffic.add(ac);
		reset();
	}

	public void addTraffic(String id, Position pi, Velocity vi) {
		if (!hasOwnship()) {
			error.addError("addTraffic: setOwnship must be called before addTraffic.");
			return;
		}
		addTraffic(core_.ownship.makeIntruder(id,pi,vi));
	}

	public void addTraffic(Position pi, Velocity vi) {
		addTraffic("AC_"+(core_.traffic.size()+1),pi,vi);
	}

	/* General Settings */

	/**
	 * Set alert thresholds
	 */
	public void setAlertor(AlertLevels alertor) {
		core_.parameters.alertor.copy(alertor);
		reset();
	}

	/**
	 * Set bands parameters
	 */
	public void setKinematicBandsParameters(KinematicBandsParameters parameters) {
		core_.parameters.setKinematicBandsParameters(parameters);

		// Set Track Bands
		trk_band_.set_step(parameters.getTrackStep());  
		trk_band_.set_turn_rate(parameters.getTurnRate()); 
		trk_band_.set_bank_angle(parameters.getBankAngle()); 
		trk_band_.set_recovery(parameters.isEnabledRecoveryTrackBands());
		trk_band_.set_step(parameters.getTrackStep());
		setLeftTrack(parameters.getLeftTrack());
		setRightTrack(parameters.getRightTrack());

		// Set Ground Speed Bands
		gs_band_.set_step(parameters.getGroundSpeedStep());
		gs_band_.set_horizontal_accel(parameters.getHorizontalAcceleration()); 
		gs_band_.set_recovery(parameters.isEnabledRecoveryGroundSpeedBands());
		gs_band_.set_step(parameters.getGroundSpeedStep());
		setMinGroundSpeed(parameters.getMinGroundSpeed());
		setMaxGroundSpeed(parameters.getMaxGroundSpeed());

		// Set Vertical Speed Bands
		vs_band_.set_step(parameters.getVerticalSpeedStep()); 
		vs_band_.set_vertical_accel(parameters.getVerticalAcceleration());
		vs_band_.set_recovery(parameters.isEnabledRecoveryVerticalSpeedBands());   
		vs_band_.set_step(parameters.getVerticalSpeedStep());
		setMinVerticalSpeed(parameters.getMinVerticalSpeed());
		setMaxVerticalSpeed(parameters.getMaxVerticalSpeed());

		// Set Altitude Bands
		alt_band_.set_step(parameters.getAltitudeStep()); 
		alt_band_.set_vertical_rate(parameters.getVerticalRate()); 
		alt_band_.set_vertical_accel(parameters.getVerticalAcceleration());
		alt_band_.set_recovery(parameters.isEnabledRecoveryAltitudeBands());   
		alt_band_.set_step(parameters.getAltitudeStep());
		setMinAltitude(parameters.getMinAltitude());
		setMaxAltitude(parameters.getMaxAltitude());

		reset();
	}

	/**
	 * @return recovery stability time in seconds. Recovery bands are computed at time of first green plus
	 * this time.
	 */
	public double getRecoveryStabilityTime() {
		return core_.parameters.getRecoveryStabilityTime();
	}

	/**
	 * @return recovery stability time in specified units. Recovery bands are computed at time of first green plus
	 * this time.
	 */
	public double getRecoveryStabilityTime(String u) {
		return core_.parameters.getRecoveryStabilityTime(u);
	}

	/**
	 * Sets recovery stability time in seconds. Recovery bands are computed at time of first green plus
	 * this time.
	 */
	public void setRecoveryStabilityTime(double t) {
		core_.parameters.setRecoveryStabilityTime(t);
		reset();
	}

	/**
	 * Sets recovery stability time in specified units. Recovery bands are computed at time of first green plus
	 * this time.
	 */
	public void setRecoveryStabilityTime(double t, String u) {
		core_.parameters.setRecoveryStabilityTime(t,u);
		reset();
	}

	/**
	 * @return minimum horizontal separation for recovery bands in internal units [m].
	 */
	public double getMinHorizontalRecovery() {
		return core_.parameters.getMinHorizontalRecovery();
	}

	/** 
	 * Return minimum horizontal separation for recovery bands in specified units [u]
	 */
	public double getMinHorizontalRecovery(String u) {
		return core_.parameters.getMinHorizontalRecovery(u);
	}

	/** 
	 * Sets minimum horizontal separation for recovery bands in internal units [m].
	 */
	public void setMinHorizontalRecovery(double val) {
		core_.parameters.setMinHorizontalRecovery(val);
		reset();
	}

	/** 
	 * Set minimum horizontal separation for recovery bands in specified units [u].
	 */
	public void setMinHorizontalRecovery(double val, String u) {
		core_.parameters.setMinHorizontalRecovery(val,u);
		reset();
	}

	/** 
	 * @return minimum vertical separation for recovery bands in internal units [m].
	 */
	public double getMinVerticalRecovery() {
		return core_.parameters.getMinVerticalRecovery();
	}

	/** 
	 * Return minimum vertical separation for recovery bands in specified units [u].
	 */
	public double getMinVerticalRecovery(String u) {
		return core_.parameters.getMinVerticalRecovery(u);
	}

	/**
	 * Sets minimum vertical separation for recovery bands in internal units [m].
	 */
	public void setMinVerticalRecovery(double val) {
		core_.parameters.setMinVerticalRecovery(val);
		reset();
	}

	/** 
	 * Set minimum vertical separation for recovery bands in units
	 */
	public void setMinVerticalRecovery(double val, String u) {
		core_.parameters.setMinVerticalRecovery(val,u);
		reset();
	}

	/** 
	 * @return true if collision avoidance bands are enabled.
	 */
	public boolean isEnabledCollisionAvoidanceBands() {
		return core_.parameters.isEnabledCollisionAvoidanceBands();
	}

	/** 
	 * Enable/disable collision avoidance bands.
	 */ 
	public void setCollisionAvoidanceBands(boolean flag) {
		core_.parameters.setCollisionAvoidanceBands(flag);
		reset();
	}

	/** 
	 * Enable collision avoidance bands.
	 */ 
	public void enableCollisionAvoidanceBands() {
		setCollisionAvoidanceBands(true);
	}

	/** 
	 * Disable collision avoidance bands.
	 */ 
	public void disableCollisionAvoidanceBands() {
		setCollisionAvoidanceBands(false);
	}

	/** 
	 * @return get factor for computing collision avoidance bands. Factor value is in (0,1]
	 */
	public double getCollisionAvoidanceBandsFactor() {
		return core_.parameters.getCollisionAvoidanceBandsFactor();
	}

	/** 
	 * @return set factor for computing collision avoidance bands. Factor value is in (0,1]
	 */
	public void setCollisionAvoidanceBandsFactor(double val) {
		core_.parameters.setCollisionAvoidanceBandsFactor(val);
		reset();
	}

	/**
	 * @return most urgent aircraft.
	 */
	public TrafficState getMostUrgentAircraft() {
		return core_.most_urgent_ac;
	}

	/**
	 * Set most urgent aircraft.
	 */
	public void setMostUrgentAircraft(TrafficState ac) {
		core_.most_urgent_ac = ac;
		reset();
	}

	/**
	 * Set most urgent aircraft by identifier.
	 */
	public void setMostUrgentAircraft(String id) {
		core_.most_urgent_ac = core_.intruder(id);
		reset();
	}

	/** 
	 * @return true if repulsive criteria is enabled for conflict bands.
	 */
	public boolean isEnabledConflictCriteria() {
		return core_.parameters.isEnabledConflictCriteria();
	}

	/** 
	 * Enable/disable repulsive criteria for conflict bands.
	 */
	public void setConflictCriteria(boolean flag) {
		core_.parameters.setConflictCriteria(flag);
		reset();
	}

	/** 
	 * Enable repulsive criteria for conflict bands.
	 */
	public void enableConflictCriteria() {
		setConflictCriteria(true);
	}

	/** 
	 * Disable repulsive criteria for conflict bands.
	 */
	public void disableConflictCriteria() {
		setConflictCriteria(false);
	}

	/** 
	 * @return true if repulsive criteria is enabled for recovery bands.
	 */
	public boolean isEnabledRecoveryCriteria() {
		return core_.parameters.isEnabledRecoveryCriteria();
	}

	/** 
	 * Enable/disable repulsive criteria for recovery bands.
	 */
	public void setRecoveryCriteria(boolean flag) {
		core_.parameters.setRecoveryCriteria(flag);
		reset();
	}

	/** 
	 * Enable repulsive criteria for recovery bands.
	 */
	public void enableRecoveryCriteria() {
		setRecoveryCriteria(true);
	}

	/** 
	 * Disable repulsive criteria for recovery bands.
	 */
	public void disableRecoveryCriteria() {
		setRecoveryCriteria(false);
	}

	/** 
	 * Enable/disable repulsive criteria for conflict and recovery bands.
	 */
	public void setRepulsiveCriteria(boolean flag) {
		setConflictCriteria(flag);
		setRecoveryCriteria(flag);
	}

	/** 
	 * Enable repulsive criteria for conflict and recovery bands.
	 */
	public void enableRepulsiveCriteria() {
		setRepulsiveCriteria(true);
	}

	/** 
	 * Disable repulsive criteria for conflict and recovery bands.
	 */
	public void disableRepulsiveCriteria() {
		setRepulsiveCriteria(false);
	}

	/** 
	 * Sets recovery bands flag for track, ground speed, and vertical speed bands to specified value.
	 */ 
	public void setRecoveryBands(boolean flag) {
		trk_band_.set_recovery(flag);
		gs_band_.set_recovery(flag);
		vs_band_.set_recovery(flag);
		alt_band_.set_recovery(flag);
		core_.parameters.setRecoveryTrackBands(flag);
		core_.parameters.setRecoveryGroundSpeedBands(flag);
		core_.parameters.setRecoveryVerticalSpeedBands(flag);
		core_.parameters.setRecoveryAltitudeBands(flag);
		reset();
	}

	/** 
	 * Enables recovery bands for track, ground speed, and vertical speed.
	 */ 
	public void enableRecoveryBands() {
		setRecoveryBands(true);
	}

	/** 
	 * Disable recovery bands for track, ground speed, and vertical speed.
	 */ 
	public void disableRecoveryBands() {
		setRecoveryBands(false);
	}

	/* Track Bands Settings */

	/** 
	 * @return left track in radians [0 - pi] [rad] from current ownship's track
	 */
	public double getLeftTrack() {
		if (trk_band_.get_rel()) {
			return trk_band_.get_min();
		} else if (trk_band_.check_input(core_)) {
			return -trk_band_.min_rel(core_.ownship);
		} else {
			return Double.NaN;
		}
	}

	/** 
	 * @return left track in specified units [0 - pi] [u] from current ownship's track
	 */
	public double getLeftTrack(String u) {
		return Units.to(u,getLeftTrack());
	}

	/** 
	 * @return right track in radians [0 - pi] [rad] from current ownship's track
	 */
	public double getRightTrack() {
		if (trk_band_.get_rel()) {
			return trk_band_.get_max();
		} else if (trk_band_.check_input(core_)) {
			return trk_band_.max_rel(core_.ownship);
		} else {
			return Double.NaN;
		}
	}

	/** 
	 * @return right track in specified units [0 - pi] [u] from current ownship's track
	 */
	public double getRightTrack(String u) {
		return Units.to(u,getRightTrack());
	}

	/** 
	 * Set left track to value in internal units [rad]. Value is expected to be in [0 - pi]
	 */
	public void setLeftTrack(double val) {
		val = -Math.abs(Util.to_pi(val));
		if (!trk_band_.get_rel()) {
			trk_band_.set_rel(true);
		}
		trk_band_.set_min(val);
		reset(); 
	}

	/** 
	 * Set left track to value in specified units [u]. Value is expected to be in [0 - pi]
	 */
	public void setLeftTrack(double val, String u) {
		setLeftTrack(Units.from(u,val));
	}

	/** 
	 * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
	 */
	public void setRightTrack(double val) {
		val = Math.abs(Util.to_pi(val));
		if (!trk_band_.get_rel()) {
			trk_band_.set_rel(true);
		}
		trk_band_.set_max(val);
		reset();
	}

	/** 
	 * Set right track to value in specified units [u]. Value is expected to be in [0 - pi]
	 */
	public void setRightTrack(double val, String u) {
		setRightTrack(Units.from(u,val));
	}

	/** 
	 * Set absolute min/max tracks for bands computations. Tracks are specified in internal units [rad].
	 * Values are expected to be in [0 - 2pi]
	 */
	public void setMinMaxTrack(double min, double max) {
		min = Util.to_2pi(min);
		max = Util.to_2pi(max);
		if (max == 0) {
			max = 2*Math.PI;
		}
		if (trk_band_.get_rel()) {
			trk_band_.set_rel(false);
		}
		trk_band_.set_min(min);
		trk_band_.set_max(max);
		reset();
	}

	/** 
	 * Set absolute min/max tracks for bands computations. Tracks are specified in given units [u].
	 * Values are expected to be in [0 - 2pi] [u]
	 */
	public void setMinMaxTrack(double min, double max, String u) {
		setMinMaxTrack(Units.from(u,min),Units.from(u,max));
	}

	/** 
	 * @return step size for track bands in internal units [rad]. 
	 */
	public double getTrackStep() {
		return core_.parameters.getTrackStep();
	}

	/** 
	 * @return step size for track bands in specified units [u]. 
	 */
	public double getTrackStep(String u) {
		return core_.parameters.getTrackStep(u);
	}

	/** 
	 * Sets step size for track bands in internal units [rad].
	 */
	public void setTrackStep(double val) {
		core_.parameters.setTrackStep(val);
		trk_band_.set_step(core_.parameters.getTrackStep());
		reset();
	}

	/** 
	 * Sets step size for track bands in specified units [u].
	 */
	public void setTrackStep(double val, String u) {
		core_.parameters.setTrackStep(val,u);
		trk_band_.set_step(core_.parameters.getTrackStep());
		reset();
	}

	/** 
	 * @return bank angle in internal units [rad].
	 */
	public double getBankAngle() {
		return core_.parameters.getBankAngle();
	}

	/** 
	 * @return bank angle in specified units [u].
	 */
	public double getBankAngle(String u) {
		return core_.parameters.getBankAngle(u);
	}

	/** 
	 * Sets bank angle for track bands to value in internal units [rad]. As a side effect, this method
	 * resets the turn rate.
	 */
	public void setBankAngle(double val) {
		core_.parameters.setBankAngle(val);
		trk_band_.set_bank_angle(core_.parameters.getBankAngle());
		trk_band_.set_turn_rate(0);
		reset();
	}

	/** 
	 * Sets bank angle for track bands to value in specified units [u]. As a side effect, this method
	 * resets the turn rate.
	 */
	public void setBankAngle(double val, String u) {
		core_.parameters.setBankAngle(val,u);
		trk_band_.set_bank_angle(core_.parameters.getBankAngle());
		trk_band_.set_turn_rate(0);
		reset();
	}

	/** 
	 * @return turn rate in internal units [rad/s].
	 */
	public double getTurnRate() {
		return core_.parameters.getTurnRate();
	}

	/** 
	 * @return turn rate in specified units [u].
	 */
	public double getTurnRate(String u) {
		return core_.parameters.getTurnRate(u);
	}

	/** 
	 * Sets turn rate for track bands to value in internal units [rad/s]. As a side effect, this method
	 * resets the bank angle.
	 */
	public void setTurnRate(double val) {
		core_.parameters.setTurnRate(val);
		trk_band_.set_turn_rate(core_.parameters.getTurnRate());
		trk_band_.set_bank_angle(0);
		reset();
	}

	/** 
	 * Sets turn rate for track bands to value in specified units [u]. As a side effect, this method
	 * resets the bank angle.
	 */
	public void setTurnRate(double val, String u) {
		core_.parameters.setTurnRate(val,u);
		trk_band_.set_turn_rate(core_.parameters.getTurnRate());
		trk_band_.set_bank_angle(0);
		reset();
	}

	/**
	 * @return true if recovery track bands are enabled.
	 */
	public boolean isEnabledRecoveryTrackBands() {
		return core_.parameters.isEnabledRecoveryTrackBands();
	}

	/** 
	 * Sets recovery bands flag for track bands to specified value.
	 */ 
	public void setRecoveryTrackBands(boolean flag) {
		trk_band_.set_recovery(flag);
		core_.parameters.setRecoveryTrackBands(flag);
		reset();
	}

	/* Ground Speed Bands Settings */

	/** 
	 * @return minimum ground speed for ground speed bands in internal units [m/s].
	 */
	public double getMinGroundSpeed() {
		if (gs_band_.get_rel()) {
			if (gs_band_.check_input(core_)) {
				return gs_band_.min_val(core_.ownship);
			} else {
				return Double.NaN;
			}
		} else {
			return gs_band_.get_min();
		}
	}

	/** 
	 * @return minimum ground speed for ground speed bands in specified units [u].
	 */
	public double getMinGroundSpeed(String u) {
		return Units.to(u,getMinGroundSpeed());
	}

	/** 
	 * Sets minimum ground speed for ground speed bands to value in internal units [m/s].
	 */
	public void setMinGroundSpeed(double val) {
		if (error.isNonNegative("setMinGroundSpeed",val)) {
			if (gs_band_.get_rel()) {
				gs_band_.set_rel(false);
			}
			gs_band_.set_min(val);
			reset();
		}
	}

	/** 
	 * Sets minimum ground speed for ground speed bands to value in specified units [u].
	 */
	public void setMinGroundSpeed(double val, String u) {
		setMinGroundSpeed(Units.from(u, val));
	}

	/** 
	 * @return maximum ground speed for ground speed bands in internal units [m/s].
	 */
	public double getMaxGroundSpeed() {
		if (gs_band_.get_rel()) {
			if (gs_band_.check_input(core_)) {
				return gs_band_.max_val(core_.ownship);
			} else {
				return Double.NaN;
			}
		} else {
			return gs_band_.get_max();
		}
	}

	/** 
	 * @return maximum ground speed for ground speed bands in specified units [u].
	 */
	public double getMaxGroundSpeed(String u) {
		return Units.to(u,getMaxGroundSpeed());
	}

	/** 
	 * Sets maximum ground speed for ground speed bands to value in internal units [m/s].
	 */
	public void setMaxGroundSpeed(double val) {
		if (error.isPositive("setMaxGroundSpeed",val)) {
			if (gs_band_.get_rel()) {
				gs_band_.set_rel(false);
			}
			gs_band_.set_max(val);
			reset();
		}
	}

	/** 
	 * Sets maximum ground speed for ground speed bands to value in specified units [u].
	 */
	public void setMaxGroundSpeed(double val, String u) {
		setMaxGroundSpeed(Units.from(u, val));
	}

	/** 
	 * Set below/above ground speed, relative to ownship's ground speed, for bands computations. 
	 * Ground speeds are specified in internal units [m/s]. Below and above are expected to be
	 * non-negative values.
	 */
	public void setBelowAboveGroundSpeed(double below, double above) {
		below = -Math.abs(below);
		above = Math.abs(above);
		if (!gs_band_.get_rel()) {
			gs_band_.set_rel(true);
		}
		gs_band_.set_min(below);
		gs_band_.set_max(above);
		reset();
	}     

	/** 
	 * Set below/above ground speed, relative to ownship's ground speed, for bands computations. 
	 * Ground speeds are specified in given units [u]. Below and above are expected to be
	 * non-negative values.
	 */
	public void setBelowAboveGroundSpeed(double below, double above, String u) {
		setBelowAboveGroundSpeed(Units.from(u,below),Units.from(u,above));
	}

	/** 
	 * @return step size for ground speed bands in internal units [m/s]. 
	 */
	public double getGroundSpeedStep() {
		return core_.parameters.getGroundSpeedStep();
	}

	/** 
	 * @return step size for ground speed bands in specified units [u]. 
	 */
	public double getGroundSpeedStep(String u) {
		return core_.parameters.getGroundSpeedStep(u);
	}

	/** 
	 * Sets step size for ground speed bands to value in internal units [m/s].
	 */
	public void setGroundSpeedStep(double val) {
		core_.parameters.setGroundSpeedStep(val);
		gs_band_.set_step(core_.parameters.getGroundSpeedStep());
		reset();
	}

	/** 
	 * Sets step size for ground speed bands to value in specified units [u].
	 */
	public void setGroundSpeedStep(double val, String u) {
		core_.parameters.setGroundSpeedStep(val,u);
		gs_band_.set_step(core_.parameters.getGroundSpeedStep());
		reset();
	}

	/** 
	 * @return horizontal acceleration for ground speed bands to value in internal units [m/s^2]. 
	 */
	public double getHorizontalAcceleration() {
		return core_.parameters.getHorizontalAcceleration();
	}

	/** 
	 * @return horizontal acceleration for ground speed bands to value in specified units [u]. 
	 */
	public double getHorizontalAcceleration(String u) {
		return core_.parameters.getHorizontalAcceleration(u);
	}

	/** 
	 * Sets horizontal acceleration for ground speed bands to value in internal units [m/s^2].
	 */
	public void setHorizontalAcceleration(double val) {
		core_.parameters.setHorizontalAcceleration(val);
		gs_band_.set_horizontal_accel(core_.parameters.getHorizontalAcceleration());
		reset();
	}

	/** 
	 * Sets horizontal acceleration for ground speed bands to value in specified units [u].
	 */
	public void setHorizontalAcceleration(double val, String u) {
		core_.parameters.setHorizontalAcceleration(val,u);
		gs_band_.set_horizontal_accel(core_.parameters.getHorizontalAcceleration());
		reset();
	}

	/**
	 * @return true if recovery ground speed bands are enabled.
	 */
	public boolean isEnabledRecoveryGroundSpeedBands() {
		return core_.parameters.isEnabledRecoveryGroundSpeedBands();
	}

	/** 
	 * Sets recovery bands flag for ground speed bands to specified value.
	 */ 
	public void setRecoveryGroundSpeedBands(boolean flag) {
		gs_band_.set_recovery(flag);
		core_.parameters.setRecoveryGroundSpeedBands(flag);
		reset();
	}

	/* Vertical Speed Bands Settings */

	/** 
	 * @return minimum vertical speed for vertical speed bands in internal units [m/s].
	 */
	public double getMinVerticalSpeed() {
		if (vs_band_.get_rel()) {
			if (vs_band_.check_input(core_)) {
				return vs_band_.min_val(core_.ownship);
			} else {
				return Double.NaN;
			}
		} else {
			return vs_band_.get_min();
		}
	}

	/** 
	 * @return minimum vertical speed for vertical speed bands in specified units [u].
	 */
	public double getMinVerticalSpeed(String u) {
		return Units.to(u,getMinVerticalSpeed());
	}

	/** 
	 * Sets minimum vertical speed for vertical speed bands to value in internal units [m/s].
	 */
	public void setMinVerticalSpeed(double val) {
		if (vs_band_.get_rel()) {
			vs_band_.set_rel(false);
		}
		vs_band_.set_min(val);
		reset();
	}

	/** 
	 * Sets minimum vertical speed for vertical speed bands to value in specified units [u].
	 */
	public void setMinVerticalSpeed(double val, String u) {
		setMinVerticalSpeed(Units.from(u, val));
	}

	/** 
	 * @return maximum vertical speed for vertical speed bands in internal units [m/s].
	 */
	public double getMaxVerticalSpeed() {
		if (vs_band_.get_rel()) {
			if (vs_band_.check_input(core_)) {
				return vs_band_.max_val(core_.ownship);
			} else {
				return Double.NaN;
			}
		} else {
			return vs_band_.get_max();
		}
	}

	/** 
	 * @return maximum vertical speed for vertical speed bands in specified units [u].
	 */
	public double getMaxVerticalSpeed(String u) {
		return Units.to(u,getMaxVerticalSpeed());
	}

	/** 
	 * Sets maximum vertical speed for vertical speed bands to value in internal units [m/s].
	 */
	public void setMaxVerticalSpeed(double val) {
		if (vs_band_.get_rel()) {
			vs_band_.set_rel(false);
		}
		vs_band_.set_max(val);
		reset();
	}

	/** 
	 * Sets maximum vertical speed for vertical speed bands to value in specified units [u].
	 */
	public void setMaxVerticalSpeed(double val, String u) {
		setMaxVerticalSpeed(Units.from(u, val));
	}

	/** 
	 * Set below/above vertical speed, relative to ownship's vertical speed, for bands computations. 
	 * Vertical speeds are specified in internal units [m/s]. Below and above are expected to be
	 * non-negative values.
	 */
	public void setBelowAboveVerticalSpeed(double below, double above) {
		below = -Math.abs(below);
		above = Math.abs(above);
		if (!vs_band_.get_rel()) {
			vs_band_.set_rel(true);
		}
		vs_band_.set_min(below);
		vs_band_.set_max(above);
		reset();
	}

	/** 
	 * Set below/above vertical speed, relative to ownship's vertical speed, for bands computations. 
	 * Vertical speeds are specified in given units [u].Below and above are expected to be
	 * non-negative values.
	 */
	public void setBelowAboveVerticalSpeed(double below, double above, String u) {
		setBelowAboveVerticalSpeed(Units.from(u,below),Units.from(u,above));
	}

	/** 
	 * @return step size for vertical speed bands in internal units [m/s].
	 */
	public double getVerticalSpeedStep() {
		return core_.parameters.getVerticalSpeedStep();
	}

	/** 
	 * @return step size for vertical speed bands in specified units [u].
	 */
	public double getVerticalSpeedStep(String u) {
		return core_.parameters.getVerticalSpeedStep(u);
	}

	/** 
	 * Sets step size for vertical speed bands to value in internal units [m/s].
	 */
	public void setVerticalSpeedStep(double val) {
		core_.parameters.setVerticalSpeedStep(val);
		vs_band_.set_step(core_.parameters.getVerticalSpeedStep());
		reset();
	}

	/** 
	 * Sets step size for vertical speed bands to value in specified units [u].
	 */
	public void setVerticalSpeedStep(double val, String u) {
		core_.parameters.setVerticalSpeedStep(val,u);
		vs_band_.set_step(core_.parameters.getVerticalSpeedStep());
		reset();
	}

	/** 
	 * @return constant vertical acceleration for vertical speed and altitude bands in internal [m/s^2]
	 * units
	 */
	public double getVerticalAcceleration() {
		return core_.parameters.getVerticalAcceleration();
	}

	/** 
	 * @return constant vertical acceleration for vertical speed and altitude bands in specified
	 * units
	 */
	public double getVerticalAcceleration(String u) {
		return core_.parameters.getVerticalAcceleration(u);
	}

	/** 
	 * Sets the constant vertical acceleration for vertical speed and altitude bands
	 * to value in internal units [m/s^2]
	 */
	public void setVerticalAcceleration(double val) {
		core_.parameters.setVerticalAcceleration(val);
		vs_band_.set_vertical_accel(core_.parameters.getVerticalAcceleration());
		alt_band_.set_vertical_accel(core_.parameters.getVerticalAcceleration());
		reset();
	}

	/** 
	 * Sets the constant vertical acceleration for vertical speed and altitude bands
	 * to value in specified units [u].
	 */
	public void setVerticalAcceleration(double val, String u) {
		core_.parameters.setVerticalAcceleration(val,u);
		vs_band_.set_vertical_accel(core_.parameters.getVerticalAcceleration());
		alt_band_.set_vertical_accel(core_.parameters.getVerticalAcceleration());
		reset();
	}

	/**
	 * @return true if recovery vertical speed bands are enabled.
	 */
	public boolean isEnabledRecoveryVerticalSpeedBands() {
		return core_.parameters.isEnabledRecoveryVerticalSpeedBands();
	}

	/** 
	 * Sets recovery bands flag for vertical speed bands to specified value.
	 */ 
	public void setRecoveryVerticalSpeedBands(boolean flag) {
		vs_band_.set_recovery(flag);
		core_.parameters.setRecoveryVerticalSpeedBands(flag);
		reset();
	}

	/* Altitude Bands Settings */

	/** 
	 * @return minimum altitude for altitude bands in internal units [m]
	 */
	public double getMinAltitude() {
		if (alt_band_.get_rel()) {
			if (alt_band_.check_input(core_)) {
				return alt_band_.min_val(core_.ownship);
			} else {
				return Double.NaN;
			}
		} else {
			return alt_band_.get_min();
		}
	}

	/** 
	 * @return minimum altitude for altitude bands in specified units [u].
	 */
	public double getMinAltitude(String u) {
		return Units.to(u,getMinAltitude());
	}

	/** 
	 * Sets minimum altitude for altitude bands to value in internal units [m]
	 */
	public void setMinAltitude(double val) {
		if (error.isNonNegative("setMinAltitude",val)) {
			if (alt_band_.get_rel()) {
				alt_band_.set_rel(false);
			}
			alt_band_.set_min(val);
			reset();
		}
	}

	/** 
	 * Sets minimum altitude for altitude bands to value in specified units [u].
	 */
	public void setMinAltitude(double val, String u) {
		setMinAltitude(Units.from(u, val));
	}

	/** 
	 * @return maximum altitude for altitude bands in internal units [m]
	 */
	public double getMaxAltitude() {
		if (alt_band_.get_rel()) {
			if (alt_band_.check_input(core_)) {
				return alt_band_.max_val(core_.ownship);
			} else {
				return Double.NaN;
			}
		} else {
			return alt_band_.get_max();
		}
	}

	/** 
	 * @return maximum altitude for altitude bands in specified units [u].
	 */
	public double getMaxAltitude(String u) {
		return Units.to(u,getMaxAltitude());
	}

	/** 
	 * Sets maximum altitude for altitude bands to value in internal units [m]
	 */
	public void setMaxAltitude(double val) {
		if (error.isPositive("setMaxAltitude",val)) {
			if (alt_band_.get_rel()) {
				alt_band_.set_rel(false);
			}
			alt_band_.set_max(val);
			reset();
		}
	}

	/** 
	 * Sets maximum altitude for altitude bands to value in specified units [u].
	 */
	public void setMaxAltitude(double val, String u) {
		setMaxAltitude(Units.from(u, val));
	}

	/** 
	 * Set below/above altitude, relative to ownship's altitude, for bands computations. 
	 * Altitude are specified in internal units [m]. Below and above are expected to be
	 * non-negative values.
	 */
	public void setBelowAboveAltitude(double below, double above) {
		below = -Math.abs(below);
		above = Math.abs(above);
		if (!alt_band_.get_rel()) {
			alt_band_.set_rel(true);
		}
		alt_band_.set_min(below);
		alt_band_.set_max(above);
		reset();
	}     

	/** 
	 * Set below/above altitude, relative to ownship's altitude, for bands computations. 
	 * Altitudes are specified in given units [u]. Below and above are expected to be
	 * non-negative values.
	 */
	public void setBelowAboveAltitude(double below, double above, String u) {
		setBelowAboveAltitude(Units.from(u,below),Units.from(u,above));
	}

	/** 
	 * @return step size for altitude bands in internal units [m]
	 */
	public double getAltitudeStep() {
		return core_.parameters.getAltitudeStep();
	}

	/** 
	 * @return step size for altitude bands in specified units [u].
	 */
	public double getAltitudeStep(String u) {
		return core_.parameters.getAltitudeStep(u);
	}

	/** 
	 * Sets step size for altitude bands to value in internal units [m]
	 */
	public void setAltitudeStep(double val) {
		core_.parameters.setAltitudeStep(val);
		alt_band_.set_step(core_.parameters.getAltitudeStep());
		reset();
	}

	/** 
	 * Sets step size for altitude bands to value in specified units [u].
	 */
	public void setAltitudeStep(double val, String u) {
		core_.parameters.setAltitudeStep(val,u);
		alt_band_.set_step(core_.parameters.getAltitudeStep());
		reset();
	}

	/** 
	 * @return the vertical climb/descend rate for altitude bands in internal units [m/s]
	 */
	public double getVerticalRate() {
		return core_.parameters.getVerticalRate();
	}

	/** 
	 * @return the vertical climb/descend rate for altitude bands in specified units [u].
	 */
	public double getVerticalRate(String u) {
		return core_.parameters.getVerticalRate(u);
	}

	/** 
	 * Sets vertical rate for altitude bands to value in internal units [m/s]
	 */
	public void setVerticalRate(double val) {
		core_.parameters.setVerticalRate(val);
		alt_band_.set_vertical_rate(core_.parameters.getVerticalRate());
		reset();
	}

	/** 
	 * Sets vertical rate for altitude bands to value in specified units [u].
	 */
	public void setVerticalRate(double val, String u) {
		core_.parameters.setVerticalRate(val,u);
		alt_band_.set_vertical_rate(core_.parameters.getVerticalRate());
		reset();
	}

	/** 
	 * @return horizontal NMAC distance in internal units [m].
	 */
	public double getHorizontalNMAC() {
		return core_.parameters.getHorizontalNMAC();
	}

	/** 
	 * @return horizontal NMAC distance in specified units [u].
	 */
	public double getHorizontalNMAC(String u) {
		return core_.parameters.getHorizontalNMAC(u);
	}

	/** 
	 * @return vertical NMAC distance in internal units [m].
	 */
	public double getVerticalNMAC() {
		return core_.parameters.getVerticalNMAC();
	}

	/** 
	 * @return vertical NMAC distance in specified units [u].
	 */
	public double getVerticalNMAC(String u) {
		return core_.parameters.getVerticalNMAC(u);
	}

	/** 
	 * Set horizontal NMAC distance to value in internal units [m].
	 */
	public void setHorizontalNMAC(double val) {
		core_.parameters.setHorizontalNMAC(val);
		reset();
	}

	/** 
	 * Set horizontal NMAC distance to value in specified units [u].
	 */
	public void setHorizontalNMAC(double val, String u) {
		core_.parameters.setHorizontalNMAC(val,u);
		reset();
	}

	/** 
	 * Set vertical NMAC distance to value in internal units [m].
	 */
	public void setVerticalNMAC(double val) {
		core_.parameters.setVerticalNMAC(val);
		reset();
	}

	/** 
	 * Set vertical NMAC distance to value in specified units [u].
	 */
	public void setVerticalNMAC(double val, String u) {
		core_.parameters.setVerticalNMAC(val,u);
		reset();
	}

	/**
	 * @return true if recovery altitude bands are enabled.
	 */
	public boolean isEnabledRecoveryAltitudeBands() {
		return core_.parameters.isEnabledRecoveryAltitudeBands();
	}

	/** 
	 * Sets recovery bands flag for altitude bands to specified value.
	 */ 
	public void setRecoveryAltitudeBands(boolean flag) {
		alt_band_.set_recovery(flag);
		core_.parameters.setRecoveryAltitudeBands(flag);
		reset();
	}

	/* Utility methods */

	/**
	 *  Clear ownship and traffic data from this object.   
	 */
	public void clear() {
		core_.clear();
		reset();
	}

	public void reset() {
		core_.reset();
		trk_band_.reset();
		gs_band_.reset();
		vs_band_.reset();
		alt_band_.reset();
	}

	/* Main interface methods */

	/**
	 *  Return list of conflict aircraft for a given alert level.
	 *  Note: alert level are 1-indexed.
	 */
	public List<TrafficState> conflictAircraft(int alert_level) {
		return core_.conflictAircraft(alert_level);
	}

	/**
	 * Return time interval of violation for given alert level
	 * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
	 */
	public Interval timeIntervalOfViolation(int alert_level) {
		return core_.timeIntervalOfViolation(alert_level);
	}

	/** 
	 * @return the number of track band intervals, negative if the ownship has not been set
	 */
	public int trackLength() {
		return trk_band_.length(core_);
	}

	/** 
	 * Force computation of track bands. Usually, bands are only computed when needed. This method
	 * forces the computation of track bands (this method is included mainly for debugging purposes). 
	 */
	public void forceTrackBandsComputation() {
		trk_band_.force_compute(core_);
	}

	/**
	 * @return the i-th interval, in internal units [rad], of the computed track bands.
	 * @param i index
	 */
	public Interval track(int i) {
		return trk_band_.interval(core_,i);
	}

	/**
	 * @return the i-th interval, in specified units [u], of the computed track bands.
	 * @param i index
	 * @param u units
	 */
	public Interval track(int i, String u) {
		Interval ia = trk_band_.interval(core_,i);
		if (ia.isEmpty()) {
			return ia;
		}
		return new Interval(Units.to(u, ia.low), Units.to(u, ia.up));
	}

	/**
	 * @return the i-th region of the computed track bands.
	 * @param i index
	 */
	public BandsRegion trackRegion(int i) {
		return trk_band_.region(core_,i);
	}

	/**
	 * @return the range index of a given track specified in internal units [rad]
	 * @param trk [rad]
	 */
	public int trackRangeOf(double trk) {
		return trk_band_.rangeOf(core_,trk);
	}

	/**
	 * @return the range index of a given track specified in given units [u]
	 * @param trk [u]
	 * @param u Units
	 */
	public int trackRangeOf(double trk, String u) {
		return trackRangeOf(Units.from(u, trk));
	}

	/**
	 * @return the region of a given track specified in internal units [rad].
	 * @param trk [rad]
	 */
	public BandsRegion regionOfTrack(double trk) {
		return trackRegion(trackRangeOf(trk));
	}

	/**
	 * @return the region of a given track specified in given units [u]
	 * @param trk [u]
	 * @param u Units
	 */
	public BandsRegion regionOfTrack(double trk, String u) {
		return trackRegion(trackRangeOf(trk,u));
	}

	/**
	 * Return last time to track maneuver, in seconds, for ownship with respect to traffic
	 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within 
	 * lookahead time. Return negative infinity if there is no time to maneuver.
	 */
	public double lastTimeToTrackManeuver(TrafficState ac) {
		return trk_band_.last_time_to_maneuver(core_,ac);
	}

	/**
	 * @return time to recovery using track bands. Return NaN when bands are not saturated or when 
	 * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
	 */
	public double timeToTrackRecovery() {
		return trk_band_.timeToRecovery(core_);
	}

	/**
	 * @return list of aircraft responsible for peripheral track bands for a given alert level.
	 * Note: alert level are 1-indexed.
	 */
	public List<TrafficState> peripheralTrackAircraft(int alert_level) {
		return trk_band_.peripheralAircraft(core_,alert_level);
	}

	/**
	 * Compute track resolution maneuver for given alert level.
	 * @parameter dir is right (true)/left (false) of ownship current track
	 * @return track resolution in internal units [rad] in specified direction.
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no resolution to the right, and negative infinity if there 
	 * is no resolution to the left.
	 */
	public double trackResolution(boolean dir, int alert_level) {
		return trk_band_.compute_resolution(core_,alert_level,dir);
	}

	/**
	 * Compute track resolution maneuver for conflict alert level.
	 * @parameter dir is right (true)/left (false) of ownship current track
	 * @parameter u units
	 * @return track resolution in specified units [u] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no resolution to the right, and negative infinity if there 
	 * is no resolution to the left.
	 */
	public double trackResolution(boolean dir, int alert_level, String u) {
		return Units.to(u,trackResolution(dir,alert_level));
	}

	/**
	 * Compute track resolution maneuver for conflict alert level.
	 * @parameter dir is right (true)/left (false) of ownship current track
	 * @return track resolution in internal units [rad] in specified direction.
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no resolution to the right, and negative infinity if there 
	 * is no resolution to the left.
	 */
	public double trackResolution(boolean dir) {
		return trackResolution(dir,0);
	}

	/**
	 * Compute track resolution maneuver for conflict alert level.
	 * @parameter dir is right (true)/left (false) of ownship current track
	 * @parameter u units
	 * @return track resolution in specified units [u] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no resolution to the right, and negative infinity if there 
	 * is no resolution to the left.
	 */
	public double trackResolution(boolean dir, String u) {
		return Units.to(u,trackResolution(dir));
	}

	/**
	 * Compute preferred track direction, for given alert level, 
	 * based on resolution that is closer to current track.
	 * True: Right. False: Left.
	 */
	public boolean preferredTrackDirection(int alert_level) {
		return trk_band_.preferred_direction(core_, alert_level);
	}

	/**
	 * Compute preferred track direction, for conflict alert level, 
	 * based on resolution that is closer to current track.
	 * True: Right. False: Left.
	 */
	public boolean preferredTrackDirection() {
		return preferredTrackDirection(0);
	}

	/**
	 * @return the number of ground speed band intervals, negative if the ownship has not been set
	 */
	public int groundSpeedLength() {
		return gs_band_.length(core_);
	}

	/** 
	 * Force computation of ground speed bands. Usually, bands are only computed when needed. This method
	 * forces the computation of ground speed bands (this method is included mainly for debugging purposes). 
	 */
	public void forceGroundSpeedBandsComputation() {
		gs_band_.force_compute(core_);
	}

	/**
	 * @return the i-th interval, in internal units [m/s], of the computed ground speed bands.
	 * @param i index
	 */
	public Interval groundSpeed(int i) {
		return gs_band_.interval(core_,i);
	}

	/**
	 * @return the i-th interval, in specified units [u], of the computed ground speed bands.
	 * @param i index
	 * @param u units
	 */
	public Interval groundSpeed(int i, String u) {
		Interval ia = gs_band_.interval(core_,i);
		if (ia.isEmpty()) {
			return ia;
		}
		return new Interval(Units.to(u, ia.low), Units.to(u, ia.up));
	}

	/**
	 * @return the i-th region of the computed ground speed bands.
	 * @param i index
	 */
	public BandsRegion groundSpeedRegion(int i) {
		return gs_band_.region(core_,i);
	}

	/**
	 * @return the range index of a given ground speed specified in internal units [m/s]
	 * @param gs [m/s]
	 */
	public int groundSpeedRangeOf(double gs) {
		return gs_band_.rangeOf(core_,gs);
	}

	/**
	 * @return the range index of a given ground speed specified in given units [u]
	 * @param gs [u]
	 * @param u Units
	 */
	public int groundSpeedRangeOf(double gs, String u) {
		return groundSpeedRangeOf(Units.from(u,gs));
	}

	/**
	 * @return the region of a given ground speed specified in internal units [m/s]
	 * @param gs [m/s]
	 */
	public BandsRegion regionOfGroundSpeed(double gs) {
		return groundSpeedRegion(groundSpeedRangeOf(gs));
	}

	/**
	 * @return the region of a given ground speed specified in given units [u]
	 * @param gs [u]
	 * @param u Units
	 */
	public BandsRegion regionOfGroundSpeed(double gs, String u) {
		return groundSpeedRegion(groundSpeedRangeOf(gs,u));
	}

	/**
	 * Return last time to ground speed maneuver, in seconds, for ownship with respect to traffic
	 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within 
	 * lookahead time. Return negative infinity if there is no time to maneuver.
	 */
	public double lastTimeToGroundSpeedManeuver(TrafficState ac) {
		return gs_band_.last_time_to_maneuver(core_,ac);
	}

	/**
	 * @return time to recovery using ground speed bands.  Return NaN when bands are not saturated or when 
	 * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
	 */
	public double timeToGroundSpeedRecovery() {
		return gs_band_.timeToRecovery(core_);
	}

	/**
	 * @return list of aircraft responsible for peripheral ground speed bands for a given alert level.
	 * Note: alert level are 1-indexed.
	 */
	public List<TrafficState> peripheralGroundSpeedAircraft(int alert_level) {
		return gs_band_.peripheralAircraft(core_,alert_level);
	}

	/**
	 * Compute ground speed resolution maneuver for given alert level.
	 * @parameter dir is up (true)/down (false) of ownship current ground speed
	 * @return ground speed resolution in internal units [m/s] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double groundSpeedResolution(boolean dir, int alert_level) {
		return gs_band_.compute_resolution(core_,alert_level,dir);
	}

	/**
	 * Compute ground speed resolution maneuver for given alert level.
	 * @parameter dir is up (true)/down (false) of ownship current ground speed
	 * @parameter u units
	 * @return ground speed resolution in specified units [u] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double groundSpeedResolution(boolean dir, int alert_level, String u) {
		return Units.to(u,groundSpeedResolution(dir,alert_level));
	}

	/**
	 * Compute ground speed resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current ground speed
	 * @return ground speed resolution in internal units [m/s] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double groundSpeedResolution(boolean dir) {
		return groundSpeedResolution(dir,0);
	}

	/**
	 * Compute ground speed resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current ground speed
	 * @parameter u units
	 * @return ground speed resolution in specified units [u] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double groundSpeedResolution(boolean dir, String u) {
		return Units.to(u,groundSpeedResolution(dir));
	}

	/**
	 * Compute preferred ground speed direction, for given alert level, 
	 * based on resolution that is closer to current ground speed.
	 * True: Increase speed, False: Decrease speed.
	 */
	public boolean preferredGroundSpeedDirection(int alert_level) {
		return gs_band_.preferred_direction(core_, alert_level);
	}

	/**
	 * Compute preferred  ground speed direction, for conflict alert level, 
	 * based on resolution that is closer to current ground speed.
	 * True: Increase speed, False: Decrease speed.
	 */
	public boolean preferredGroundSpeedDirection() {
		return preferredGroundSpeedDirection(0);
	}

	/**
	 * @return the number of vertical speed band intervals, negative if the ownship has not been set
	 */
	public int verticalSpeedLength() {
		return vs_band_.length(core_);
	}

	/** 
	 * Force computation of vertical speed bands. Usually, bands are only computed when needed. This method
	 * forces the computation of vertical speed bands (this method is included mainly for debugging purposes). 
	 */
	public void forceVerticalSpeedBandsComputation() {
		vs_band_.force_compute(core_);
	}

	/**
	 * @return the i-th interval, in internal units [m/s], of the computed vertical speed bands.
	 * @param i index
	 */
	public Interval verticalSpeed(int i) {
		return vs_band_.interval(core_,i);
	}

	/**
	 * @return the i-th interval, in specified units [u], of the computed vertical speed bands.
	 * @param i index
	 * @param u units
	 */
	public Interval verticalSpeed(int i, String u) {
		Interval ia = vs_band_.interval(core_,i);
		if (ia.isEmpty()) {
			return ia;
		}
		return new Interval(Units.to(u, ia.low), Units.to(u, ia.up));
	}

	/**
	 * @return the i-th region of the computed vertical speed bands.
	 * @param i index
	 */
	public BandsRegion verticalSpeedRegion(int i) {
		return vs_band_.region(core_,i);
	}

	/**
	 * @return the region of a given vertical speed specified in internal units [m/s]
	 * @param vs [m/s]
	 */
	public int verticalSpeedRangeOf(double vs) {
		return vs_band_.rangeOf(core_,vs);
	}

	/**
	 * @return the region of a given vertical speed specified in given units [u]
	 * @param vs [u]
	 * @param u Units
	 */
	public int verticalSpeedRangeOf(double vs, String u) {
		return verticalSpeedRangeOf(Units.from(u, vs));
	}

	/**
	 * @return the region of a given vertical speed specified in internal units [m/s]
	 * @param vs [m/s]
	 */
	public BandsRegion regionOfVerticalSpeed(double vs) {
		return verticalSpeedRegion(verticalSpeedRangeOf(vs));
	}

	/**
	 * @return the region of a given vertical speed specified in given units [u]
	 * @param vs [u]
	 * @param u Units
	 */
	public BandsRegion regionOfVerticalSpeed(double vs, String u) {
		return verticalSpeedRegion(verticalSpeedRangeOf(vs,u));
	}

	/**
	 * Return last time to vertical speed maneuver, in seconds, for ownship with respect to traffic
	 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within 
	 * lookahead time. Return negative infinity if there is no time to maneuver.
	 */
	public double lastTimeToVerticalSpeedManeuver(TrafficState ac) {
		return vs_band_.last_time_to_maneuver(core_,ac);
	}

	/**
	 * @return time to recovery using vertical speed bands. Return NaN when bands are not saturated or when 
	 * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
	 */
	public double timeToVerticalSpeedRecovery() {
		return vs_band_.timeToRecovery(core_);
	}

	/**
	 * @return list of aircraft responsible for peripheral vertical speed bands for a given alert level.
	 * Note: alert level are 1-indexed. 
	 */
	public List<TrafficState> peripheralVerticalSpeedAircraft(int alert_level) {
		return vs_band_.peripheralAircraft(core_,alert_level);
	}

	/**
	 * Compute vertical speed resolution maneuver for given alert level.
	 * @parameter dir is up (true)/down (false) of ownship current vertical speed
	 * @return vertical speed resolution in internal units [m/s] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double verticalSpeedResolution(boolean dir, int alert_level) {
		return vs_band_.compute_resolution(core_,alert_level,dir);
	}

	/**
	 * Compute vertical speed resolution maneuver for given alert level.
	 * @parameter dir is up (true)/down (false) of ownship current vertical speed
	 * @parameter u units
	 * @return vertical speed resolution in specified units [u] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double verticalSpeedResolution(boolean dir, int alert_level, String u) {
		return Units.to(u,verticalSpeedResolution(dir,alert_level));
	}

	/**
	 * Compute vertical speed resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current vertical speed
	 * @return vertical speed resolution in internal units [m/s] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double verticalSpeedResolution(boolean dir) {
		return verticalSpeedResolution(dir,0);
	}

	/**
	 * Compute vertical speed resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current vertical speed
	 * @parameter u units
	 * @return vertical speed resolution in specified units [u] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double verticalSpeedResolution(boolean dir, String u) {
		return Units.to(u,verticalSpeedResolution(dir));
	}

	/**
	 * Compute preferred vertical speed direction, for given alert level, 
	 * based on resolution that is closer to current vertical speed.
	 * True: Increase speed, False: Decrease speed.
	 */
	public boolean preferredVerticalSpeedDirection(int alert_level) {
		return vs_band_.preferred_direction(core_, alert_level);
	}

	/**
	 * Compute preferred  vertical speed direction, for conflict alert level, 
	 * based on resolution that is closer to current vertical speed.
	 * True: Increase speed, False: Decrease speed.
	 */
	public boolean preferredVerticalSpeedDirection() {
		return preferredVerticalSpeedDirection(0);
	}

	/**
	 * @return the number of altitude band intervals, negative if the ownship has not been set.
	 */
	public int altitudeLength() {
		return alt_band_.length(core_);
	}

	/** 
	 * Force computation of altitude bands. Usually, bands are only computed when needed. This method
	 * forces the computation of altitude bands (this method is included mainly for debugging purposes). 
	 */
	public void forceAltitudeBandsComputation() {
		alt_band_.force_compute(core_);
	}

	/**
	 * @return the i-th interval, in internal units [m], of the computed altitude bands.
	 * @param i index
	 */
	public Interval altitude(int i) {
		return alt_band_.interval(core_,i);
	}

	/**
	 * @return the i-th interval, in specified units [u], of the computed altitude bands.
	 * @param i index
	 * @param u units
	 */
	public Interval altitude(int i, String u) {
		Interval ia = alt_band_.interval(core_,i);
		if (ia.isEmpty()) {
			return ia;
		}
		return new Interval(Units.to(u, ia.low), Units.to(u, ia.up));
	}

	/**
	 * @return the i-th region of the computed altitude bands.
	 * @param i index
	 */
	public BandsRegion altitudeRegion(int i) {
		return alt_band_.region(core_,i);
	}

	/**
	 * @return the range index of a given altitude specified internal units [m]
	 * @param alt [m]
	 */
	public int altitudeRangeOf(double alt) {
		return alt_band_.rangeOf(core_,alt);
	}

	/**
	 * @return the range index of a given altitude specified in given units [u]
	 * @param alt [u]
	 * @param u Units
	 */
	public int altitudeRangeOf(double alt, String u) {
		return altitudeRangeOf(Units.from(u,alt));
	}

	/**
	 * @return the region of a given altitude specified in internal units [m]
	 * @param alt [m]
	 */
	public BandsRegion regionOfAltitude(double alt) {
		return altitudeRegion(altitudeRangeOf(alt));
	}

	/**
	 * @return the region of a given altitude specified in given units [u]
	 * @param alt [u]
	 * @param u Units
	 */
	public BandsRegion regionOfAltitude(double alt, String u) {
		return altitudeRegion(altitudeRangeOf(alt,u));
	}

	/**
	 * Return last time to altitude maneuver, in seconds, for ownship with respect to traffic
	 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within 
	 * lookahead time. Return negative infinity if there is no time to maneuver.
	 */
	public double lastTimeToAltitudeManeuver(TrafficState ac) {
		return alt_band_.last_time_to_maneuver(core_,ac);
	}

	/**
	 * @return time to recovery using altitude bands. Return NaN when bands are not saturated or when 
	 * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
	 */
	public double timeToAltitudeRecovery() {
		return alt_band_.timeToRecovery(core_);
	}

	/**
	 * @return list of aircraft responsible for peripheral altitude bands for a given alert level.
	 * Note: alert level are 1-indexed. 
	 */
	public List<TrafficState> peripheralAltitudeAircraft(int alert_level) {
		return alt_band_.peripheralAircraft(core_,alert_level);
	}

	/**
	 * Compute altitude resolution maneuver for given alert level.
	 * @parameter dir is up (true)/down (false) of ownship current altitude
	 * @return altitude resolution in internal units [m] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double altitudeResolution(boolean dir, int alert_level) {
		return alt_band_.compute_resolution(core_,alert_level,dir);
	}

	/**
	 * Compute altitude resolution maneuver for given alert level.
	 * @parameter dir is up (true)/down (false) of ownship current altitude
	 * @parameter u units
	 * @return altitude resolution in specified units [u] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double altitudeResolution(boolean dir, int alert_level, String u) {
		return Units.to(u,altitudeResolution(dir,alert_level));
	}

	/**
	 * Compute altitude resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current altitude
	 * @return altitude resolution in internal units [m] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double altitudeResolution(boolean dir) {
		return altitudeResolution(dir,0);
	}

	/**
	 * Compute altitude resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current altitude
	 * @parameter u units
	 * @return altitude resolution in specified units [u] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double altitudeResolution(boolean dir, String u) {
		return Units.to(u,altitudeResolution(dir));
	}

	/**
	 * Compute preferred altitude direction, for given alert level, 
	 * based on resolution that is closer to current altitude.
	 * True: Climb, False: Descend.
	 */
	public boolean preferredAltitudeDirection(int alert_level) {
		return alt_band_.preferred_direction(core_, alert_level);
	}

	/**
	 * Compute preferred  altitude direction, for conflict alert level, 
	 * based on resolution that is closer to current altitude.
	 * True: Climb, False: Descend.
	 */
	public boolean preferredAltitudeDirection() {
		return preferredAltitudeDirection(0);
	}

	/** 
	 * Return true if and only if threshold values, defining an alerting level, are violated.
	 */ 
	private boolean check_thresholds(AlertThresholds athr, TrafficState ac, int turning, int accelerating, int climbing) {
		if (athr.isValid()) {
			Vect3 so = core_.ownship.get_s();
			Velocity vo = core_.ownship.get_v();
			Vect3 si = ac.get_s();
			Velocity vi = ac.get_v();
			Detection3D detector = athr.getDetector();	
			double alerting_time = Util.min(core_.parameters.getLookaheadTime(),athr.getAlertingTime());

			if (detector.violation(so,vo,si,vi)) {
				return true;
			}
			ConflictData det = detector.conflictDetection(so,vo,si,vi,0,alerting_time);
			if (det.conflict()) {
				return true;
			}
			if (athr.getTrackSpread() > 0 || athr.getGroundSpeedSpread() > 0 || 
					athr.getVerticalSpeedSpread() > 0 || athr.getAltitudeSpread() > 0) {
				if (athr.getTrackSpread() > 0) {
					KinematicTrkBands trk_band = new KinematicTrkBands(core_.parameters);
					trk_band.set_rel(true);
					trk_band.set_min(turning <= 0 ? -athr.getTrackSpread() : 0);
					trk_band.set_max(turning >= 0 ? athr.getTrackSpread() : 0);
					if (trk_band.kinematic_conflict(core_,ac,detector,alerting_time)) {
						return true;
					}
				}
				if (athr.getGroundSpeedSpread() > 0) {
					KinematicGsBands gs_band = new KinematicGsBands(core_.parameters);
					gs_band.set_rel(true);
					gs_band.set_min(accelerating <= 0 ? -athr.getGroundSpeedSpread() : 0);
					gs_band.set_max(accelerating >= 0 ? athr.getGroundSpeedSpread() : 0);
					if (gs_band.kinematic_conflict(core_,ac,detector,alerting_time)) {
						return true;
					}
				}
				if (athr.getVerticalSpeedSpread() > 0) {
					KinematicVsBands vs_band = new KinematicVsBands(core_.parameters);
					vs_band.set_rel(true);
					vs_band.set_min(climbing <= 0 ? -athr.getVerticalSpeedSpread() : 0);
					vs_band.set_max(climbing >= 0 ? athr.getVerticalSpeedSpread() : 0);
					if (vs_band.kinematic_conflict(core_,ac,detector,alerting_time)) {
						return true;
					}
				}
				if (athr.getAltitudeSpread() > 0) {
					KinematicAltBands alt_band = new KinematicAltBands(core_.parameters);
					alt_band.set_rel(true);
					alt_band.set_min(climbing <= 0 ? -athr.getAltitudeSpread() : 0);
					alt_band.set_max(climbing >= 0 ? athr.getAltitudeSpread() : 0);
					if (alt_band.kinematic_conflict(core_,ac,detector,alerting_time)) {
						return true;
					}
				}
			}
		}
		return false;
	}

	/** 
	 * Computes alerting level beteween ownship and aircraft. 
	 * The number 0 means no alert. A negative number means that aircraft index is not valid.
	 * When the alertor object has been configured to consider ownship maneuvers, i.e.,
	 * using spread values, the alerting logic could also use information about the ownship
	 * turning, accelerating, and climbing status as follows:
	 * - turning < 0: ownship is turning left, turning > 0: ownship is turning right, turning = 0: 
	 * do not make any turning assumption about the ownship.
	 * - accelerating < 0: ownship is decelerating, accelerating > 0: ownship is accelerating, 
	 * accelerating = 0: do not make any accelerating assumption about the ownship.
	 * - climbing < 0: ownship is descending, climbing > 0: ownship is climbing, climbing = 0:
	 * do not make any climbing assumption about the ownship.
	 */
	public int alerting(TrafficState ac, int turning, int accelerating, int climbing) {
		for (int alert_level=core_.parameters.alertor.mostSevereAlertLevel(); alert_level > 0; --alert_level) {
			AlertThresholds athr = core_.parameters.alertor.getLevel(alert_level);
			if (check_thresholds(athr,ac,turning,accelerating,climbing)) {
				return alert_level;
			}
		}
		return 0;
	}

	public String toString() {
		String s = "";
		s+="## KinematicMultiBands\n"; 
		s+=core_.toString();
		s+="## Track Bands Internals\n";
		s+=trk_band_.toString()+"\n";
		s+="## Ground Speed Bands Internals\n";
		s+=gs_band_.toString()+"\n";
		s+="## Vertical Speed Bands Internals\n";
		s+=vs_band_.toString()+"\n";
		s+="## Altitude Bands Internals\n";
		s+=alt_band_.toString()+"\n";
		return s;
	}

	public String outputStringInfo() {
		String s="";
		s+="Ownship Aircraft: "+core_.ownship.getId()+"\n";
		s+="Traffic Aircraft: "+TrafficState.listToString(core_.traffic)+"\n";
		s+="Conflict Criteria: "+(core_.parameters.isEnabledConflictCriteria()?"Enabled":"Disabled")+"\n";
		s+="Recovery Criteria: "+(core_.parameters.isEnabledRecoveryCriteria()?"Enabled":"Disabled")+"\n";
		s+="Most Urgent Aircraft: "+core_.most_urgent_ac.getId()+"\n";
		s+="Horizontal Epsilon: "+core_.epsilonH()+"\n";
		s+="Vertical Epsilon: "+core_.epsilonV()+"\n";
		return s;
	}

	public String outputStringAlerting() {
		String s="";
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Conflict Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(conflictAircraft(alert_level))+"\n";
		}
		return s;
	}

	public String outputStringTrackBands() {
		String s="";
		String u = core_.parameters.getUnits("trk_step");
		double val = core_.ownship.track();
		s+="Ownship Track: "+Units.str(u,val)+"\n";
		s+="Region of Current Track: "+regionOfTrack(val).toString()+"\n";
		s+="Track Bands ["+u+","+u+"]:\n"; 
		for (int i=0; i < trackLength(); ++i) {
			s+="  "+track(i,u).toString()+" "+trackRegion(i).toString()+"\n";
		} 
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Peripheral Track Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(peripheralTrackAircraft(alert_level))+"\n";
		}
		s+="Track Resolution (right): "+Units.str(u,trackResolution(true))+"\n";
		s+="Track Resolution (left): "+Units.str(u,trackResolution(false))+"\n";
		s+="Preferred Track Direction: ";
		if (preferredTrackDirection()) { 
			s+="right\n";
		} else {
			s+="left\n";
		}
		s+="Time to Track Recovery: "+Units.str("s",timeToTrackRecovery())+"\n";
		return s;
	}

	public String outputStringGroundSpeedBands() {
		String s="";
		String u = core_.parameters.getUnits("gs_step");
		double val = core_.ownship.groundSpeed();
		s+="Ownship Ground Speed: "+Units.str(u,val)+"\n";
		s+="Region of Current Ground Speed: "+regionOfGroundSpeed(val).toString()+"\n";
		s+="Ground Speed Bands ["+u+","+u+"]:\n"; 
		for (int i=0; i < groundSpeedLength(); ++i) {
			s+="  "+groundSpeed(i,u).toString()+" "+groundSpeedRegion(i).toString()+"\n";
		} 
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Peripheral Ground Speed Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(peripheralGroundSpeedAircraft(alert_level))+"\n";
		}
		s+="Ground Speed Resolution (up): "+Units.str(u,groundSpeedResolution(true))+"\n";
		s+="Ground Speed Resolution (down): "+Units.str(u,groundSpeedResolution(false))+"\n";
		s+="Preferred Ground Speed Direction: ";
		if (preferredGroundSpeedDirection()) {
			s+="up\n";
		} else {
			s+="down\n";
		}
		s+="Time to Ground Speed Recovery: "+Units.str("s",timeToGroundSpeedRecovery())+"\n";
		return s;
	}

	public String outputStringVerticalSpeedBands() {
		String s="";
		String u = core_.parameters.getUnits("vs_step");
		double val = core_.ownship.verticalSpeed();
		s+="Ownship Vertical Speed: "+Units.str(u,val)+"\n";
		s+="Region of Current Vertical Speed: "+regionOfVerticalSpeed(val).toString()+"\n";
		s+="Vertical Speed Bands ["+u+","+u+"]:\n";
		for (int i=0; i < verticalSpeedLength(); ++i) {
			s+="  "+verticalSpeed(i,u).toString()+" "+ verticalSpeedRegion(i).toString()+"\n";
		} 
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Peripheral Vertical Speed Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(peripheralVerticalSpeedAircraft(alert_level))+"\n";
		}
		s+="Vertical Speed Resolution (up): "+Units.str(u,verticalSpeedResolution(true))+"\n";
		s+="Vertical Speed Resolution (down): "+Units.str(u,verticalSpeedResolution(false))+"\n";
		s+="Preferred Vertical Speed Direction: ";
		if (preferredVerticalSpeedDirection()) {
			s+="up\n";
		} else {
			s+="down\n";
		}
		s+="Time to Vertical Speed Recovery: "+Units.str("s",timeToVerticalSpeedRecovery())+"\n";
		return s;
	}

	public String outputStringAltitudeBands() {
		String s="";
		String u = core_.parameters.getUnits("alt_step");
		double val = core_.ownship.altitude();
		s+="Ownship Altitude: "+Units.str(u,val)+"\n";
		s+="Region of Current Altitude: "+regionOfAltitude(val).toString()+"\n";
		s+="Altitude Bands ["+u+","+u+"]:\n";
		for (int i=0; i < altitudeLength(); ++i) {
			s+="  "+altitude(i,u).toString()+" "+ altitudeRegion(i).toString()+"\n";
		} 
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Peripheral Altitude Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(peripheralAltitudeAircraft(alert_level))+"\n";
		}
		s+="Altitude Resolution (up): "+Units.str(u,altitudeResolution(true))+"\n";
		s+="Altitude Resolution (down): "+Units.str(u,altitudeResolution(false))+"\n";
		s+="Preferred Altitude Direction: ";
		if (preferredAltitudeDirection()) {
			s+="up\n";
		} else {
			s+="down\n";
		}
		s+="Time to Altitude Recovery: "+Units.str("s",timeToAltitudeRecovery())+"\n";
		return s;
	}

	public String outputStringLastTimeToManeuver() {
		String s="";
		for (TrafficState ac : core_.traffic) {
			s+="Last Times to Maneuver with Respect to "+ac.getId()+"\n";
			s+="  Last Time to Track Maneuver: "+Units.str("s",lastTimeToTrackManeuver(ac))+"\n";
			s+="  Last Time to Ground Speed Maneuver: "+Units.str("s",lastTimeToGroundSpeedManeuver(ac))+"\n";
			s+="  Last Time to Vertical Speed Maneuver: "+Units.str("s",lastTimeToVerticalSpeedManeuver(ac))+"\n";
			s+="  Last Time to Altitude Maneuver: "+Units.str("s",lastTimeToAltitudeManeuver(ac))+"\n";
		}
		return s;
	}

	public String outputString() {
		String s="";
		s+=outputStringInfo();
		s+=outputStringAlerting();
		s+=outputStringTrackBands();
		s+=outputStringGroundSpeedBands();
		s+=outputStringVerticalSpeedBands();
		s+=outputStringAltitudeBands();
		s+=outputStringLastTimeToManeuver();
		return s;
	}

	public String toPVS(int precision) {
		String s="";
		s += "%%% Conflict Aircraft:\n";
		s += "(: ";
		boolean comma = false;
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			if (comma) {
				s += ", ";
			} else {
				comma = true;
			}
			s += "("+alert_level+","+TrafficState.listToPVSStringList(conflictAircraft(alert_level),precision)+")";
		}
		s += " :)::list[[nat,list[string]]]\n";
		s += "%%% Region of Current Track:\n"+
				trackRegion(trackRangeOf(getOwnship().track()))+"\n";
		s += "%%% Track Bands: "+trackLength()+"\n";
		s += trk_band_.toPVS(precision)+"\n";
		s += "%%% Peripheral Track Aircraft:\n";
		s += "(: ";
		comma = false;
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			if (comma) {
				s += ", ";
			} else {
				comma = true;
			}
			s += "("+alert_level+","+TrafficState.listToPVSStringList(peripheralTrackAircraft(alert_level),precision)+")";
		}
		s += " :)::list[[nat,list[string]]]\n";
		s += "%%% Track Resolution:\n";
		s += "("+f.double2PVS(trackResolution(false),precision)+
				","+f.double2PVS(trackResolution(true),precision)+
				","+preferredTrackDirection()+")\n";
		s += "%%% Time to Track Recovery:\n"+f.double2PVS(timeToTrackRecovery(),2)+"\n";
		s += "%%% Last Times to Track Maneuver:\n(:"; 
		comma = false;
		for (TrafficState ac : getTraffic()) {
			if (comma) {
				s += ",";
			} else {
				comma = true;
			}
			s += " "+f.double2PVS(lastTimeToTrackManeuver(ac),2);
		}
		s += " :)\n";

		s += "%%% Region of Current Ground Speed:\n"+
				groundSpeedRegion(groundSpeedRangeOf(getOwnship().groundSpeed()))+"\n";
		s += "%%% Ground Speed Bands: "+groundSpeedLength()+"\n";
		s += gs_band_.toPVS(precision)+"\n";
		s += "%%% Peripheral Ground Speed Aircraft:\n";
		s += "(: ";
		comma = false;
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			if (comma) {
				s += ", ";
			} else {
				comma = true;
			}
			s += "("+alert_level+","+TrafficState.listToPVSStringList(peripheralGroundSpeedAircraft(alert_level),precision)+")";
		}
		s += " :)::list[[nat,list[string]]]\n";
		s += "%%% Ground Speed Resolution:\n";
		s += "("+f.double2PVS(groundSpeedResolution(false),precision)+
				","+f.double2PVS(groundSpeedResolution(true),precision)+
				","+preferredGroundSpeedDirection()+")\n";
		s += "%%% Time to Ground Speed Recovery:\n"+f.double2PVS(timeToGroundSpeedRecovery(),2)+"\n";
		s += "%%% Last Times to Ground Speed Maneuver:\n(:"; 
		comma = false;
		for (TrafficState ac : getTraffic()) {
			if (comma) {
				s += ",";
			} else {
				comma = true;
			}
			s += " "+f.double2PVS(lastTimeToGroundSpeedManeuver(ac),2);
		}
		s += " :)\n";

		s += "%%% Region of Current Vertical Speed:\n"+
				verticalSpeedRegion(verticalSpeedRangeOf(getOwnship().verticalSpeed()))+"\n";
		s += "%%% Vertical Speed Bands: "+verticalSpeedLength()+"\n";
		s += vs_band_.toPVS(precision)+"\n";
		s += "%%% Peripheral Vertical Speed Aircraft:\n";
		s += "(: ";
		comma = false;
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			if (comma) {
				s += ", ";
			} else {
				comma = true;
			}
			s += "("+alert_level+","+TrafficState.listToPVSStringList(peripheralVerticalSpeedAircraft(alert_level),precision)+")";
		}
		s += " :)::list[[nat,list[string]]]\n";
		s += "%%% Vertical Speed Resolution:\n";
		s += "("+f.double2PVS(verticalSpeedResolution(false),precision)+
				","+f.double2PVS(verticalSpeedResolution(true),precision)+
				","+preferredVerticalSpeedDirection()+")\n";
		s += "%%% Time to Vertical Speed Recovery:\n"+f.double2PVS(timeToVerticalSpeedRecovery(),2)+"\n";
		s += "%%% Last Times to Vertical Speed Maneuver:\n(:"; 
		comma = false;
		for (TrafficState ac : getTraffic()) {
			if (comma) {
				s += ",";
			} else {
				comma = true;
			}
			s += " "+f.double2PVS(lastTimeToVerticalSpeedManeuver(ac),2);
		}
		s += " :)\n";

		s += "%%% Region of Current Altitude:\n"+
				altitudeRegion(altitudeRangeOf(getOwnship().altitude()))+"\n";
		s += "%%% Altitude Bands: "+altitudeLength()+"\n";
		s += alt_band_.toPVS(precision)+"\n";
		s += "%%% Peripheral Altitude Aircraft:\n";
		s += "(: ";
		comma = false;
		for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
			if (comma) {
				s += ", ";
			} else {
				comma = true;
			}
			s += "("+alert_level+","+TrafficState.listToPVSStringList(peripheralAltitudeAircraft(alert_level),precision)+")";
		}
		s += " :)::list[[nat,list[string]]]\n";
		s += "%%% Altitude Resolution:\n";
		s += "("+f.double2PVS(altitudeResolution(false),precision)+
				","+f.double2PVS(altitudeResolution(true),precision)+
				","+preferredAltitudeDirection()+")\n";
		s += "%%% Time to Altitude Recovery:\n"+f.double2PVS(timeToAltitudeRecovery(),2)+"\n";
		s += "%%% Last Times to Altitude Maneuver:\n(:"; 
		comma = false;
		for (TrafficState ac : getTraffic()) {
			if (comma) {
				s += ",";
			} else {
				comma = true;
			}
			s += " "+f.double2PVS(lastTimeToAltitudeManeuver(ac),2);
		}
		s += " :)\n";

		return s;
	}

	public void linearProjection(double offset) {
		if (offset != 0) {
			core_.ownship = core_.ownship.linearProjection(offset);
			for (int i = 0; i < core_.traffic.size(); i++) {
				core_.traffic.set(i,core_.traffic.get(i).linearProjection(offset));
			}   
			reset();
		}
	}

	// ErrorReporter Interface Methods

	public boolean hasError() {
		return error.hasError();
	}

	public boolean hasMessage() {
		return error.hasMessage();
	}

	public String getMessage() {
		return error.getMessage();
	}

	public String getMessageNoClear() {
		return error.getMessageNoClear();
	}

	/** 
	 * @return lookahead time in seconds. 
	 */ 
	public double getLookaheadTime() {
		return core_.parameters.getLookaheadTime();
	}

	/**
	 * @return lookahead time in specified units [u]. 
	 */

	public double getLookahedTime(String u) {
		return Units.to(u,getLookaheadTime());
	}

	/** 
	 * Sets lookahead time in seconds. 
	 */ 
	public void setLookaheadTime(double t) {
		core_.parameters.setLookaheadTime(t);
		reset();
	}

	/** 
	 * Sets lookahead time in specified units [u]. 
	 */ 
	public void setLookaheadTime(double t, String u) {
		setLookaheadTime(Units.from(u,t));
	}

	public boolean isLatLon() {
		return hasOwnship() && core_.ownship.isLatLon();
	}

}

