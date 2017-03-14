/*
 * ================================================================================
 * Daidalus : This class implements the interface to DAIDALUS (Detect and Avoid 
 * Alerting Logic for Unmanned Systems). 
 * 
 * Contact:  Cesar Munoz               NASA Langley Research Center
 *           
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 * ==================================================================================
 */

package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.List;
import java.util.Optional;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.LossData;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class Daidalus implements ErrorReporter {

	private TrafficState ownship_; // Ownship aircraft. Velocity vector is wind-based.
	private List<TrafficState> traffic_; // Traffic aircraft states. Positions are synchronized in time with ownship. Velocity vector is wind-based.  
	private double current_time_; // Current time
	private Velocity wind_vector_; // Wind information
	private UrgencyStrategy urgency_strat_; // Strategy for most urgent aircraft
	private ErrorLog error = new ErrorLog("Daidalus");

	/**
	 * Parameter values for Daidalus object
	 */
	public KinematicBandsParameters parameters; // Parameters

	/**
	 * Create a new Daidalus object such that
	 * - Alerting thresholds are unbuffered as defined by SC-228 MOPS.
	 * - Maneuver guidance logic assumes instantaneous maneuvers
	 * - Bands saturate at DMOD/ZTHR
	 */
	public Daidalus() {
		parameters = new KinematicBandsParameters();
		urgency_strat_ = new NoneUrgencyStrategy(); 
		wind_vector_ = Velocity.ZERO;
		current_time_ = 0;
		ownship_ = TrafficState.INVALID;
		traffic_ = new ArrayList<TrafficState>();
		set_WC_SC_228_MOPS();
	}

	/**
	 * Create a new Daidalus object and copy all configuration parameters and 
	 * traffic information from another Daidalus object.
	 */
	public Daidalus(Daidalus daa) {
		parameters = new KinematicBandsParameters(daa.parameters);
		urgency_strat_ = daa.urgency_strat_.copy();
		wind_vector_ = daa.wind_vector_;
		current_time_ = daa.current_time_;
		ownship_ = daa.ownship_;
		traffic_ = new ArrayList<TrafficState>();
		traffic_.addAll(daa.traffic_);
	}

	/*  
	 * Set Daidalus object such that 
	 * - Alerting thresholds are unbuffered as defined by SC-228 MOPS.
	 * - Maneuver guidance logic assumes instantaneous maneuvers
	 * - Bands saturate at DMOD/ZTHR
	 */
	public void set_WC_SC_228_MOPS() {
		parameters.alertor = AlertLevels.WC_SC_228_Thresholds();
		parameters.setInstantaneousBands();
		parameters.setCollisionAvoidanceBands(false);
		parameters.setMinHorizontalRecovery(0.66,"nmi");
		parameters.setMinVerticalRecovery(450,"ft");
	}

	/*  
	 * Set DAIDALUS object such that 
	 * - Alerting thresholds are buffered 
	 * - Maneuver guidance logic assumes kinematic maneuvers
	 * - Turn rate is set to 3 deg/s, when type is true, and to  1.5 deg/s
	 *   when type is false.
	 * - Bands don't saturate until NMAC
	 */
	public void set_Buffered_WC_SC_228_MOPS(boolean type) {
		parameters.alertor = AlertLevels.Buffered_WC_SC_228_Thresholds();
		parameters.setKinematicBands(type);
		parameters.setCollisionAvoidanceBands(true);
		parameters.setCollisionAvoidanceBandsFactor(0.2);
		parameters.setMinHorizontalRecovery(1.0,"nmi");
		parameters.setMinVerticalRecovery(450,"ft");
	}

	/**
	 * Clear aircraft list, current time, and wind vector.
	 */
	public void reset() {
		ownship_ = TrafficState.INVALID;
		traffic_.clear();
		wind_vector_ = Velocity.ZERO;
		current_time_ = 0;
	}

	/**
	 * @return number of aircraft, including ownship.
	 */
	public int numberOfAircraft() {
		if (!ownship_.isValid()) {
			return 0;
		} else {
			return traffic_.size()+1;
		}
	}

	/**
	 * @return last traffic index. Every traffic aircraft has an index between 1 and lastTrafficIndex. 
	 * The index 0 is reserved for the ownship. When lastTrafficIndex is 0, the ownship is set but no
	 * traffic aircraft has been set. When lastTrafficIndex is negative, ownship has not been set.
	 */
	public int lastTrafficIndex() {
		if (!ownship_.isValid()) {
			return -1; 
		} else {
			return traffic_.size();
		}
	}

	/**
	 * Get wind vector
	 */
	public Velocity getWindField() {
		return wind_vector_;
	}

	/**
	 * Set wind vector (common to all aircraft)
	 */
	public void setWindField(Velocity wind) {
		if (lastTrafficIndex() >= 0) {
			Velocity delta_wind = wind_vector_.Sub(wind);
			ownship_ = TrafficState.makeOwnship(ownship_.getId(),ownship_.getPosition(),
					ownship_.getVelocity().Add(delta_wind),ownship_.getTime());
			for (int i=0; i < traffic_.size(); ++i) {
				TrafficState ac = traffic_.get(i);
				traffic_.set(i,ownship_.makeIntruder(ac.getId(),ac.getPosition(),
						ac.getVelocity().Add(delta_wind)));
			}
		}
		wind_vector_ = wind;
	}

	/**
	 * Clear all aircraft and set ownship state and current time. 
	 * Velocity vector is ground velocity.
	 */
	public void setOwnshipState(String id, Position pos, Velocity vel, double time) {
		traffic_.clear();
		ownship_ = TrafficState.makeOwnship(id,pos,vel.Sub(wind_vector_),time);
		current_time_ = time;
	}

	/**
	 * Clear all aircraft and set ownship state and current time. 
	 */
	public void setOwnshipState(TrafficState ownship, double time) {
		setOwnshipState(ownship.getId(),ownship.getPosition(),ownship.getVelocity(),
				time);
	}

	/**
	 * Add traffic state at given time. Velocity vector is ground velocity.
	 * If time is different from current time, traffic state is projected, past or future, 
	 * into current time assuming wind information. If it's the first aircraft, this aircraft is 
	 * set as the ownship. Return aircraft index.
	 */
	public int addTrafficState(String id, Position pos, Velocity vel, double time) {
		if (lastTrafficIndex() < 0) {
			setOwnshipState(id,pos,vel,time);
			return 0;
		} else {
			double dt = current_time_-time;
			Position pt = dt == 0 ? pos : pos.linear(vel,dt);    
			TrafficState ac = ownship_.makeIntruder(id,pt,vel.Sub(wind_vector_));
			if (ac.isValid()) {
				traffic_.add(ac);
				return traffic_.size();
			} else {
				return -1;
			}
		}
	}

	/**
	 * Add traffic state at current time. Velocity vector is ground velocity. 
	 * If it's the first aircraft, this aircraft is set as the ownship. 
	 * Return aircraft index.
	 */
	public int addTrafficState(String id, Position pos, Velocity vel) {
		return addTrafficState(id,pos,vel,current_time_);
	}

	/**
	 * Add traffic state at given time. 
	 * If time is different from current time, traffic state is projected, past or future, 
	 * into current time assuming wind information. If it's the first aircraft, this aircraft is 
	 * set as the ownship. Return aircraft index.
	 */
	public int addTrafficState(TrafficState ac, double time) {
		return addTrafficState(ac.getId(),ac.getPosition(),ac.getVelocity(),time);
	}

	/**
	 * Add traffic state at current time. 
	 * If it's the first aircraft, this aircraft is set as the ownship. 
	 * Return aircraft index.
	 */
	public int addTrafficState(TrafficState ac) {
		return addTrafficState(ac.getId(),ac.getPosition(),ac.getVelocity());
	}

	/**
	 * Exchange ownship aircraft with aircraft at index ac_idx.
	 */
	public void resetOwnship(int ac_idx) {
		if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
			int ac = ac_idx-1;
			TrafficState new_own = TrafficState.makeOwnship(traffic_.get(ac));
			TrafficState old_own = new_own.makeIntruder(ownship_);
			ownship_ = new_own;
			for (int i = 0; i < traffic_.size(); ++i) {
				if (i == ac) {
					traffic_.set(i,old_own);
				} else {
					traffic_.set(i,ownship_.makeIntruder(traffic_.get(i)));
				}
			}
		} else {
			error.addError("resetOwnship: aircraft index "+ac_idx+" is out of bounds");
		}
	}

	/**
	 * Exchange ownship aircraft with aircraft named id.
	 */
	public void resetOwnship(String id) {
		resetOwnship(aircraftIndex(id));
	}

	/** 
	 * Get index of aircraft with given name. Return -1 if no such index exists
	 */
	public int aircraftIndex(String name) {
		if (lastTrafficIndex() >= 0) {
			if (ownship_.getId().equals(name)) {
				return 0;
			}
			for (int i = 0; i < traffic_.size(); ++i) {
				if (traffic_.get(i).getId().equals(name))
					return i+1;
			}
		}
		return -1;
	}

	/**
	 * @return get current time, i.e., time of ownship aircraft.
	 */
	public double getCurrentTime() {
		return current_time_;
	}

	/**
	 * Linearly projects all aircraft states to time t [s] and set current time to t.
	 */
	public void setCurrentTime(double time) {
		if (time != current_time_ && lastTrafficIndex() >= 0) {
			double dt = time-current_time_;
			Velocity vo = ownship_.getVelocity().Add(wind_vector_); // Original ground velocity
			Position po = ownship_.getPosition().linear(vo,dt);   
			ownship_ = TrafficState.makeOwnship(ownship_.getId(),po,ownship_.getVelocity(),time);
			for (int i=0; i < traffic_.size(); ++i) {
				TrafficState ac = traffic_.get(i);
				Velocity vi = ac.getVelocity().Add(wind_vector_); // Original ground velocity
				Position pi = ac.getPosition().linear(vi,dt); 
				traffic_.set(i,ownship_.makeIntruder(ac.getId(),pi,ac.getVelocity()));
			}
			current_time_ = time;
		}
	}

	/** 
	 * Computes alerting type of ownship and aircraft at index ac_idx for current 
	 * aircraft states.  The number 0 means no alert. A negative number means
	 * that aircraft index is not valid.
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
	public int alerting(int ac_idx, int turning, int accelerating, int climbing) {
		if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
			TrafficState ac = traffic_.get(ac_idx-1);
			KinematicMultiBands kb = getKinematicMultiBands();
			return kb.alerting(ac, turning, accelerating, climbing);
		} else {
			error.addError("alerting: aircraft index "+ac_idx+" is out of bounds");
			return -1;
		} 
	}

	/** 
	 * Computes alerting type of ownship and aircraft at index ac_idx for current 
	 * aircraft states.  The number 0 means no alert. A negative number means
	 * that aircraft index is not valid.
	 */
	public int alerting(int ac_idx) {
		return alerting(ac_idx,0,0,0);
	}

	/**
	 * Detects conflict with aircraft at index ac_idx for given alert level. 
	 * Conflict data provides time to violation and time to end of violation 
	 * within lookahead time. 
	 */
	public ConflictData detection(int ac_idx, int alert_level) {
		Optional<Detection3D> detector = parameters.alertor.detector(alert_level);
		if (1 <= ac_idx && ac_idx <= lastTrafficIndex() && detector.isPresent()) {
			TrafficState ac = traffic_.get(ac_idx-1);
			return detector.get().conflictDetection(ownship_.get_s(),ownship_.get_v(),ac.get_s(),ac.get_v(),
					0,parameters.getLookaheadTime());
		} else {
			error.addError("detection: aircraft index "+ac_idx+" is out of bounds");
			return new ConflictData();
		}
	}

	/**
	 * Detects conflict with aircraft at index ac_idx for conflict alert level. 
	 * Conflict data provides time to violation and time to end of violation 
	 * within lookahead time. 
	 */
	public ConflictData detection(int ac_idx) {
		return detection(ac_idx,0);
	}

	/**
	 * @return time to violation, in seconds, between ownship and aircraft at index ac_idx, for the
	 * lookahead time. The returned time is relative to current time. POSITIVE_INFINITY means no
	 * conflict within lookahead time. NaN means aircraft index is out of range.
	 */
	public double timeToViolation(int ac_idx) {
		if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
			ConflictData det = detection(ac_idx);
			if (det.conflict()) {
				return det.getTimeIn();
			}
			return Double.POSITIVE_INFINITY;
		} else {
			error.addError("timeToViolation: aircraft index "+ac_idx+" is out of bounds");
			return Double.NaN;
		}
	}

	/**
	 * @return kinematic bands at current time. Computation of bands is lazy, they are only
	 * compute when needed.
	 */
	public KinematicMultiBands getKinematicMultiBands() {   
		if (lastTrafficIndex() < 0) {
			error.addError("getKinematicBands: ownship has not been set");
			return new KinematicMultiBands();
		} else {
			KinematicMultiBands bands = new KinematicMultiBands(parameters); 
			bands.setOwnship(ownship_);
			bands.setTraffic(traffic_);
			bands.setMostUrgentAircraft(mostUrgentAircraft());
			return bands;
		}
	}

	/**
	 * Returns state of ownship.
	 */
	public TrafficState getOwnshipState() {
		return ownship_;
	}

	/**
	 * Returns state of aircraft at index ac_idx 
	 */
	public TrafficState getAircraftState(int ac_idx) {
		if (0 <= ac_idx && ac_idx <= lastTrafficIndex()) {
			if (ac_idx == 0) {
				return ownship_;
			} else {
				return traffic_.get(ac_idx-1);
			}
		} else {
			error.addError("getAircraftState: aircraft index "+ac_idx+" is out of bounds");
			return TrafficState.INVALID;
		}
	}

	/**
	 * @return strategy for computing most urgent aircraft. 
	 */
	public UrgencyStrategy getUrgencyStrategy() { 
		return urgency_strat_;
	}

	/**
	 * Set strategy for computing most urgent aircraft.
	 */
	public void setUrgencyStrategy(UrgencyStrategy strat) { 
		urgency_strat_ = strat;
	}

	/**
	 * Returns most urgent aircraft for given alert level according to urgency strategy.
	 */
	public TrafficState mostUrgentAircraft(int alert_level) {
		Optional<Detection3D> detector = parameters.alertor.detector(alert_level);    
		if (lastTrafficIndex() > 0 && detector.isPresent()) {
			return urgency_strat_.mostUrgentAircraft(detector.get(),ownship_,traffic_,parameters.getLookaheadTime());
		} else {
			return TrafficState.INVALID;
		}
	}

	/**
	 * Returns most urgent aircraft for conflict alert level according to urgency strategy.
	 */
	public TrafficState mostUrgentAircraft() {
		return mostUrgentAircraft(0);
	}

	private static void add_blob(List<List<Position>> blobs, Deque<Position> vin, Deque<Position> vout) {
		if (vin.isEmpty() && vout.isEmpty()) {
			return;
		}
		// Add conflict contour
		List<Position> blob = new ArrayList<Position>(vin);
		blob.addAll(vout);
		blobs.add(blob);
		vin.clear();
		vout.clear();
	}

	/**
	 * Computes horizontal contours contributed by aircraft at index ac_idx, for 
	 * given alert level. A contour is a non-empty list of points in counter-clockwise 
	 * direction representing a polygon.   
	 * @param blobs list of track contours returned by reference.
	 * @param ac_idx is the index of the aircraft used to compute the contours.
	 */
	public void horizontalContours(List<List<Position>>blobs, int ac_idx, int alert_level) {
		Optional<Detection3D> detector = parameters.alertor.detector(alert_level);    
		blobs.clear();
		if (1 <= ac_idx && ac_idx <= lastTrafficIndex() && detector.isPresent()) {
			Deque<Position> vin = new ArrayDeque<Position>();
			Position po = ownship_.getPosition();
			Velocity vo = ownship_.getVelocity();
			TrafficState intruder = traffic_.get(ac_idx-1);
			Vect3 si = intruder.get_s();
			Velocity vi = intruder.get_v();
			double current_trk = vo.trk();
			Deque<Position> vout = new ArrayDeque<Position>();
			/* First step: Computes conflict contour (contour in the current path of the aircraft).
			 * Get contour portion to the right.  If los.getTimeIn() == 0, a 360 degree
			 * contour will be computed. Otherwise, stops at the first non-conflict degree.
			 */
			double right = 0; // Contour conflict limit to the right relative to current track  [0-2pi rad]
			double two_pi = 2*Math.PI;
			for (; right < two_pi; right += parameters.getTrackStep()) {
				Velocity vop = vo.mkTrk(current_trk+right);
				LossData los = detector.get().conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
						0,parameters.getLookaheadTime());
				if ( !los.conflict() ) {
					break;
				}
				if (los.getTimeIn() != 0 ) {
					// if not in los, add position at time in (counter clock-wise)
					vin.addLast(po.linear(vop,los.getTimeIn()));
				}
				// in any case, add position ad time out (counter clock-wise)
				vout.addFirst(po.linear(vop,los.getTimeOut()));
			}
			/* Second step: Compute conflict contour to the left */
			double left = 0;  // Contour conflict limit to the left relative to current track [0-2pi rad]
			if (0 < right && right < two_pi) {
				/* There is a conflict contour, but not a violation */
				for (left = parameters.getTrackStep(); left < two_pi; left += parameters.getTrackStep()) {
					Velocity vop = vo.mkTrk(current_trk-left);
					LossData los = detector.get().conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
							0,parameters.getLookaheadTime());
					if ( !los.conflict() ) {
						break;
					}
					vin.addFirst(po.linear(vop,los.getTimeIn()));
					vout.addLast(po.linear(vop,los.getTimeOut()));
				}
			}
			add_blob(blobs,vin,vout);
			// Third Step: Look for other blobs to the right within track threshold
			if (right < parameters.getHorizontalContourThreshold()) {
				for (; right < two_pi-left; right += parameters.getTrackStep()) {
					Velocity vop = vo.mkTrk(current_trk+right);
					LossData los = detector.get().conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
							0,parameters.getLookaheadTime());
					if (los.conflict()) {
						vin.addLast(po.linear(vop,los.getTimeIn()));
						vout.addFirst(po.linear(vop,los.getTimeOut()));
					} else {
						add_blob(blobs,vin,vout);
						if (right >= parameters.getHorizontalContourThreshold()) {
							break;
						}
					}
				}
				add_blob(blobs,vin,vout);
			}
			// Fourth Step: Look for other blobs to the left within track threshold
			if (left < parameters.getHorizontalContourThreshold()) {
				for (; left < two_pi-right; left += parameters.getTrackStep()) {
					Velocity vop = vo.mkTrk(current_trk-left);
					LossData los = detector.get().conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
							0,parameters.getLookaheadTime());
					if (los.conflict()) {
						vin.addFirst(po.linear(vop,los.getTimeIn()));
						vout.addLast(po.linear(vop,los.getTimeOut()));
					} else {
						add_blob(blobs,vin,vout);
						if (left >= parameters.getHorizontalContourThreshold()) {
							break;
						}
					}
				}
				add_blob(blobs,vin,vout);
			}
		} else {
			error.addError("trackContour: aircraft index "+ac_idx+" is out of bounds");
		}
	}

	/**
	 * Computes horizontal contours contributed by aircraft at index ac_idx, for 
	 * conflict alert level. A contour is a non-empty list of points in counter-clockwise 
	 * direction representing a polygon.   
	 * @param blobs list of track contours returned by reference.
	 * @param ac_idx is the index of the aircraft used to compute the contours.
	 */
	public void horizontalContours(List<List<Position>>blobs, int ac_idx) {
		horizontalContours(blobs,ac_idx,0);
	}

	public String aircraftListToPVS(int prec) {
		return ownship_.listToPVSAircraftList(traffic_,prec);
	}

	public String outputStringAircraftStates() {
		String ualt = parameters.getUnits("alt_step");
		String ugs = parameters.getUnits("gs_step");
		String uvs = parameters.getUnits("vs_step");
		String uxy = "m";
		if (Units.isCompatible(ugs,"knot")) {
			uxy = "nmi";
		} else if (Units.isCompatible(ugs,"fpm")) {
			uxy = "ft";
		} else if (Units.isCompatible(ugs,"kph")) {
			uxy = "km";
		}
		return ownship_.formattedTraffic(traffic_, uxy, ualt, ugs, uvs);
	}

	public String toString() {
		String s;
		s = "Daidalus Object\n";
		s += parameters.toString();
		if (ownship_.isValid()) {
			s += "###\nAircraft States:\n"+outputStringAircraftStates();
		}
		return s;
	}

	public static String release() {
		return "DAIDALUSj V-"+KinematicBandsParameters.VERSION+
				"-FormalATM-"+Constants.version+" (March-18-2017)"; 
	}

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

}
