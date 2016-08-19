/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

public class AlertThresholds {

	private Detection3D detector_; // State-based detector  
	private double alerting_time_; // Alerting_time
	// If alerting_time > 0, alert is based on detection
	// If alerting_time = 0, alert is based on violation
	private double late_alerting_time_; // Late alerting time (for maneuver guidance). If zero, same as alerting_time
	private BandsRegion region_;  // Guidance region for this alert
	private double spread_trk_; // Alert when track band within spread (non-negative value)
	private double spread_gs_;  // Alert when ground speed band within spread (non-negative value)
	private double spread_vs_;  // Alert when vertical speed band within speed (non-negative value)
	private double spread_alt_; // Alert when altitude  band within spread (non-negative value)

	/** 
	 * Creates an alert threholds object. Parameter det is a detector,
	 * alerting_time is a non-negative alerting time (possibly positive infinity),
	 * late_alerting_time is a late alerting time >= at (for maneuver guidance),
	 * region is the type of guidance
	 */
	public AlertThresholds(Detection3D detector, double alerting_time, double late_alerting_time, 
			BandsRegion region) {
		detector_ = detector.copy();
		alerting_time_ = Math.abs(alerting_time);
		late_alerting_time_ = Math.max(alerting_time_,late_alerting_time);
		region_ = region;
		spread_trk_ = 0;
		spread_gs_ = 0;
		spread_vs_ = 0;
		spread_alt_ = 0;
	}

	public AlertThresholds(AlertThresholds athr) {
		detector_ = athr.detector_.copy();
		alerting_time_ = athr.alerting_time_;
		late_alerting_time_ = athr.late_alerting_time_;
		region_ = athr.region_;
		spread_trk_ = athr.spread_trk_;
		spread_gs_ = athr.spread_gs_;
		spread_vs_ = athr.spread_vs_;
		spread_alt_ = athr.spread_alt_;    
	}

	private AlertThresholds() {
		detector_ = null;
		alerting_time_ = 0;
		late_alerting_time_ = 0;
		region_ = BandsRegion.UNKNOWN;
		spread_trk_ = 0;
		spread_gs_ = 0;
		spread_vs_ = 0;
		spread_alt_ = 0;
	}

	public static final AlertThresholds INVALID = new AlertThresholds();

	public boolean isValid() {
		return detector_ != null && region_ != BandsRegion.UNKNOWN;
	}

	/**
	 * Return detector.
	 */
	public Detection3D getDetector() {
		return detector_;
	}

	/**
	 * Set detector.
	 */
	public void setDetector(Detection3D det) {
		this.detector_ = det.copy();
	}

	/**
	 * Return alerting time in seconds.
	 */
	public double getAlertingTime() {
		return alerting_time_;
	}

	/**
	 * Set alerting time in seconds. Alerting time is non-negative number.
	 */
	public void setAlertingTime(double t) {
		alerting_time_ = Math.abs(t);
	}

	/**
	 * Return late alerting time in seconds.
	 */
	public double getLateAlertingTime() {
		return late_alerting_time_;
	}

	/**
	 * Set late alerting time in seconds. Late alerting time is a positive number >= alerting time
	 */
	public void setLateAlertingTime(double t) {
		late_alerting_time_ = Math.abs(t);
	}

	/**
	 * Return guidance region.
	 */
	public BandsRegion getRegion() {
		return region_;
	}

	/** 
	 * Set guidance region.
	 */
	public void setRegion(BandsRegion region) {
		region_ = region;
	}

	/**
	 * Get track spread in internal units [rad]. Spread is relative to ownship's track
	 */
	public double getTrackSpread() {
		return spread_trk_;
	}

	/**
	 * Get track spread in given units [u]. Spread is relative to ownship's track
	 */
	public double getTrackSpread(String u) {
		return Units.to(u,getTrackSpread());
	}  

	/** 
	 * Set track spread in internal units. Spread is relative to ownship's track and is expected 
	 * to be in [0,pi].
	 */
	public void setTrackSpread(double spread) {
		spread_trk_ = Math.abs(Util.to_pi(spread));
	}

