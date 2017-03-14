/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.HashMap;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

import gov.nasa.larcfm.IO.ConfigReader;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ParameterAcceptor;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.f;

final public class KinematicBandsParameters implements ParameterAcceptor, ErrorReporter {

	/**
	 * DAIDALUS version
	 */
	public static final String VERSION = "1.1";

	/**
	 * Alertor
	 */
	public AlertLevels alertor; // Alert levels

	/**
	 * String indicating the DAIDALUS version
	 */

	protected ErrorLog error = new ErrorLog("DaidalusParameters");

	// Bands
	private double lookahead_time_; // [s] Lookahead time
	private double left_trk_;  // Left track [0 - pi]
	private double right_trk_; // Right track [0 - pi]
	private double min_gs_;    // Minimum ground speed
	private double max_gs_;    // Maximum ground speed
	private double min_vs_;    // Minimum vertical speed 
	private double max_vs_;    // Maximum vertical speed
	private double min_alt_;   // Minimum altitude
	private double max_alt_;   // Maximum altitude

	// Kinematic bands
	private double trk_step_; // Track step
	private double gs_step_;  // Ground speed step
	private double vs_step_;  // Vertical speed step
	private double alt_step_; // Altitude step
	private double horizontal_accel_; // Horizontal acceleration
	private double vertical_accel_; // Vertical acceleration
	private double turn_rate_; // Turn rate
	private double bank_angle_; // Bank angles (only used when turn_rate is 0)
	private double vertical_rate_; // Vertical rate

	// Recovery bands
	private double horizontal_nmac_; // Horizontal Near Mid-Air Collision
	private double vertical_nmac_; // Vertical Near Mid-Air Collision
	private double recovery_stability_time_; // Recovery stability time
	private double min_horizontal_recovery_; // Horizontal distance protected during recovery. TCAS RA DMOD is used this value is 0
	private double min_vertical_recovery_; // Vertical distance protected during recovery. TCAS RA ZTHR is used when this value is 0
	private boolean conflict_crit_; /* Use criteria for conflict bands */
	private boolean recovery_crit_; /* Use criteria for recovery bands */ 
	/* Compute recovery bands */
	private boolean recovery_trk_; 
	private boolean recovery_gs_; 
	private boolean recovery_vs_; 
	private boolean recovery_alt_; 
	/* Compute collision avoidance bands */
	private boolean ca_bands_;  // When true, compute recovery bands until NMAC
	private double  ca_factor_; // Reduction factor when computing CA bands. It;s a value in [0,1]
	// Contours
	private double contour_thr_; // Horizontal threshold, specified as an angle to the left/right of current aircraft direction,
	// for computing horizontal contours. A value of 0 means only conflict contours. A value of pi means all contours.

	private HashMap<String,String> units_;
	private List<String> keys_;  // List of key values 

	/* NOTE: By default, no alert levels are configured */
	public KinematicBandsParameters() {

		units_ = new HashMap<String,String>();
		keys_ = new ArrayList<String>();

		// Bands Parameters
		lookahead_time_ = 180.0; // [s]
		keys_.add("lookahead_time");
		units_.put("lookahead_time","s");

		left_trk_  = Units.from("deg",180.0); 
		keys_.add("left_trk");
		units_.put("left_trk","deg");

		right_trk_ = Units.from("deg",180.0);
		keys_.add("right_trk");
		units_.put("right_trk","deg");

		min_gs_  = Units.from("knot",10.0);  
		keys_.add("min_gs");
		units_.put("min_gs","knot");

		max_gs_  = Units.from("knot",700.0);
		keys_.add("max_gs");
		units_.put("max_gs","knot");

		min_vs_  = Units.from("fpm",-6000.0);
		keys_.add("min_vs");
		units_.put("min_vs","fpm");

		max_vs_  = Units.from("fpm",6000.0); 
		keys_.add("max_vs");
		units_.put("max_vs","fpm");

		min_alt_ = Units.from("ft",100.0);  
		keys_.add("min_alt");
		units_.put("min_alt","ft");

		max_alt_ = Units.from("ft",50000.0);
		keys_.add("max_alt");
		units_.put("max_alt","ft");

		// Kinematic Parameters
		trk_step_ = Units.from("deg",1.0); 
		keys_.add("trk_step");
		units_.put("trk_step","deg");

		gs_step_ = Units.from("knot",5.0); 
		keys_.add("gs_step");
		units_.put("gs_step","knot");

		vs_step_ = Units.from("fpm",100.0); 
		keys_.add("vs_step");
		units_.put("vs_step","fpm");

		alt_step_ = Units.from("ft", 100.0); 
		keys_.add("alt_step");
		units_.put("alt_step","ft");

		horizontal_accel_ = Units.from("m/s^2",2.0); 
		keys_.add("horizontal_accel");
		units_.put("horizontal_accel","m/s^2");

		vertical_accel_ = Units.from("G",0.25);    // Section 1.2.3, DAA MOPS V3.6
		keys_.add("vertical_accel");
		units_.put("vertical_accel","G");

		turn_rate_ = Units.from("deg/s",3.0); // Section 1.2.3, DAA MOPS V3.6
		keys_.add("turn_rate");
		units_.put("turn_rate","deg/s");

		bank_angle_ = 0.0;    
		keys_.add("bank_angle");
		units_.put("bank_angle","deg");

		vertical_rate_ = Units.from("fpm",500.0);   // Section 1.2.3, DAA MOPS V3.6                     
		keys_.add("vertical_rate");
		units_.put("vertical_rate","fpm");

		// Recovery Bands Parameters
		recovery_stability_time_ = 2.0; // [s] 
		keys_.add("recovery_stability_time");
		units_.put("recovery_stability_time","s");

		min_horizontal_recovery_ = 0.0; 
		keys_.add("min_horizontal_recovery");
		units_.put("min_horizontal_recovery","nmi");

		min_vertical_recovery_ = 0.0; 
		keys_.add("min_vertical_recovery");
		units_.put("min_vertical_recovery","ft");

		recovery_trk_ = true; 
		keys_.add("recovery_trk");

		recovery_gs_ = true; 
		keys_.add("recovery_gs");

		recovery_vs_ = true; 
		keys_.add("recovery_vs");

		recovery_alt_ = true; 
		keys_.add("recovery_alt");

		// Collision Avoidance Bands Parameters
		ca_bands_ = false; 
		keys_.add("ca_bands");

		ca_factor_ = 0.2;
		keys_.add("ca_factor");

		horizontal_nmac_ = ACCoRDConfig.NMAC_D;      // Defined in RTCA SC-147
		keys_.add("horizontal_nmac");
		units_.put("horizontal_nmac","ft");

		vertical_nmac_ = ACCoRDConfig.NMAC_H;        // Defined in RTCA SC-147
		keys_.add("vertical_nmac");
		units_.put("vertical_nmac","ft");

		// Implicit Coordination Parameters
		conflict_crit_ = false;
		keys_.add("conflict_crit");

		recovery_crit_ = false; 
		keys_.add("recovery_crit");

		// Horizontal Contour Threshold
		contour_thr_ = Units.from("deg",180.0);
		keys_.add("contour_thr");
		units_.put("contour_thr","deg");

		// Alert Levels
		alertor = new AlertLevels();
	}

