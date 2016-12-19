/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.f;

public class AlertThresholds {

	private Detection3D detector_; // State-based detector  
	private double alerting_time_; // Alerting_time
	// If alerting_time > 0, alert is based on detection
	// If alerting_time = 0, alert is based on violation
	private double early_alerting_time_; // Early alerting time (for maneuver guidance). If zero, same as alerting_time
	private BandsRegion region_;  // Guidance region for this alert
	private double spread_trk_; // Alert when track band within spread (non-negative value)
	private double spread_gs_;  // Alert when ground speed band within spread (non-negative value)
	private double spread_vs_;  // Alert when vertical speed band within speed (non-negative value)
	private double spread_alt_; // Alert when altitude  band within spread (non-negative value)

	/** 
	 * Creates an alert threholds object. Parameter det is a detector,
	 * alerting_time is a non-negative alerting time (possibly positive infinity),
	 * early_alerting_time is a early alerting time >= at (for maneuver guidance),
	 * region is the type of guidance
	 */
	public AlertThresholds(Detection3D detector, double alerting_time, double early_alerting_time, 
			BandsRegion region) {
		detector_ = detector.copy();
		alerting_time_ = Math.abs(alerting_time);
		early_alerting_time_ = Util.max(alerting_time_,early_alerting_time);
		region_ = region;
		spread_trk_ = 0;
		spread_gs_ = 0;
		spread_vs_ = 0;
		spread_alt_ = 0;
	}

	public AlertThresholds(AlertThresholds athr) {
		detector_ = athr.detector_.copy();
		alerting_time_ = athr.alerting_time_;
		early_alerting_time_ = athr.early_alerting_time_;
		region_ = athr.region_;
		spread_trk_ = athr.spread_trk_;
		spread_gs_ = athr.spread_gs_;
		spread_vs_ = athr.spread_vs_;
		spread_alt_ = athr.spread_alt_;    
	}

	private AlertThresholds() {
		detector_ = null;
		alerting_time_ = 0;
		early_alerting_time_ = 0;
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
	 * Return early alerting time in seconds.
	 */
	public double getEarlyAlertingTime() {
		return early_alerting_time_;
	}

	/**
	 * Set early alerting time in seconds. Early alerting time is a positive number >= alerting time
	 */
	public void setEarlyAlertingTime(double t) {
		early_alerting_time_ = Math.abs(t);
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

	public String toString() {
		return detector_.toString()+", Alerting Time: "+f.Fm1(alerting_time_)+
				" [s], Early Alerting Time: "+f.Fm1(early_alerting_time_)+
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
				", early_alerting_time:= "+f.Fm1(early_alerting_time_)+
				", region:= "+region_+
				", spread_trk:= ("+f.FmPrecision(spread_trk_,prec)+","+f.FmPrecision(spread_trk_,prec)+")"+
				", spread_gs:= ("+f.FmPrecision(spread_gs_,prec)+","+f.FmPrecision(spread_gs_,prec)+")"+
				", spread_vs:= ("+f.FmPrecision(spread_vs_,prec)+","+f.FmPrecision(spread_vs_,prec)+")"+
				", spread_alt:= ("+f.FmPrecision(spread_alt_,prec)+","+f.FmPrecision(spread_alt_,prec)+")"+
				" #)"; 
	}

}