	/** 
	 * Set track spread in given units. Spread is relative to ownship's track and is expected 
	 * to be in [0,pi] [u].
	 */
	public void setTrackSpread(double spread, String u) {
		setTrackSpread(Units.from(u,spread));
	}

	/**
	 * Get ground speed spread in internal units [m/s]. Spread is relative to ownship's ground speed
	 */
	public double getGroundSpeedSpread() {
		return spread_gs_;
	}

	/**
	 * Get ground speed spread in given units. Spread is relative to ownship's ground speed
	 */
	public double getGroundSpeedSpread(String u) {
		return Units.to(u,getGroundSpeedSpread());
	}  

	/** 
	 * Set ground speed spread in internal units [m/s]. Spread is relative to ownship's ground speed and is expected 
	 * to be non-negative
	 */
	public void setGroundSpeedSpread(double spread) {
		spread_gs_ = Math.abs(spread);
	}

	/** 
	 * Set ground speed spread in given units. Spread is relative to ownship's ground speed and is expected 
	 * to be non-negative
	 */
	public void setGroundSpeedSpread(double spread, String u) {
		setGroundSpeedSpread(Units.from(u,spread));
	}

	/**
	 * Get vertical speed spread in internal units [m/s]. Spread is relative to ownship's vertical speed
	 */
	public double getVerticalSpeedSpread() {
		return spread_vs_;
	}

	/**
	 * Get vertical speed spread in given units. Spread is relative to ownship's vertical speed
	 */
	public double getVerticalSpeedSpread(String u) {
		return Units.to(u,getVerticalSpeedSpread());
	}  

	/** 
	 * Set vertical speed spread in internal units [m/s]. Spread is relative to ownship's vertical speed and is expected 
	 * to be non-negative
	 */
	public void setVerticalSpeedSpread(double spread) {
		spread_vs_ = Math.abs(spread);
	}

	/** 
	 * Set vertical speed spread in given units. Spread is relative to ownship's vertical speed and is expected 
	 * to be non-negative
	 */
	public void setVerticalSpeedSpread(double spread, String u) {
		setVerticalSpeedSpread(Units.from(u,spread));
	}

	/**
	 * Get altitude spread in internal units [m]. Spread is relative to ownship's altitude
	 */
	public double getAltitudeSpread() {
		return spread_alt_;
	}

	/**
	 * Get altitude spread in given units. Spread is relative to ownship's altitude
	 */
	public double getAltitudeSpread(String u) {
		return Units.to(u,getAltitudeSpread());
	}  

	/** 
	 * Set altitude spread in internal units [m]. Spread is relative to ownship's altitude and is expected 
	 * to be non-negative
	 */
	public void setAltitudeSpread(double spread) {
		spread_alt_ = Math.abs(spread);
	}

	/** 
	 * Set altitude spread in given units. Spread is relative to ownship's altitude and is expected 
	 * to be non-negative
	 */
	public void setAltitudeSpread(double spread, String u) {
		setAltitudeSpread(Units.from(u,spread));
	}