	public KinematicBandsParameters(KinematicBandsParameters parameters) {
		keys_ = new ArrayList<String>();
		keys_.addAll(parameters.keys_);
		units_ = new HashMap<String,String>();
		alertor = new AlertLevels(); 
		setKinematicBandsParameters(parameters);
	}

	/**
	 * Set kinematic bands parameters
	 */
	public void setKinematicBandsParameters(KinematicBandsParameters parameters) {
		units_.putAll(parameters.units_);

		// Bands
		lookahead_time_ = parameters.lookahead_time_;
		left_trk_ = parameters.left_trk_;
		right_trk_ = parameters.right_trk_;
		min_gs_ = parameters.min_gs_;   
		max_gs_ = parameters.max_gs_;   
		min_vs_ = parameters.min_vs_;   
		max_vs_ = parameters.max_vs_;   
		min_alt_ = parameters.min_alt_; 
		max_alt_ = parameters.max_alt_; 

		// Kinematic bands
		trk_step_         = parameters.trk_step_;  
		gs_step_          = parameters.gs_step_;
		vs_step_          = parameters.vs_step_; 
		alt_step_         = parameters.alt_step_; 
		horizontal_accel_ = parameters.horizontal_accel_; 
		vertical_accel_   = parameters.vertical_accel_; 
		turn_rate_        = parameters.turn_rate_; 
		bank_angle_       = parameters.bank_angle_; 
		vertical_rate_    = parameters.vertical_rate_; 

		// Recovery bands
		horizontal_nmac_         = parameters.horizontal_nmac_;
		vertical_nmac_           = parameters.vertical_nmac_;
		recovery_stability_time_ = parameters.recovery_stability_time_; 
		min_horizontal_recovery_ = parameters.min_horizontal_recovery_;
		min_vertical_recovery_   = parameters.min_vertical_recovery_;
		conflict_crit_           = parameters.conflict_crit_;
		recovery_crit_           = parameters.recovery_crit_; 
		recovery_trk_            = parameters.recovery_trk_;
		recovery_gs_             = parameters.recovery_gs_;
		recovery_vs_             = parameters.recovery_vs_;
		recovery_alt_            = parameters.recovery_alt_;
		ca_bands_                = parameters.ca_bands_;
		ca_factor_               = parameters.ca_factor_;

		// Contours
		contour_thr_ = parameters.contour_thr_;	

		// Alert levels
		// Bands
		lookahead_time_ = parameters.lookahead_time_;
		left_trk_ = parameters.left_trk_;
		right_trk_ = parameters.right_trk_;
		min_gs_ = parameters.min_gs_;   
		max_gs_ = parameters.max_gs_;   
		min_vs_ = parameters.min_vs_;   
		max_vs_ = parameters.max_vs_;   
		min_alt_ = parameters.min_alt_; 
		max_alt_ = parameters.max_alt_; 

		// Kinematic bands
		trk_step_         = parameters.trk_step_;  
		gs_step_          = parameters.gs_step_;
		vs_step_          = parameters.vs_step_; 
		alt_step_         = parameters.alt_step_; 
		horizontal_accel_ = parameters.horizontal_accel_; 
		vertical_accel_   = parameters.vertical_accel_; 
		turn_rate_        = parameters.turn_rate_; 
		bank_angle_       = parameters.bank_angle_; 
		vertical_rate_    = parameters.vertical_rate_; 

		// Recovery bands
		horizontal_nmac_         = parameters.horizontal_nmac_;
		vertical_nmac_           = parameters.vertical_nmac_;
		recovery_stability_time_ = parameters.recovery_stability_time_; 
		min_horizontal_recovery_ = parameters.min_horizontal_recovery_;
		min_vertical_recovery_   = parameters.min_vertical_recovery_;
		conflict_crit_           = parameters.conflict_crit_;
		recovery_crit_           = parameters.recovery_crit_; 
		recovery_trk_            = parameters.recovery_trk_;
		recovery_gs_             = parameters.recovery_gs_;
		recovery_vs_             = parameters.recovery_vs_;
		recovery_alt_            = parameters.recovery_alt_;
		ca_bands_                = parameters.ca_bands_;
		ca_factor_               = parameters.ca_factor_;

		// Contours
		contour_thr_ = parameters.contour_thr_;	

		// Alert levels
		alertor.copy(parameters.alertor);
	}

	/** 
	 * @return lookahead time in seconds.
	 */
	public double getLookaheadTime() {
		return lookahead_time_;
	}

	/** 
	 * @return lookahead time in specified units [u].
	 */
	public double getLookaheadTime(String u) {
		return Units.to(u,getLookaheadTime());
	}

	/** 
	 * @return left track in radians [0 - pi] [rad] from current ownship's track
	 */
	public double getLeftTrack() {
		return left_trk_;
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
		return right_trk_;
	}

	/** 
	 * @return right track in specified units [0 - pi] [u] from current ownship's track
	 */
	public double getRightTrack(String u) {
		return Units.to(u,getRightTrack());
	}

	/** 
	 * @return minimum ground speed in internal units [m/s].
	 */
	public double getMinGroundSpeed() {
		return min_gs_;
	}

