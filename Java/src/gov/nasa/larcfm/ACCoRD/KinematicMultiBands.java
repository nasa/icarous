/* 
 * Kinematic Multi Bands
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

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Interval;
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

	public KinematicTrkBands  trk_band_; 
	public KinematicGsBands   gs_band_;  
	public KinematicVsBands   vs_band_;  
	public KinematicAltBands  alt_band_; 
	public KinematicBandsCore core_;

	protected ErrorLog error = new ErrorLog("KinematicMultiBands");

	/** 
	 * Construct a KinematicMultiBands without an empty list of detectors. 
	 */
	public KinematicMultiBands() {
		core_ = new KinematicBandsCore();
		trk_band_ = new KinematicTrkBands();
		gs_band_ = new KinematicGsBands();
		vs_band_ = new KinematicVsBands();
		alt_band_ = new KinematicAltBands();
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

	/** General Settings **/

	/**
	 * Set alert thresholds
	 */
	public void setAlertor(AlertLevels alertor) {
		core_.alertor.setAll(alertor);
		reset();
	}

	/**
	 * Set bands parameters
	 */
	public void setParameters(DaidalusParameters parameters) {
		core_.setParameters(parameters);

		// Set Track Bands
		setLeftTrack(parameters.getLeftTrack());
		setRightTrack(parameters.getRightTrack());
		trk_band_.set_step(parameters.getTrackStep());  
		trk_band_.set_turn_rate(parameters.getTurnRate()); 
		trk_band_.set_bank_angle(parameters.getBankAngle()); 
		trk_band_.set_recovery(parameters.isEnabledRecoveryTrackBands());

		// Set Ground Speed Bands
		setMinGroundSpeed(parameters.getMinGroundSpeed());   
		setMaxGroundSpeed(parameters.getMaxGroundSpeed());   
		gs_band_.set_step(parameters.getGroundSpeedStep());
		gs_band_.set_horizontal_accel(parameters.getHorizontalAcceleration()); 
		gs_band_.set_recovery(parameters.isEnabledRecoveryGroundSpeedBands());

		setVerticalAcceleration(parameters.getVerticalAcceleration()); 

		// Set Vertical Speed Bands
		setMinVerticalSpeed(parameters.getMinVerticalSpeed());   
		setMaxVerticalSpeed(parameters.getMaxVerticalSpeed());  
		vs_band_.set_step(parameters.getVerticalSpeedStep()); 
		vs_band_.set_recovery(parameters.isEnabledRecoveryVerticalSpeedBands());   

		// Set Altitude Bands
		setMinAltitude(parameters.getMinAltitude()); 
		setMaxAltitude(parameters.getMaxAltitude()); 
		alt_band_.set_step(parameters.getAltitudeStep()); 
		alt_band_.set_vertical_rate(parameters.getVerticalRate()); 
		alt_band_.set_recovery(parameters.isEnabledRecoveryAltitudeBands());   

		reset();
	}

	/**
	 * @return recovery stability time in seconds. Recovery bands are computed at time of first green plus
	 * this time.
	 */
	public double getRecoveryStabilityTime() {
		return core_.recovery_stability_time;
	}

	/**
	 * Sets recovery stability time in seconds. Recovery bands are computed at time of first green plus
	 * this time.
	 */
	public void setRecoveryStabilityTime(double t) {
		if (error.isNonNegative("setRecoveryStabilityTime",t) && 
				t != core_.recovery_stability_time) {
			core_.recovery_stability_time = t;
			reset();
		}
	}

	/**
	 * @return minimum horizontal separation for recovery bands in internal units [m].
	 */
	public double getMinHorizontalRecovery() {
		return core_.min_horizontal_recovery;
	}

	/** 
	 * Return minimum horizontal separation for recovery bands in specified units [u]
	 */
	public double getMinHorizontalRecovery(String u) {
		return Units.to(u,getMinHorizontalRecovery());
	}

	/** 
	 * Sets minimum horizontal separation for recovery bands in internal units [m].
	 */
	public void setMinHorizontalRecovery(double val) {
		if (error.isNonNegative("setMinHorizontalRecovery",val) && 
				val != core_.min_horizontal_recovery) {
			core_.min_horizontal_recovery = val;
			reset();
		}
	}

	/** 
	 * Set minimum horizontal separation for recovery bands in specified units [u].
	 */
	public void setMinHorizontalRecovery(double val, String u) {
		setMinHorizontalRecovery(Units.from(u,val));
	}

	/** 
	 * @return minimum vertical separation for recovery bands in internal units [m].
	 */
	public double getMinVerticalRecovery() {
		return core_.min_vertical_recovery;
	}

	/** 
	 * Return minimum vertical separation for recovery bands in specified units [u].
	 */
	public double getMinVerticalRecovery(String u) {
		return Units.to(u,getMinVerticalRecovery());
	}

	/**
	 * Sets minimum vertical separation for recovery bands in internal units [m].
	 */
	public void setMinVerticalRecovery(double val) {
		if (error.isNonNegative("setMinVerticalRecovery",val) 
				&& val != core_.min_vertical_recovery) {
			core_.min_vertical_recovery = val;
			reset();
		}
	}

	/** 
	 * Set minimum vertical separation for recovery bands in units
	 */
	public void setMinVerticalRecovery(double val, String u) {
		setMinVerticalRecovery(Units.from(u,val));
		reset();
	}

	/** 
	 * @return true if collision avoidance bands are enabled.
	 */
	public boolean isEnabledCollisionAvoidanceBands() {
		return core_.ca_bands;
	}

	/** 
	 * Enable/disable collision avoidance bands.
	 */ 
	public void setCollisionAvoidanceBands(boolean flag) {
		if (flag != core_.ca_bands) {
			core_.ca_bands = flag;
			reset();
		}
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
		return core_.conflict_crit;
	}

	/** 
	 * Enable/disable repulsive criteria for conflict bands.
	 */
	public void setConflictCriteria(boolean flag) {
		core_.conflict_crit = flag;
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
		return core_.recovery_crit;
	}

	/** 
	 * Enable/disable repulsive criteria for recovery bands.
	 */
	public void setRecoveryCriteria(boolean flag) {
		core_.recovery_crit = flag;
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

	/** Track Bands Settings **/

	/** 
	 * @return left track in radians [-pi - 0] [rad] from current ownship's track
	 */
	public double getLeftTrack() {
		if (trk_band_.get_rel()) {
			return trk_band_.get_min();
		} else if (trk_band_.check_input(core_.ownship)) {
			return -trk_band_.min_rel(core_.ownship);
		} else {
			return Double.NaN;
		}
	}

	/** 
	 * @return left track in specified units [-pi - 0] [u] from current ownship's track
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
		} else if (trk_band_.check_input(core_.ownship)) {
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
	 * Set left track to value in internal units [rad]. Value is expected to be in [-pi - 0]
	 */
	public void setLeftTrack(double val) {
		val = -Math.abs(val);
		if (error.isBetween("setLeftTrack",val,-Math.PI,0)) {
			if (!trk_band_.get_rel()) {
				trk_band_.set_rel(true);
			}
			trk_band_.set_min(val);
		}    
	}

	/** 
	 * Set left track to value in specified units [u]. Value is expected to be in [-pi - 0]
	 */
	public void setLeftTrack(double val, String u) {
		setLeftTrack(Units.from(u,val));
	}

	/** 
	 * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
	 */
	public void setRightTrack(double val) {
		val = Math.abs(val);
		if (error.isBetween("setRightTrack",val,0,Math.PI)) {
			if (!trk_band_.get_rel()) {
				trk_band_.set_rel(true);
			}
			trk_band_.set_max(val);
		}    
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
		if (error.isBetween("setMinMaxTrack",min,0,2*Math.PI) &&
				error.isBetween("setMinMaxTrack",max,0,2*Math.PI)) {
			if (trk_band_.get_rel()) {
				trk_band_.set_rel(false);
			}
			trk_band_.set_min(min);
			trk_band_.set_max(max);
		}     
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
		return trk_band_.get_step();
	}

	/** 
	 * @return step size for track bands in specified units [u]. 
	 */
	public double getTrackStep(String u) {
		return Units.to(u,getTrackStep());
	}

	/** 
	 * Sets step size for track bands in internal units [rad].
	 */
	public void setTrackStep(double val) {
		if (error.isPositive("setTrackStep",val) && 
				error.isLessThan("setTrackStep",val,Math.PI)) {
			trk_band_.set_step(val);
		}
	}

	/** 
	 * Sets step size for track bands in specified units [u].
	 */
	public void setTrackStep(double val, String u) {
		setTrackStep(Units.from(u,val));
	}

	/** 
	 * @return bank angle in internal units [rad].
	 */
	public double getBankAngle() {
		return trk_band_.get_bank_angle();
	}

	/** 
	 * @return bank angle in specified units [u].
	 */
	public double getBankAngle(String u) {
		return Units.to(u,getBankAngle());
	}

	/** 
	 * Sets bank angle for track bands to value in internal units [rad]. As a side effect, this method
	 * resets the turn rate.
	 */
	public void setBankAngle(double val) {
		if (error.isNonNegative("setBankAngle",val)) {
			trk_band_.set_bank_angle(val);
			trk_band_.set_turn_rate(0);
		}
	}

	/** 
	 * Sets bank angle for track bands to value in specified units [u]. As a side effect, this method
	 * resets the turn rate.
	 */
	public void setBankAngle(double val, String u) {
		setBankAngle(Units.from(u,val));
	}

	/** 
	 * @return turn rate in internal units [rad/s].
	 */
	public double getTurnRate() {
		return trk_band_.get_turn_rate();
	}

	/** 
	 * @return turn rate in specified units [u].
	 */
	public double getTurnRate(String u) {
		return Units.to(u,getTurnRate());
	}

	/** 
	 * Sets turn rate for track bands to value in internal units [rad/s]. As a side effect, this method
	 * resets the bank angle.
	 */
	public void setTurnRate(double val) {
		if (error.isNonNegative("setTurnRate",val)) {
			trk_band_.set_turn_rate(val);
			trk_band_.set_bank_angle(0);
		}
	}

	/** 
	 * Sets turn rate for track bands to value in specified units [u]. As a side effect, this method
	 * resets the bank angle.
	 */
	public void setTurnRate(double rate, String u) {
		setTurnRate(Units.from(u,rate));
	}

	/**
	 * @return true if recovery track bands are enabled.
	 */
	public boolean isEnabledRecoveryTrackBands() {
		return trk_band_.get_recovery();
	}

	/** 
	 * Sets recovery bands flag for track bands to specified value.
	 */ 
	public void setRecoveryTrackBands(boolean flag) {
		trk_band_.set_recovery(flag);
	}

	/** Ground Speed Bands Settings **/

	/** 
	 * @return minimum ground speed for ground speed bands in internal units [m/s].
	 */
	public double getMinGroundSpeed() {
		if (gs_band_.get_rel()) {
			if (gs_band_.check_input(core_.ownship)) {
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
			if (gs_band_.check_input(core_.ownship)) {
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
	 * Ground speeds are specified in internal units [m/s]. Below value is expected to be non-positive,
	 * and above value is expected to be non-negative.
	 */
	public void setBelowAboveGroundSpeed(double below, double above) {
		below = -Math.abs(below);
		above = Math.abs(above);
		if (!gs_band_.get_rel()) {
			gs_band_.set_rel(true);
		}
		gs_band_.set_min(below);
		gs_band_.set_max(above);
	}     

	/** 
	 * Set below/above ground speed, relative to ownship's ground speed, for bands computations. 
	 * Ground speeds are specified in given units [u]. Below value is expected to be non-positive,
	 * and above value is expected to be non-negative.
	 */
	public void setBelowAboveGroundSpeed(double below, double above, String u) {
		setBelowAboveGroundSpeed(Units.from(u,below),Units.from(u,above));
	}

	/** 
	 * @return step size for ground speed bands in internal units [m/s]. 
	 */
	public double getGroundSpeedStep() {
		return gs_band_.get_step();
	}

	/** 
	 * @return step size for ground speed bands in specified units [u]. 
	 */
	public double getGroundSpeedStep(String u) {
		return Units.to(u,getGroundSpeedStep());
	}

	/** 
	 * Sets step size for ground speed bands to value in internal units [m/s].
	 */
	public void setGroundSpeedStep(double val) {
		if (error.isPositive("setGroundSpeedStep",val)) {
			gs_band_.set_step(val);
		}
	}

	/** 
	 * Sets step size for ground speed bands to value in specified units [u].
	 */
	public void setGroundSpeedStep(double val, String u) {
		setGroundSpeedStep(Units.from(u,val));
	}

	/** 
	 * @return horizontal acceleration for ground speed bands to value in internal units [m/s^2]. 
	 */
	public double getHorizontalAcceleration() {
		return gs_band_.get_horizontal_accel();
	}

	/** 
	 * @return horizontal acceleration for ground speed bands to value in specified units [u]. 
	 */
	public double getHorizontalAcceleration(String u) {
		return Units.to(u,getHorizontalAcceleration());
	}

	/** 
	 * Sets horizontal acceleration for ground speed bands to value in internal units [m/s^2].
	 */
	public void setHorizontalAcceleration(double val) {
		if (error.isNonNegative("setHorizontalAcceleration",val)) {
			gs_band_.set_horizontal_accel(val);
		}
	}

	/** 
	 * Sets horizontal acceleration for ground speed bands to value in specified units [u].
	 */
	public void setHorizontalAcceleration(double val, String u) {
		setHorizontalAcceleration(Units.from(u,val));
	}

	/**
	 * @return true if recovery ground speed bands are enabled.
	 */
	public boolean isEnabledRecoveryGroundSpeedBands() {
		return gs_band_.get_recovery();
	}

	/** 
	 * Sets recovery bands flag for ground speed bands to specified value.
	 */ 
	public void setRecoveryGroundSpeedBands(boolean flag) {
		gs_band_.set_recovery(flag);
	}

	/** Vertical Speed Bands Settings **/

	/** 
	 * @return minimum vertical speed for vertical speed bands in internal units [m/s].
	 */
	public double getMinVerticalSpeed() {
		if (vs_band_.get_rel()) {
			if (vs_band_.check_input(core_.ownship)) {
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
			if (vs_band_.check_input(core_.ownship)) {
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
	}

	/** 
	 * Sets maximum vertical speed for vertical speed bands to value in specified units [u].
	 */
	public void setMaxVerticalSpeed(double val, String u) {
		setMaxVerticalSpeed(Units.from(u, val));
	}

	/** 
	 * Set below/above vertical speed, relative to ownship's vertical speed, for bands computations. 
	 * Vertical speeds are specified in internal units [m/s]. Below value is expected to be non-positive,
	 * and above value is expected to be non-negative.
	 */
	public void setBelowAboveVerticalSpeed(double below, double above) {
		below = -Math.abs(below);
		above = Math.abs(above);
		if (!vs_band_.get_rel()) {
			vs_band_.set_rel(true);
		}
		vs_band_.set_min(below);
		vs_band_.set_max(above);
	}

	/** 
	 * Set below/above vertical speed, relative to ownship's vertical speed, for bands computations. 
	 * Vertical speeds are specified in given units [u]. Below value is expected to be non-positive,
	 * and above value is expected to be non-negative.
	 */
	public void setBelowAboveVerticalSpeed(double below, double above, String u) {
		setBelowAboveVerticalSpeed(Units.from(u,below),Units.from(u,above));
	}

	/** 
	 * @return step size for vertical speed bands in internal units [m/s].
	 */
	public double getVerticalSpeedStep() {
		return vs_band_.get_step();
	}

	/** 
	 * @return step size for vertical speed bands in specified units [u].
	 */
	public double getVerticalSpeedStep(String u) {
		return Units.to(u,getVerticalSpeedStep());
	}

	/** 
	 * Sets step size for vertical speed bands to value in internal units [m/s].
	 */
	public void setVerticalSpeedStep(double val) {
		if (error.isPositive("setVerticalSpeedStep",val)) {
			vs_band_.set_step(val);
		}
	}

	/** 
	 * Sets step size for vertical speed bands to value in specified units [u].
	 */
	public void setVerticalSpeedStep(double val, String u) {
		setVerticalSpeedStep(Units.from(u,val));
	}

	/** 
	 * @return constant vertical acceleration for vertical speed and altitude bands in internal [m/s^2]
	 * units
	 */
	public double getVerticalAcceleration() {
		return vs_band_.get_vertical_accel();
	}

	/** 
	 * @return constant vertical acceleration for vertical speed and altitude bands in specified
	 * units
	 */
	public double getVerticalAcceleration(String u) {
		return Units.to(u,getVerticalAcceleration());
	}

	/** 
	 * Sets the constant vertical acceleration for vertical speed and altitude bands
	 * to value in internal units [m/s^2]
	 */
	public void setVerticalAcceleration(double val) {
		if (error.isNonNegative("setVerticalAcceleration",val)) {
			vs_band_.set_vertical_accel(val);
			alt_band_.set_vertical_accel(val);
		}
	}

	/** 
	 * Sets the constant vertical acceleration for vertical speed and altitude bands
	 * to value in specified units [u].
	 */
	public void setVerticalAcceleration(double val, String u) {
		setVerticalAcceleration(Units.from(u,val));
	}

	/**
	 * @return true if recovery vertical speed bands are enabled.
	 */
	public boolean isEnabledRecoveryVerticalSpeedBands() {
		return vs_band_.get_recovery();
	}

	/** 
	 * Sets recovery bands flag for vertical speed bands to specified value.
	 */ 
	public void setRecoveryVerticalSpeedBands(boolean flag) {
		vs_band_.set_recovery(flag);
	}

	/** Altitude Bands Settings **/

	/** 
	 * @return minimum altitude for altitude bands in internal units [m]
	 */
	public double getMinAltitude() {
		if (alt_band_.get_rel()) {
			if (alt_band_.check_input(core_.ownship)) {
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
			if (alt_band_.check_input(core_.ownship)) {
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
	 * Altitude are specified in internal units [m]. Below value is expected to be non-positive,
	 * and above value is expected to be non-negative.
	 */
	public void setBelowAboveAltitude(double below, double above) {
		below = -Math.abs(below);
		above = Math.abs(above);
		if (!alt_band_.get_rel()) {
			alt_band_.set_rel(true);
		}
		alt_band_.set_min(below);
		alt_band_.set_max(above);
	}     

	/** 
	 * Set below/above altitude, relative to ownship's altitude, for bands computations. 
	 * Altitudes are specified in given units [u]. Below value is expected to be non-positive,
	 * and above value is expected to be non-negative.
	 */
	public void setBelowAboveAltitude(double below, double above, String u) {
		setBelowAboveAltitude(Units.from(u,below),Units.from(u,above));
	}

	/** 
	 * @return step size for altitude bands in internal units [m]
	 */
	public double getAltitudeStep() {
		return alt_band_.get_step();
	}

	/** 
	 * @return step size for altitude bands in specified units [u].
	 */
	public double getAltitudeStep(String u) {
		return Units.to(u,getAltitudeStep());
	}

	/** 
	 * Sets step size for altitude bands to value in internal units [m]
	 */
	public void setAltitudeStep(double val) {
		if (error.isPositive("setAltitudeStep",val)) {
			alt_band_.set_step(val);
		}
	}

	/** 
	 * Sets step size for altitude bands to value in specified units [u].
	 */
	public void setAltitudeStep(double val, String u) {
		setAltitudeStep(Units.from(u,val));
	}

	/** 
	 * @return the vertical climb/descend rate for altitude bands in internal units [m/s]
	 */
	public double getVerticalRate() {
		return alt_band_.get_vertical_rate();
	}

	/** 
	 * @return the vertical climb/descend rate for altitude bands in specified units [u].
	 */
	public double getVerticalRate(String u) {
		return Units.to(u,getVerticalRate());
	}

	/** 
	 * Sets vertical rate for altitude bands to value in internal units [m/s]
	 */
	public void setVerticalRate(double val) {
		if (error.isNonNegative("setVerticalRate",val)) {
			alt_band_.set_vertical_rate(val);
		}
	}

	/** 
	 * Sets vertical rate for altitude bands to value in specified units [u].
	 */
	public void setVerticalRate(double rate, String u) {
		setVerticalRate(Units.from(u,rate));
	}

	/**
	 * @return true if recovery altitude bands are enabled.
	 */
	public boolean isEnabledRecoveryAltitudeBands() {
		return alt_band_.get_recovery();
	}

	/** 
	 * Sets recovery bands flag for altitude bands to specified value.
	 */ 
	public void setRecoveryAltitudeBands(boolean flag) {
		alt_band_.set_recovery(flag);
	}

	/** Utility methods **/

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

	/** Main interface methods **/

	/**
	 *  Return list of conflict aircraft for a given alert level.
	 *  Note: alert level are 1-indexed.
	 */
	public List<TrafficState> conflictAircraft(int alert_level) {
		return core_.conflictAircraft(alert_level);
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
	 * @return the region of a given track specified in internal units [rad]
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
	 * Compute track resolution maneuver for conflict alert level.
	 * @parameter dir is right (true)/left (false) of ownship current track
	 * @return track resolution in internal units [rad] in specified direction.
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no resolution to the right, and negative infinity if there 
	 * is no resolution to the left.
	 */
	public double trackResolution(boolean dir) {
		return trk_band_.compute_resolution(core_,dir);
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
	 * Compute ground speed resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current ground speed
	 * @return ground speed resolution in internal units [m/s] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double groundSpeedResolution(boolean dir) {
		return gs_band_.compute_resolution(core_,dir);
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
	 * Compute vertical speed resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current vertical speed
	 * @return vertical speed resolution in internal units [m/s] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double verticalSpeedResolution(boolean dir) {
		return vs_band_.compute_resolution(core_,dir);
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
	 * Compute altitude resolution maneuver for conflict alert level.
	 * @parameter dir is up (true)/down (false) of ownship current altitude
	 * @return altitude resolution in internal units [m] in specified direction. 
	 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict, 
	 * positive infinity if there is no up resolution, and negative infinity if there 
	 * is no down resolution.
	 */
	public double altitudeResolution(boolean dir) {
		int conflict_level = core_.alertor.conflictAlertLevel();
		if (conflict_level == 0 || core_.conflictAircraft(conflict_level).isEmpty()) {
			return Double.NaN;
		} else {
			return alt_band_.compute_resolution(core_,dir);
		}
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

	public String toString() {
		int precision = Constants.get_output_precision();
		String s = ""; 
		s+=core_.alertor.toString()+"\n";
		s+="# Default Parameters (Bands)\n";
		s+="min_trk = "+DaidalusParameters.val_unit(trk_band_.get_min(),"deg")+"\n";
		s+="max_trk = "+DaidalusParameters.val_unit(trk_band_.get_max(),"deg")+"\n";
		s+="rel_trk = "+trk_band_.get_rel()+"\n";
		s+="min_gs = "+DaidalusParameters.val_unit(gs_band_.get_min(),"knot")+"\n";
		s+="max_gs = "+DaidalusParameters.val_unit(gs_band_.get_max(),"knot")+"\n";
		s+="rel_gs = "+gs_band_.get_rel()+"\n";
		s+="min_vs = "+DaidalusParameters.val_unit(vs_band_.get_min(),"fpm")+"\n";
		s+="max_vs = "+DaidalusParameters.val_unit(vs_band_.get_max(),"fpm")+"\n";
		s+="rel_vs = "+vs_band_.get_rel()+"\n";
		s+="min_alt = "+DaidalusParameters.val_unit(alt_band_.get_min(),"ft")+"\n";
		s+="max_alt = "+DaidalusParameters.val_unit(alt_band_.get_max(),"ft")+"\n";
		s+="rel_alt = "+alt_band_.get_rel()+"\n";
		s+="# Default Parameters (Kinematic Bands)\n";
		s+="trk_step = "+DaidalusParameters.val_unit(trk_band_.get_step(),"deg")+"\n";
		s+="gs_step = "+DaidalusParameters.val_unit(gs_band_.get_step(),"knot")+"\n";
		s+="vs_step = "+DaidalusParameters.val_unit(vs_band_.get_step(),"fpm")+"\n";
		s+="alt_step = "+DaidalusParameters.val_unit(alt_band_.get_step(),"ft")+"\n";
		s+="horizontal_accel = "+DaidalusParameters.val_unit(getHorizontalAcceleration(),"m/s^2")+"\n";
		s+="vertical_accel = "+DaidalusParameters.val_unit(getVerticalAcceleration(),"m/s^2")+"\n";
		s+="turn_rate = "+DaidalusParameters.val_unit(trk_band_.get_turn_rate(),"deg/s")+"\n";
		s+="bank_angle = "+DaidalusParameters.val_unit(trk_band_.get_bank_angle(),"deg")+"\n";
		s+="vertical_rate = "+DaidalusParameters.val_unit(alt_band_.get_vertical_rate(),"fpm")+"\n";
		s+="# Default Parameters (Recovery Bands)\n";
		s+="recovery_stability_time = "+DaidalusParameters.val_unit(getRecoveryStabilityTime(),"s")+"\n";
		s+="min_horizontal_recovery = "+DaidalusParameters.val_unit(core_.min_horizontal_recovery,"nmi")+
				" ("+f.Fm4(Units.to("nmi",core_.minHorizontalRecovery()))+" [nmi])\n";
		s+="min_vertical_recovery = "+DaidalusParameters.val_unit(core_.min_vertical_recovery,"ft")+
				" ("+f.Fm4(Units.to("ft",core_.minVerticalRecovery()))+" [ft])\n";
		s+="most_urgent_ac = "+core_.most_urgent_ac.getId()+"\n";
		s+="conflict_crit = "+core_.conflict_crit+"\n";
		s+="recovery_crit = "+core_.recovery_crit+"\n";
		s+="recovery_trk = "+trk_band_.get_recovery()+"\n";
		s+="recovery_gs = "+gs_band_.get_recovery()+"\n";
		s+="recovery_vs = "+vs_band_.get_recovery()+"\n";
		s+="recovery_alt = "+alt_band_.get_recovery()+"\n";
		s+="#\n";
		s+="NAME sx sy sz vx vy vz time\n";
		s+="[none] [m] [m] [m] [m/s] [m/s] [m/s] [s]\n";
		if (hasOwnship()) {
			s+=core_.ownship.getId()+", "+core_.ownship.get_s().formatXYZ(precision,"",", ","")+
					", "+core_.own_v().formatXYZ(precision,"",", ","")+", 0\n";
		}
		if (hasTraffic()) {
			for (int i = 0; i < core_.traffic.size(); i++) {
				s+=core_.traffic.get(i).getId()+", "+core_.traffic_s(i).formatXYZ(precision,"",", ","")+
						", "+core_.traffic_v(i).formatXYZ(precision,"",", ","")+", 0\n";
			}
		}
		s+="Track bands [rad,rad]:\n"+trk_band_.toString()+"\n";
		s+="Ground speed bands [m/s,m/s]:\n"+gs_band_.toString()+"\n";
		s+="Vertical speed bands [m/s,m/s]:\n"+vs_band_.toString()+"\n";
		s+="Altitude Bands [m,m]:\n"+alt_band_.toString()+"\n";
		return s;
	}

	public String outputString() {
		String s="";
		s+="Ownship Aircraft: "+core_.ownship.getId()+"\n";
		s+="Traffic Aircraft: "+TrafficState.listToString(core_.traffic)+"\n";
		s+="Conflict Criteria: "+(core_.conflict_crit?"Enabled":"Disabled")+"\n";
		s+="Recovery Criteria: "+(core_.recovery_crit?"Enabled":"Disabled")+"\n";
		s+="Most Urgent Aircraft: "+core_.most_urgent_ac.getId()+"\n";
		s+="Horizontal Epsilon: "+core_.epsilonH()+"\n";
		s+="Vertical Epsilon: "+core_.epsilonV()+"\n";
		for (int alert_level=1; alert_level <= core_.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Conflict Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(conflictAircraft(alert_level))+"\n";
		}
		// Track
		double val = core_.ownship.track("deg");
		s+="Ownship Track: "+f.Fm1(val)+" [deg]\n";
		s+="Region of Current Track: "+regionOfTrack(val,"deg").toString()+"\n";
		s+="Track Bands [deg,deg]:\n"; 
		for (int i=0; i < trackLength(); ++i) {
			s+="  "+track(i, "deg").toString(1)+" "+trackRegion(i)+"\n";
		} 
		for (int alert_level=1; alert_level <= core_.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Peripheral Track Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(peripheralTrackAircraft(alert_level))+"\n";
		}
		s+="Track Resolution (right): "+f.Fm2(trackResolution(true,"deg"))+" [deg]\n";
		s+="Track Resolution (left): "+f.Fm2(trackResolution(false,"deg"))+" [deg]\n";
		s+="Time to Track Recovery: "+f.Fm2(timeToTrackRecovery())+" [s]\n";

		// Ground Speed
		val = core_.ownship.groundSpeed("knot");
		s+="Ownship Ground Speed: "+f.Fm1(val)+" [knot]\n";
		s+="Region of Current Ground Speed: "+regionOfGroundSpeed(val,"knot").toString()+"\n";
		s+="Ground Speed Bands [knot,knot]:\n";
		for (int i=0; i < groundSpeedLength(); ++i) {
			s+="  "+groundSpeed(i, "kn").toString(1)+" "+groundSpeedRegion(i)+"\n";
		} 
		for (int alert_level=1; alert_level <= core_.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Peripheral Ground Speed Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(peripheralGroundSpeedAircraft(alert_level))+"\n";
		}
		s+="Ground Speed Resolution (up): "+f.Fm2(groundSpeedResolution(true,"kn"))+" [kn]\n";
		s+="Ground Speed Resolution (down): "+f.Fm2(groundSpeedResolution(false,"kn"))+" [kn]\n";
		s+="Time to Ground Speed Recovery: "+f.Fm2(timeToGroundSpeedRecovery())+" [s]\n";

		// Vertical Speed
		val = core_.ownship.verticalSpeed("fpm");
		s+="Ownship Vertical Speed: "+f.Fm1(val)+" [fpm]\n";
		s+="Region of Current Vertical Speed: "+regionOfVerticalSpeed(val,"fpm").toString()+"\n";
		s+="Vertical Speed Bands [fpm,fpm]:\n";
		for (int i=0; i < verticalSpeedLength(); ++i) {
			s+="  "+verticalSpeed(i, "fpm").toString(1)+" "+ verticalSpeedRegion(i)+"\n";
		} 
		for (int alert_level=1; alert_level <= core_.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Peripheral Vertical Speed Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(peripheralVerticalSpeedAircraft(alert_level))+"\n";
		}
		s+="Vertical Speed Resolution (up): "+f.Fm2(verticalSpeedResolution(true,"fpm"))+" [fpm]\n";
		s+="Vertical Speed Resolution (down): "+f.Fm2(verticalSpeedResolution(false,"fpm"))+" [fpm]\n";
		s+="Time to Vertical Speed Recovery: "+f.Fm2(timeToVerticalSpeedRecovery())+" [s]\n";

		// Altitude
		val = core_.ownship.altitude("ft");
		s+="Ownship Altitude: "+f.Fm1(val)+" [ft]\n";
		s+="Region of Current Altitude: "+regionOfAltitude(val,"ft").toString()+"\n";
		s+="Altitude Bands [ft,ft]:\n";
		for (int i=0; i < altitudeLength(); ++i) {
			s+="  "+altitude(i, "ft").toString(1)+" "+ altitudeRegion(i)+"\n";
		} 
		for (int alert_level=1; alert_level <= core_.alertor.mostSevereAlertLevel(); ++alert_level) {
			s+="Peripheral Altitude Aircraft (alert level "+alert_level+"): "+
					TrafficState.listToString(peripheralAltitudeAircraft(alert_level))+"\n";
		}
		s+="Altitude Resolution (up): "+f.Fm2(altitudeResolution(true,"ft"))+" [ft]\n";
		s+="Altitude Resolution (down): "+f.Fm2(altitudeResolution(false,"ft"))+" [ft]\n";
		s+="Time to Altitude Recovery: "+f.Fm2(timeToAltitudeRecovery())+" [s]\n";

		// Last Time to Maneuver
		for (TrafficState ac : core_.traffic) {
			s+="Last Times to Maneuver with Respect to "+ac.getId()+"\n";
			s+="  Track Maneuver: "+f.Fm2(lastTimeToTrackManeuver(ac))+" [s]\n";
			s+="  Ground Speed Maneuver: "+f.Fm2(lastTimeToGroundSpeedManeuver(ac))+" [s]\n";
			s+="  Vertical Speed Maneuver: "+f.Fm2(lastTimeToVerticalSpeedManeuver(ac))+" [s]\n";
			s+="  Altitude Maneuver: "+f.Fm2(lastTimeToAltitudeManeuver(ac))+" [s]\n";
		}
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
    return core_.lookahead_time;
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
    core_.lookahead_time = t;
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