	/** 
	 * Return true if and only if threshold values, defining an alerting level, are violated.
	 */ 
	public boolean alerting(DaidalusParameters parameters, TrafficState own, TrafficState ac, 
			TrafficState mu_ac, int turning, int accelerating, int climbing) {
		if (isValid()) {
			Vect3 so = own.get_s();
			Velocity vo = own.get_v();
			Vect3 si = ac.get_s();
			Velocity vi = ac.get_v();
			if (detector_.violation(so,vo,si,vi)) {
				return true;
			}
			ConflictData det = detector_.conflictDetection(so,vo,si,vi,0,alerting_time_);
			if (det.conflict()) {
				return true;
			}
			if (spread_trk_ > 0 || spread_gs_ > 0 || spread_vs_ > 0 || spread_alt_ > 0) {
				KinematicBandsCore core = new KinematicBandsCore(); 
				core.setParameters(parameters);
				core.ownship = own;
				core.most_urgent_ac = mu_ac;
				if (spread_trk_ > 0) {
					KinematicTrkBands trk_band = new KinematicTrkBands();
					trk_band.set_rel(true);
					trk_band.set_min(turning <= 0 ? -spread_trk_ : 0);
					trk_band.set_max(turning >= 0 ? spread_trk_ : 0);
					trk_band.set_step(parameters.getTrackStep());  
					trk_band.set_turn_rate(parameters.getTurnRate()); 
					trk_band.set_bank_angle(parameters.getBankAngle()); 
					if (trk_band.kinematic_conflict(core,ac,detector_,alerting_time_)) {
						return true;
					}
				}
				if (spread_gs_ > 0) {
					KinematicGsBands gs_band = new KinematicGsBands();
					gs_band.set_rel(true);
					gs_band.set_min(accelerating <= 0 ? -spread_gs_ : 0);
					gs_band.set_max(accelerating >= 0 ? spread_gs_ : 0);
					gs_band.set_step(parameters.getGroundSpeedStep());
					gs_band.set_horizontal_accel(parameters.getHorizontalAcceleration()); 
					if (gs_band.kinematic_conflict(core,ac,detector_,alerting_time_)) {
						return true;
					}
				}
				if (spread_vs_ > 0) {
					KinematicVsBands vs_band = new KinematicVsBands();
					vs_band.set_rel(true);
					vs_band.set_min(climbing <= 0 ? -spread_vs_ : 0);
					vs_band.set_max(climbing >= 0 ? spread_vs_ : 0);
					vs_band.set_step(parameters.getVerticalSpeedStep()); 
					vs_band.set_vertical_accel(parameters.getVerticalAcceleration()); 
					if (vs_band.kinematic_conflict(core,ac,detector_,alerting_time_)) {
						return true;
					}
				}
				if (spread_alt_ > 0) {
					KinematicAltBands alt_band = new KinematicAltBands();
					alt_band.set_rel(true);
					alt_band.set_min(climbing <= 0 ? -spread_alt_ : 0);
					alt_band.set_max(climbing >= 0 ? spread_alt_ : 0);
					alt_band.set_step(parameters.getAltitudeStep()); 
					alt_band.set_vertical_rate(parameters.getVerticalRate()); 
					alt_band.set_vertical_accel(parameters.getVerticalAcceleration()); 
					if (alt_band.kinematic_conflict(core,ac,detector_,alerting_time_)) {
						return true;
					}
				}
			}
		}
		return false;
	}

	public String toString() {
		return detector_.toString()+", Alerting Time: "+f.Fm1(alerting_time_)+
				" [s], Late Alerting Time: "+f.Fm1(late_alerting_time_)+
				" [s], Region: "+region_.toString()+
				", Track Spread: "+f.Fm1(Units.to("deg",spread_trk_))+
				" [deg] , Ground Speed Spread: "+f.Fm1(Units.to("knot",spread_gs_))+
				" [knot], Vertical Speed Spread: "+f.Fm1(Units.to("fpm",spread_vs_))+
				" [fpm], Altitude Spread: "+f.Fm1(Units.to("ft",spread_alt_))+
				" [ft]";
	}

	public String toPVS(int prec) {
		return "(# wcv:= "+detector_.toPVS(prec)+
				", alerting_time:= "+f.Fm1(alerting_time_)+
				", late_alerting_time:= "+f.Fm1(late_alerting_time_)+
				", region:= "+region_.toPVS()+
				", spread_trk:= ("+f.FmPrecision(spread_trk_,prec)+","+f.FmPrecision(spread_trk_,prec)+")"+
				", spread_gs:= ("+f.FmPrecision(spread_gs_,prec)+","+f.FmPrecision(spread_gs_,prec)+")"+
				", spread_vs:= ("+f.FmPrecision(spread_vs_,prec)+","+f.FmPrecision(spread_vs_,prec)+")"+
				", spread_alt:= ("+f.FmPrecision(spread_alt_,prec)+","+f.FmPrecision(spread_alt_,prec)+")"+
				" #)"; 
	}

}