	/** 
	 * @return minimum ground speed in specified units [u].
	 */
	public double getMinGroundSpeed(String u) {
		return Units.to(u,getMinGroundSpeed());
	}

	/** 
	 * @return maximum ground speed in internal units [m/s].
	 */
	public double getMaxGroundSpeed() {
		return max_gs_;
	}

	/** 
	 * @return maximum ground speed in specified units [u].
	 */
	public double getMaxGroundSpeed(String u) {
		return Units.to(u,getMaxGroundSpeed());
	}

	/** 
	 * @return minimum vertical speed in internal units [m/s].
	 */
	public double getMinVerticalSpeed() {
		return min_vs_;
	}

	/** 
	 * @return minimum vertical speed in specified units [u].
	 */
	public double getMinVerticalSpeed(String u) {
		return Units.to(u,getMinVerticalSpeed());
	}

	/** 
	 * @return maximum vertical speed in internal units [m/s].
	 */
	public double getMaxVerticalSpeed() {
		return max_vs_;
	}

	/** 
	 * @return maximum vertical speed in specified units [u].
	 */
	public double getMaxVerticalSpeed(String u) {
		return Units.to(u,getMaxVerticalSpeed());
	}

	/** 
	 * @return minimum altitude in internal units [m].
	 */
	public double getMinAltitude() {
		return min_alt_;
	}

	/** 
	 * @return minimum altitude in specified units [u].
	 */
	public double getMinAltitude(String u) {
		return Units.to(u,getMinAltitude());
	}

	/** 
	 * @return maximum altitude in internal units [m].
	 */
	public double getMaxAltitude() {
		return max_alt_;
	}

	/** 
	 * @return maximum altitude in specified units [u].
	 */
	public double getMaxAltitude(String u) {
		return Units.to(u,getMaxAltitude());
	}

	/** 
	 * @return track step in internal units [rad].
	 */
	public double getTrackStep() {
		return trk_step_;
	}

	/** 
	 * @return track step in specified units [u].
	 */
	public double getTrackStep(String u) {
		return Units.to(u,getTrackStep());
	}

	/** 
	 * @return ground speed step in internal units [m/s].
	 */
	public double getGroundSpeedStep() {
		return gs_step_;
	}

	/** 
	 * @return ground speed step in specified units [u].
	 */
	public double getGroundSpeedStep(String u) {
		return Units.to(u,getGroundSpeedStep());
	}

	/** 
	 * @return vertical speed step in internal units [m/s].
	 */
	public double getVerticalSpeedStep() {
		return vs_step_;
	}

	/** 
	 * @return vertical speed step in specified units [u].
	 */
	public double getVerticalSpeedStep(String u) {
		return Units.to(u,getVerticalSpeedStep());
	}

	/** 
	 * @return altitude step in internal units [m].
	 */
	public double getAltitudeStep() {
		return alt_step_;
	}

	/** 
	 * @return altitude step in specified units [u].
	 */
	public double getAltitudeStep(String u) {
		return Units.to(u,getAltitudeStep());
	}

	/** 
	 * @return horizontal acceleration in internal units [m/s^2].
	 */
	public double getHorizontalAcceleration() {
		return horizontal_accel_;
	}

	/** 
	 * @return horizontal acceleration in specified units [u].
	 */
	public double getHorizontalAcceleration(String u) {
		return Units.to(u,getHorizontalAcceleration());
	}

	/** 
	 * @return vertical acceleration in internal units [m/s^2].
	 */
	public double getVerticalAcceleration() {
		return vertical_accel_;
	}

	/** 
	 * @return vertical acceleration in specified units [u].
	 */
	public double getVerticalAcceleration(String u) {
		return Units.to(u,getVerticalAcceleration());
	}

	/** 
	 * @return turn rate in internal units [rad/s].
	 */
	public double getTurnRate() {
		return turn_rate_;
	}

	/** 
	 * @return turn rate in specified units [u].
	 */
	public double getTurnRate(String u) {
		return Units.to(u,getTurnRate());
	}

	/** 
	 * @return bank angle in internal units [rad].
	 */
	public double getBankAngle() {
		return bank_angle_;
	}

	/** 
	 * @return bank angle in specified units [u].
	 */
	public double getBankAngle(String u) {
		return Units.to(u,getBankAngle());
	}

	/** 
	 * @return vertical rate in internal units [m/s].
	 */
	public double getVerticalRate() {
		return vertical_rate_;
	}

	/** 
	 * @return vertical rate in specified units [u].
	 */
	public double getVerticalRate(String u) {
		return Units.to(u,getVerticalRate());
	}

	/** 
	 * @return horizontal NMAC distance in internal units [m].
	 */
	public double getHorizontalNMAC() {
		return horizontal_nmac_;
	}

	/** 
	 * @return horizontal NMAC distance in specified units [u].
	 */
	public double getHorizontalNMAC(String u) {
		return Units.to(u,getHorizontalNMAC());
	}

	/** 
	 * @return vertical NMAC distance in internal units [m].
	 */
	public double getVerticalNMAC() {
		return vertical_nmac_;
	}

	/** 
	 * @return vertical NMAC distance in specified units [u].
	 */
	public double getVerticalNMAC(String u) {
		return Units.to(u,getVerticalNMAC());
	}

	/** 
	 * @return recovery stability time in seconds.
	 */
	public double getRecoveryStabilityTime() {
		return recovery_stability_time_;
	}

	/** 
	 * @return recovery stability time in specified units [u].
	 */
	public double getRecoveryStabilityTime(String u) {
		return Units.to(u,getRecoveryStabilityTime());
	}

	/** 
	 * @return minimum horizontal recovery distance in internal units [m].
	 */
	public double getMinHorizontalRecovery() {
		return min_horizontal_recovery_;
	}

	/** 
	 * @return minimum horizontal recovery distance in specified units [u].
	 */
	public double getMinHorizontalRecovery(String u) {
		return Units.to(u,getMinHorizontalRecovery());
	}

	/** 
	 * @return minimum vertical recovery distance in internal units [m].
	 */
	public double getMinVerticalRecovery() {
		return min_vertical_recovery_;
	}

	/** 
	 * @return minimum vertical recovery distance in specified units [u].
	 */
	public double getMinVerticalRecovery(String u) {
		return Units.to(u,getMinVerticalRecovery());
	}

	/** 
	 * Set lookahead time to value in seconds.
	 */
	public boolean setLookaheadTime(double val) {
		lookahead_time_ = Math.abs(val);	
		return error.isPositive("setLookaheadTime",val);
	}

	/** 
	 * Set lookahead time to value in specified units [u].
	 */
	public boolean setLookaheadTime(double val, String u) {
		units_.put("lookahead_time",u);
		return setLookaheadTime(Units.from(u,val));
	}

	/** 
	 * Set left track to value in internal units [rad]. Value is expected to be in [0 - pi]
	 */
	public boolean setLeftTrack(double val) {
		left_trk_ = Math.abs(Util.to_pi(val));
		return error.isBetween("setLeftTrack",val,0,Math.PI);
	}

	/** 
	 * Set left track to value in specified units [u]. Value is expected to be in [0 - pi]
	 */
	public boolean setLeftTrack(double val, String u) {
		units_.put("left_trk",u);
		return setLeftTrack(Units.from(u,val));
	}

	/** 
	 * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
	 */
	public boolean setRightTrack(double val) {
		right_trk_ = Math.abs(Util.to_pi(val));
		return error.isBetween("setRightTrack",val,0,Math.PI);
	}

	/** 
	 * Set right track to value in specified units [u]. Value is expected to be in [0 - pi]
	 */
	public boolean setRightTrack(double val, String u) {
		units_.put("right_trk",u);
		return setRightTrack(Units.from(u,val));
	}

	/** 
	 * Set minimum ground speed to value in internal units [m/s].
	 * Minimum ground speed must be greater than ground speed step.
	 */
	public boolean setMinGroundSpeed(double val) {
		min_gs_ = Math.abs(val);
		return error.isPositive("setMinGroundSpeed",val);
	}

	/** 
	 * Set minimum ground speed to value in specified units [u].
	 * Minimum ground speed must be greater than ground speed step.
	 */
	public boolean setMinGroundSpeed(double val, String u) {		
		units_.put("min_gs",u);
		return setMinGroundSpeed(Units.from(u,val));
	}

	/** 
	 * Set maximum ground speed to value in internal units [m/s].
	 */
	public boolean setMaxGroundSpeed(double val) {
		max_gs_ = Math.abs(val);
		return error.isPositive("setMaxGroundSpeed",val);
	}

	/** 
	 * Set maximum ground speed to value in specified units [u].
	 */
	public boolean setMaxGroundSpeed(double val, String u) {
		units_.put("max_gs",u);
		return setMaxGroundSpeed(Units.from(u,val));
	}

	/** 
	 * Set minimum vertical speed to value in internal units [m/s].
	 */
	public boolean setMinVerticalSpeed(double val) {
		min_vs_ = val;
		return true;
	}

	/** 
	 * Set minimum vertical speed to value in specified units [u].
	 */
	public boolean setMinVerticalSpeed(double val, String u) {
		units_.put("min_vs",u);
		return setMinVerticalSpeed(Units.from(u,val));
	}

	/** 
	 * Set maximum vertical speed to value in internal units [m/s].
	 */
	public boolean setMaxVerticalSpeed(double val) {
		max_vs_ = val;
		return true;
	}

	/** 
	 * Set maximum vertical speed to value in specified units [u].
	 */
	public boolean setMaxVerticalSpeed(double val, String u) {
		units_.put("max_vs",u);
		return setMaxVerticalSpeed(Units.from(u,val));
	}

	/** 
	 * Set minimum altitude to value in internal units [m].
	 */
	public boolean setMinAltitude(double val) {
		min_alt_ = Math.abs(val);
		return error.isNonNegative("setMinAltitude",val);
	}

	/** 
	 * Set minimum altitude to value in specified units [u].
	 */
	public boolean setMinAltitude(double val, String u) {
		units_.put("min_alt",u);
		return setMinAltitude(Units.from(u,val));
	}

	/** 
	 * Set maximum altitude to value in internal units [m].
	 */
	public boolean setMaxAltitude(double val) {
		max_alt_ = Math.abs(val);
		return error.isPositive("setMaxAltitude",val);
	}

	/** 
	 * Set maximum altitude to value in specified units [u].
	 */
	public boolean setMaxAltitude(double val, String u) {
		units_.put("max_alt",u);
		return setMaxAltitude(Units.from(u,val));
	}

	/** 
	 * Set track step to value in internal units [rad].
	 */
	public boolean setTrackStep(double val) {
		trk_step_ = Math.abs(Util.to_pi(val));
		return error.isPositive("setTrackStep",val) &&
				error.isLessThan("setTrackStep",val,Math.PI);
	}

	/** 
	 * Set track step to value in specified units [u].
	 */
	public boolean setTrackStep(double val, String u) {
		units_.put("trk_step",u);;
		return setTrackStep(Units.from(u,val));
	}

	/** 
	 * Set ground speed step to value in internal units [m/s].
	 */
	public boolean setGroundSpeedStep(double val) {
		gs_step_ = Math.abs(val);
		return error.isPositive("setGroundSpeedStep",val);
	}

	/** 
	 * Set ground speed step to value in specified units [u].
	 */
	public boolean setGroundSpeedStep(double val, String u) {
		units_.put("gs_step",u);
		return setGroundSpeedStep(Units.from(u,val));
	}

	/** 
	 * Set vertical speed step to value in internal units [m/s].
	 */
	public boolean setVerticalSpeedStep(double val) {
		vs_step_ = Math.abs(val);
		return error.isPositive("setVerticalSpeedStep",val);
	}

	/** 
	 * Set vertical speed step to value in specified units [u].
	 */
	public boolean setVerticalSpeedStep(double val, String u) {
		units_.put("vs_step",u);
		return setVerticalSpeedStep(Units.from(u,val));
	}

	/** 
	 * Set altitude step to value in internal units [m].
	 */
	public boolean setAltitudeStep(double val) {
		alt_step_ = Math.abs(val);
		return error.isPositive("setAltitudeStep",val);
	}

	/** 
	 * Set altitude step to value in specified units [u].
	 */
	public boolean setAltitudeStep(double val, String u) {
		units_.put("alt_step",u);
		return setAltitudeStep(Units.from(u,val));
	}

	/** 
	 * Set horizontal acceleration to value in internal units [m/s^2].
	 */
	public boolean setHorizontalAcceleration(double val) {
		horizontal_accel_ = Math.abs(val);
		return error.isNonNegative("setHorizontalAcceleration",val);
	}

	/** 
	 * Set horizontal acceleration to value in specified units [u].
	 */
	public boolean setHorizontalAcceleration(double val, String u) {
		units_.put("horizontal_accel",u);
		return setHorizontalAcceleration(Units.from(u,val));
	}

	/** 
	 * Set vertical acceleration to value in internal units [m/s^2].
	 */
	public boolean setVerticalAcceleration(double val) {
		vertical_accel_ = Math.abs(val);
		return error.isNonNegative("setVerticalAcceleration",val);
	}

	/** 
	 * Set vertical acceleration to value in specified units [u].
	 */
	public boolean setVerticalAcceleration(double val, String u) {
		units_.put("vertical_accel",u);
		return setVerticalAcceleration(Units.from(u,val));
	}

	private boolean set_turn_rate(double val) {
		turn_rate_ = Math.abs(val);
		return error.isNonNegative("setTurnRate",val);
	}

	private boolean set_bank_angle(double val) {
		bank_angle_ = Math.abs(val);
		return error.isNonNegative("setBankAngle",val);
	}

	/** 
	 * Set turn rate to value in internal units [rad/s]. As a side effect, this method
	 * resets the bank angle.
	 */
	public boolean setTurnRate(double val) {
		set_bank_angle(0.0);
		return set_turn_rate(val);
	}

	/** 
	 * Set turn rate to value in specified units [u]. As a side effect, this method
	 * resets the bank angle.
	 */
	public boolean setTurnRate(double val, String u) {
		units_.put("turn_rate",u);
		return setTurnRate(Units.from(u,val));
	}

	/** 
	 * Set bank angle to value in internal units [rad]. As a side effect, this method
	 * resets the turn rate.
	 */
	public boolean setBankAngle(double val) {
		set_turn_rate(0.0);
		return set_bank_angle(val);
	}

	/** 
	 * Set bank angle to value in specified units [u]. As a side effect, this method
	 * resets the turn rate.
	 */
	public boolean setBankAngle(double val, String u) {
		units_.put("bank_angle",u);
		return setBankAngle(Units.from(u,val));
	}

	/** 
	 * Set vertical rate to value in internal units [m/s].
	 */
	public boolean setVerticalRate(double val) {
		vertical_rate_ = Math.abs(val);
		return error.isNonNegative("setVerticalRate",val);
	}

	/** 
	 * Set vertical rate to value in specified units [u].
	 */
	public boolean setVerticalRate(double val, String u) {
		units_.put("vertical_rate",u);
		return setVerticalRate(Units.from(u,val));
	}

	/** 
	 * Set horizontal NMAC distance to value in internal units [m].
	 */
	public boolean setHorizontalNMAC(double val) {
		horizontal_nmac_ = Math.abs(val);
		return error.isNonNegative("setHorizontalNMAC",val);
	}

	/** 
	 * Set horizontal NMAC distance to value in specified units [u].
	 */
	public boolean setHorizontalNMAC(double val, String u) {
		units_.put("horizontal_nmac",u);
		return setHorizontalNMAC(Units.from(u,val));
	}

	/** 
	 * Set vertical NMAC distance to value in internal units [m].
	 */
	public boolean setVerticalNMAC(double val) {
		vertical_nmac_ = Math.abs(val);
		return error.isNonNegative("setVerticalNMAC",val);
	}

	/** 
	 * Set vertical NMAC distance to value in specified units [u].
	 */
	public boolean setVerticalNMAC(double val, String u) {
		units_.put("vertical_nmac",u);
		return setVerticalNMAC(Units.from(u,val));
	}

	/** 
	 * Set recovery stability time to value in seconds.
	 */
	public boolean setRecoveryStabilityTime(double val) {
		recovery_stability_time_ = Math.abs(val);
		return error.isNonNegative("setRecoveryStabilityTime",val);
	}

	/** 
	 * Set recovery stability time to value in specified units [u].
	 */
	public boolean setRecoveryStabilityTime(double val, String u) {
		units_.put("recovery_stability_time",u);
		return setRecoveryStabilityTime(Units.from(u,val));
	}

	/** 
	 * Set minimum recovery horizontal distance to value in internal units [m].
	 */
	public boolean setMinHorizontalRecovery(double val) {
		min_horizontal_recovery_ = Math.abs(val);
		return error.isNonNegative("setMinHorizontalRecovery",val);
	}

	/** 
	 * Set minimum recovery horizontal distance to value in specified units [u].
	 */
	public boolean setMinHorizontalRecovery(double val, String u) {
		units_.put("min_horizontal_recovery",u);
		return setMinHorizontalRecovery(Units.from(u,val));
	}

	/** 
	 * Set minimum recovery vertical distance to value in internal units [m].
	 */
	public boolean setMinVerticalRecovery(double val) {
		min_vertical_recovery_ = Math.abs(val);
		return error.isNonNegative("setMinVerticalRecovery",val);
	}

	/** 
	 * Set minimum recovery vertical distance to value in specified units [u].
	 */
	public boolean setMinVerticalRecovery(double val, String u) {
		units_.put("min_vertical_recovery",u);
		return setMinVerticalRecovery(Units.from(u,val));
	}

	/** 
	 * @return true if repulsive criteria is enabled for conflict bands.
	 */
	public boolean isEnabledConflictCriteria() {
		return conflict_crit_;
	}

	/** 
	 * Enable/disable repulsive criteria for conflict bands.
	 */
	public void setConflictCriteria(boolean flag) {
		conflict_crit_ = flag;
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
		return recovery_crit_;
	}

	/** 
	 * Enable/disable repulsive criteria for recovery bands.
	 */
	public void setRecoveryCriteria(boolean flag) {
		recovery_crit_ = flag;
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
	 * @return recovery bands flag for track bands.
	 */
	public boolean isEnabledRecoveryTrackBands() {
		return recovery_trk_;
	}

	/**
	 * @return recovery bands flag for ground speed bands.
	 */
	public boolean isEnabledRecoveryGroundSpeedBands() {
		return recovery_gs_;
	}

	/**
	 * @return true if recovery bands for vertical speed bands is enabled. 
	 */
	public boolean isEnabledRecoveryVerticalSpeedBands() {
		return recovery_vs_;
	}

	/**
	 * @return true if recovery bands for altitude bands is enabled. 
	 */
	public boolean isEnabledRecoveryAltitudeBands() {
		return recovery_alt_;
	}

	/** 
	 * Enable/disable recovery bands for track, ground speed, vertical speed, and altitude.
	 */ 
	public void setRecoveryBands(boolean flag) {
		setRecoveryTrackBands(flag);
		setRecoveryGroundSpeedBands(flag);
		setRecoveryVerticalSpeedBands(flag);
		setRecoveryAltitudeBands(flag);
	}

	/** 
	 * Enable all recovery bands for track, ground speed, vertical speed, and altitude.
	 */ 
	public void enableRecoveryBands() {
		setRecoveryBands(true);
	}

	/** 
	 * Disable all recovery bands for track, ground speed, vertical speed, and altitude.
	 */ 
	public void disableRecoveryBands() {
		setRecoveryBands(false);
	}

	/** 
	 * Sets recovery bands flag for track bands to value.
	 */ 
	public void setRecoveryTrackBands(boolean flag) {
		recovery_trk_ = flag;
	}

	/** 
	 * Sets recovery bands flag for ground speed bands to value.
	 */ 
	public void setRecoveryGroundSpeedBands(boolean flag) {
		recovery_gs_ = flag;
	}

	/** 
	 * Sets recovery bands flag for vertical speed bands to value.
	 */ 
	public void setRecoveryVerticalSpeedBands(boolean flag) {
		recovery_vs_ = flag;
	}

	/** 
	 * Sets recovery bands flag for altitude bands to value.
	 */ 
	public void setRecoveryAltitudeBands(boolean flag) {
		recovery_alt_ = flag;
	}

	/** 
	 * @return true if collision avoidance bands are enabled.
	 */
	public boolean isEnabledCollisionAvoidanceBands() {
		return ca_bands_;
	}

	/** 
	 * Enable/disable collision avoidance bands.
	 */ 
	public void setCollisionAvoidanceBands(boolean flag) {
		ca_bands_ = flag;
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
		return ca_factor_;
	}

	/** 
	 * @return set factor for computing collision avoidance bands. Factor value is in (0,1]
	 */
	public boolean setCollisionAvoidanceBandsFactor(double val) {
		ca_factor_ = Math.abs(val);
		return error.isPositive("setCollisionAvoidanceBandsFactor",val) &&
				error.isLessThan("setCollisionAvoidanceBandsFactor", val,1);
	}

	/** 
	 * Get horizontal contour threshold, specified in internal units [rad] as an angle to 
	 * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
	 * A value of pi means all contours.
	 */
	public double getHorizontalContourThreshold() {
		return contour_thr_;
	}

	/** 
	 * Get horizontal contour threshold, specified in given units [u] as an angle to 
	 * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
	 * A value of pi means all contours.
	 */
	public double getHorizontalContourThreshold(String u) {
		return Units.to(u,getHorizontalContourThreshold());
	}

	/** 
	 * Set horizontal contour threshold, specified in internal units [rad] [0 - pi] as an angle to 
	 * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
	 * A value of pi means all contours.
	 */
	public boolean setHorizontalContourThreshold(double val) {
		contour_thr_ = Math.abs(Util.to_pi(val));
		return error.isBetween("setHorizontalContourThreshold",val,0,Math.PI);
	}

	/** 
	 * Set horizontal contour threshold, specified in given units [u] [0 - pi] as an angle to 
	 * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
	 * A value of pi means all contours.
	 */
	public boolean setHorizontalContourThreshold(double val, String u) {
		units_.put("contour_thr",u);
		return setHorizontalContourThreshold(Units.from(u,val));
	}

	/** 
	 * Set instantaneous bands.
	 */
	public void setInstantaneousBands() {
		set_turn_rate(0.0);
		set_bank_angle(0.0);
		setHorizontalAcceleration(0.0);
		setVerticalAcceleration(0.0);
		setVerticalRate(0.0);
		setRecoveryStabilityTime(0.0);
	}

	/** 
	 * Set kinematic bands.
	 * Set turn rate to 3 deg/s, when type is true; set turn rate to  1.5 deg/s
	 * when type is false;
	 */
	public void setKinematicBands(boolean type) {
		// Section 1.2.3, DAA MOPS SC-228 V3.6
		setTurnRate(type ? 3.0 : 1.5,"deg/s");
		setHorizontalAcceleration(2.0,"m/s^2"); 
		setVerticalAcceleration(0.25,"G");
		setVerticalRate(500.0,"fpm");   
		setRecoveryStabilityTime(2.0,"s");
	}

	/**
	 *  Load parameters from file.
	 */
	public boolean loadFromFile(String file) {
		ConfigReader reader = new ConfigReader();
		reader.open(file);
		ParameterData parameters = reader.getParameters();
		setParameters(parameters);
		return !reader.hasError();
	}

	/**
	 *  Write parameters to file.
	 */
	public boolean saveToFile(String file) {
		PrintWriter p;
		try {
			p = new PrintWriter(file);
			p.print(toString());
			p.close();
		} catch (FileNotFoundException e) {
			return false;
		}
		return true;
	}

	public String toString() {
		String s = "# V-"+VERSION+"\n";
	  ParameterData p = getParameters();
	  s+=p.listToString(keys_);
	  s+="# Alert Levels\n";
		ParameterData q = alertor.getParameters();
		s+=q.toString();
		return s;
	}

	public String toPVS(int prec) {
		String s = "";
		s+="(# ";
		s+="lookahead_time := "+f.FmPrecision(lookahead_time_,prec)+", ";
		s+="left_trk := "+f.FmPrecision(left_trk_,prec)+", ";
		s+="right_trk := "+f.FmPrecision(right_trk_,prec)+", ";
		s+="min_gs := "+f.FmPrecision(min_gs_,prec)+", ";
		s+="max_gs := "+f.FmPrecision(max_gs_,prec)+", ";
		s+="min_vs := "+f.FmPrecision(min_vs_,prec)+", ";
		s+="max_vs := "+f.FmPrecision(max_vs_,prec)+", ";
		s+="min_alt := "+f.FmPrecision(min_alt_,prec)+", ";
		s+="max_alt := "+f.FmPrecision(max_alt_,prec)+", ";
		s+="trk_step := "+f.FmPrecision(trk_step_,prec)+", ";
		s+="gs_step := "+f.FmPrecision(gs_step_,prec)+", ";
		s+="vs_step := "+f.FmPrecision(vs_step_,prec)+", ";
		s+="alt_step := "+f.FmPrecision(alt_step_,prec)+", ";
		s+="horizontal_accel := "+f.FmPrecision(horizontal_accel_,prec)+", ";
		s+="vertical_accel := "+f.FmPrecision(vertical_accel_,prec)+", ";
		s+="turn_rate := "+f.FmPrecision(turn_rate_,prec)+", ";
		s+="bank_angle := "+f.FmPrecision(bank_angle_,prec)+", ";
		s+="vertical_rate := "+f.FmPrecision(vertical_rate_,prec)+", ";
		s+="horizontal_nmac :="+f.FmPrecision(horizontal_nmac_,prec)+", ";
		s+="vertical_nmac :="+f.FmPrecision(vertical_nmac_,prec)+", ";
		s+="recovery_stability_time := "+f.FmPrecision(recovery_stability_time_,prec)+", ";
		s+="min_horizontal_recovery := "+f.FmPrecision(min_horizontal_recovery_,prec)+", ";
		s+="min_vertical_recovery := "+f.FmPrecision(min_vertical_recovery_,prec)+", ";
		s+="conflict_crit := "+conflict_crit_+", ";
		s+="recovery_crit := "+recovery_crit_+", ";
		s+="recovery_trk := "+recovery_trk_+", ";
		s+="recovery_gs := "+recovery_gs_+", ";
		s+="recovery_vs := "+recovery_vs_+", ";
		s+="recovery_alt := "+recovery_alt_+", ";
		s+="ca_bands := "+ca_bands_+", ";
		s+="ca_factor := "+f.FmPrecision(ca_factor_,prec)+", ";
		s+="contour_thr := "+f.FmPrecision(contour_thr_,prec)+", ";
		s+="alertor := "+alertor.toPVS(prec);
		s+="#)";
		return s;
	}

	public ParameterData getParameters() {
		ParameterData p = new ParameterData();
	  updateParameterData(p);
		return p;
	}

	public void updateParameterData(ParameterData p) {  	
	  // Bands Parameters
	  p.setInternal("lookahead_time", lookahead_time_, getUnits("lookahead_time"));
	  p.updateComment("lookahead_time","Bands Parameters");

	  p.setInternal("left_trk", left_trk_, getUnits("left_trk"));
	  p.setInternal("right_trk", right_trk_, getUnits("right_trk"));
	  p.setInternal("min_gs", min_gs_, getUnits("min_gs"));
	  p.setInternal("max_gs", max_gs_, getUnits("max_gs"));
	  p.setInternal("min_vs", min_vs_, getUnits("min_vs"));
	  p.setInternal("max_vs", max_vs_, getUnits("max_vs"));
	  p.setInternal("min_alt", min_alt_, getUnits("min_alt"));
	  p.setInternal("max_alt", max_alt_, getUnits("max_alt"));

	  // Kinematic Parameters
	  p.setInternal("trk_step", trk_step_, getUnits("trk_step"));
	  p.updateComment("trk_step","Kinematic Parameters");

	  p.setInternal("gs_step", gs_step_, getUnits("gs_step"));
	  p.setInternal("vs_step", vs_step_, getUnits("vs_step"));
	  p.setInternal("alt_step", alt_step_, getUnits("alt_step"));
	  p.setInternal("horizontal_accel", horizontal_accel_, getUnits("horizontal_accel"));
	  p.setInternal("vertical_accel", vertical_accel_, getUnits("vertical_accel"));
	  p.setInternal("turn_rate", turn_rate_, getUnits("turn_rate"));
	  p.setInternal("bank_angle", bank_angle_, getUnits("bank_angle"));
	  p.setInternal("vertical_rate", vertical_rate_, getUnits("vertical_rate"));

	  // Recovery Bands Parameters
	  p.setInternal("recovery_stability_time", recovery_stability_time_, getUnits("recovery_stability_time"));
	  p.updateComment("recovery_stability_time","Recovery Bands Parameters");

	  p.setInternal("min_horizontal_recovery", min_horizontal_recovery_, getUnits("min_horizontal_recovery"));
	  p.setInternal("min_vertical_recovery", min_vertical_recovery_, getUnits("min_vertical_recovery"));
	  p.setBool("recovery_trk", recovery_trk_);
	  p.setBool("recovery_gs", recovery_gs_);
	  p.setBool("recovery_vs", recovery_vs_);
	  p.setBool("recovery_alt", recovery_alt_);

	  // Collision Avoidance Bands Parameters
	  p.setBool("ca_bands", ca_bands_);
	  p.updateComment("ca_bands","Collision Avoidance Bands Parameters");

	  p.setInternal("ca_factor", ca_factor_, "unitless");
	  p.setInternal("horizontal_nmac",horizontal_nmac_, getUnits("horizontal_nmac"));
	  p.setInternal("vertical_nmac",vertical_nmac_, getUnits("vertical_nmac"));

	  // Implicit Coordination Parameters
	  p.setBool("conflict_crit", conflict_crit_);
	  p.updateComment("conflict_crit","Implicit Coordination Parameters");
	  p.setBool("recovery_crit", recovery_crit_);

	  // Horizontal Contour Threshold
	  p.setInternal("contour_thr", contour_thr_, getUnits("contour_thr"));
	  p.updateComment("contour_thr","Horizontal Contour Threshold");

	  // Alertor
	  alertor.updateParameterData(p);
	}

	public void setParameters(ParameterData p) {
	  if (p.contains("lookahead_time")) {
	    setLookaheadTime(p.getValue("lookahead_time"));
	    units_.put("lookahead_time",p.getUnit("lookahead_time"));
	  }
	  if (p.contains("left_trk")) {
	    setLeftTrack(p.getValue("left_trk"));
	    units_.put("left_trk",p.getUnit("left_trk"));
	  }
	  if (p.contains("right_trk")) {
	    setRightTrack(p.getValue("right_trk"));
	    units_.put("right_trk",p.getUnit("right_trk"));
	  }
	  if (p.contains("min_gs")) {
	    setMinGroundSpeed(p.getValue("min_gs"));
	    units_.put("min_gs",p.getUnit("min_gs"));
	  }
	  if (p.contains("max_gs")) {
	    setMaxGroundSpeed(p.getValue("max_gs"));
	    units_.put("max_gs",p.getUnit("max_gs"));
	  }
	  if (p.contains("min_vs")) {
	    setMinVerticalSpeed(p.getValue("min_vs"));
	    units_.put("min_vs",p.getUnit("min_vs"));
	  }
	  if (p.contains("max_vs")) {
	    setMaxVerticalSpeed(p.getValue("max_vs"));
	    units_.put("max_vs",p.getUnit("max_vs"));
	  }
	  if (p.contains("min_alt")) {
	    setMinAltitude(p.getValue("min_alt"));
	    units_.put("min_alt",p.getUnit("min_alt"));
	  }
	  if (p.contains("max_alt")) {
	    setMaxAltitude(p.getValue("max_alt"));
	    units_.put("max_alt",p.getUnit("max_alt"));
	  }
	  // Kinematic bands
	  if (p.contains("trk_step")) {
	    setTrackStep(p.getValue("trk_step"));
	    units_.put("trk_step",p.getUnit("trk_step"));
	  }
	  if (p.contains("gs_step")) {
	    setGroundSpeedStep(p.getValue("gs_step"));
	    units_.put("gs_step",p.getUnit("gs_step"));
	  }
	  if (p.contains("vs_step")) {
	    setVerticalSpeedStep(p.getValue("vs_step"));
	    units_.put("vs_step",p.getUnit("vs_step"));
	  }
	  if (p.contains("alt_step")) {
	    setAltitudeStep(p.getValue("alt_step"));
	    units_.put("alt_step",p.getUnit("alt_step"));
	  }
	  if (p.contains("horizontal_accel")) {
	    setHorizontalAcceleration(p.getValue("horizontal_accel"));
	    units_.put("horizontal_accel",p.getUnit("horizontal_accel"));
	  }
	  if (p.contains("vertical_accel")) {
	    setVerticalAcceleration(p.getValue("vertical_accel"));
	    units_.put("vertical_accel",p.getUnit("vertical_accel"));
	  }
	  if (p.contains("turn_rate")) {
	    set_turn_rate(p.getValue("turn_rate"));
	    units_.put("turn_rate",p.getUnit("turn_rate"));
	  }
	  if (p.contains("bank_angle")) {
	    set_bank_angle(p.getValue("bank_angle"));
	    units_.put("bank_angle",p.getUnit("bank_angle"));
	  }
	  if (p.contains("vertical_rate")) {
	    setVerticalRate(p.getValue("vertical_rate"));
	    units_.put("vertical_rate",p.getUnit("vertical_rate"));
	  }
	  if (p.contains("horizontal_nmac")) {
	    setHorizontalNMAC(p.getValue("horizontal_nmac"));
	    units_.put("horizontal_nmac",p.getUnit("horizontal_nmac"));
	  }
	  if (p.contains("vertical_nmac")) {
	    setVerticalNMAC(p.getValue("vertical_nmac"));
	    units_.put("vertical_nmac",p.getUnit("vertical_nmac"));
	  }
	  // Recovery bands
	  if (p.contains("recovery_stability_time")) {
	    setRecoveryStabilityTime(p.getValue("recovery_stability_time"));
	    units_.put("recovery_stability_time",p.getUnit("recovery_stability_time"));
	  }
	  if (p.contains("min_horizontal_recovery")) {
	    setMinHorizontalRecovery(p.getValue("min_horizontal_recovery"));
	    units_.put("min_horizontal_recovery",p.getUnit("min_horizontal_recovery"));
	  }
	  if (p.contains("min_vertical_recovery")) {
	    setMinVerticalRecovery(p.getValue("min_vertical_recovery"));
	    units_.put("min_vertical_recovery",p.getUnit("min_vertical_recovery"));
	  }
	  // Criteria parameters
	  if (p.contains("conflict_crit")) {
	    setConflictCriteria(p.getBool("conflict_crit"));
	  }
	  if (p.contains("recovery_crit")) {
	    setRecoveryCriteria(p.getBool("recovery_crit"));
	  }
	  // Recovery parameters
	  if (p.contains("recovery_trk")) {
	    setRecoveryTrackBands(p.getBool("recovery_trk"));
	  }
	  if (p.contains("recovery_gs")) {
	    setRecoveryGroundSpeedBands(p.getBool("recovery_gs"));
	  }
	  if (p.contains("recovery_vs")) {
	    setRecoveryVerticalSpeedBands(p.getBool("recovery_vs"));
	  }
	  if (p.contains("recovery_alt")) {
	    setRecoveryAltitudeBands(p.getBool("recovery_alt"));
	  }
	  if (p.contains("ca_bands")) {
	    setCollisionAvoidanceBands(p.getBool("ca_bands"));
	  }
	  if (p.contains("ca_factor")) {
	    setCollisionAvoidanceBandsFactor(p.getValue("ca_factor"));
	  }
	  // Contours
	  if (p.contains("contour_thr")) {
	    setHorizontalContourThreshold(p.getValue("contour_thr"));
	    units_.put("contour_thr",p.getUnit("contour_thr"));
	  }
	  // Alertor
	  alertor.setParameters(p);
	} 

	public String getUnits(String key) {
		String u = units_.get(key);
		if (u == null) {
			return "unspecified";
		}
		return u;
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
