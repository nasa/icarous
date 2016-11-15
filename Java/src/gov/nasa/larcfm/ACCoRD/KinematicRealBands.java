/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.IntervalSet;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

abstract public class KinematicRealBands extends KinematicIntegerBands {

	private boolean outdated_; // Boolean to control re-computation of cached values
	private int checked_;  // Cached status of input values. Negative unchecked, 0 unvalid, 1 valid
	private List<List<TrafficState>> peripheral_acs_; // Cached list of peripheral aircraft per alert level
	/* The length of peripheral_acs_ is greater than or equal to the length of the alertor. */
	private List<BandsRange> ranges_;     // Cached list of bands ranges
	/* 
	 * recovery_time_ is the time to recovery from violation. 
	 * Negative infinity means no possible recovery.
	 * NaN means no recovery bands are computed (either because there is no conflict or
	 * because they are disabled)
	 */		
	private double recovery_time_; // Cached recovery time
	/*
	 * resolutions_ is a list of resolution intervals per alert level
	 * Negative/positive infinity means no possible resolutions
	 * NaN means no resolutions are computed (either because there is no conflict or
	 * because of invalid inputs)
	 */
	private List<Interval> resolutions_; // Chached resolutions per alert level

	/* Parameters for conflict bands */
	private double  min_;  // Minimum/donw value 
	private double  max_;  // Maximum/up value
	private boolean rel_;  // Determines if (min_,max_) are either relative, when rel_ is true, 
	// or absolute values, when rel is false, with respect to current value. In the former case, 
	// it is expected that min <= 0, and max >= 0. Otherwise, it is expected that 
	// min <= current value <= max.                          
	private double  mod_;  // If mod_ > 0, bands are circular modulo this value
	private boolean circular_; // True if bands is fully circular
	private double  step_; // Value step

	/* Parameters for recovery bands */
	private boolean recovery_; 

	public KinematicRealBands(double min, double max, boolean rel, double mod, double step, boolean recovery) {
		outdated_ = true;
		checked_ = -1;
		peripheral_acs_ = new ArrayList<List<TrafficState>>();
		ranges_ = new ArrayList<BandsRange>();
		recovery_time_ = Double.NaN;
		resolutions_ = new ArrayList<Interval>();
		min_ = min;
		max_ = max;
		rel_ = rel;
		mod_ = mod;
		circular_ = false;
		step_ = step;
		recovery_ = recovery;
	}

	public KinematicRealBands(double min, double max, double step, boolean recovery) {
		this(min,max,false,0,step,recovery);
	}

	public KinematicRealBands(KinematicRealBands b) {
		this(b.min_,b.max_,b.rel_,b.mod_,b.step_,b.recovery_);
	}

	abstract public double own_val(TrafficState ownship);

	abstract public double time_step(TrafficState ownship);

	abstract public boolean instantaneous_bands();

	public double get_min() {
		return min_;
	}

	public double get_max() {
		return max_;
	}

	public boolean get_rel() {
		return rel_;
	}

	public double get_mod() {
		return mod_;
	}

	public double get_step() {
		return step_;
	}

	public boolean get_recovery() {
		return recovery_;
	}

	public void set_min(double val) {
		if (val != min_) {
			min_ = val;
			reset();
		}
	}

	public void set_max(double val) {
		if (val != max_) {
			max_ = val;
			reset();
		}
	}

	// As a side effect this method resets the min_/max_ values.
	public void set_rel(boolean val) {
		if (val != rel_) {
			rel_ = val;
			min_ = Double.NaN;
			max_ = Double.NaN;
			reset();
		}
	}

	public void set_mod(double val) {
		if (val >= 0 && val != mod_) {
			mod_ = val;
			reset();
		}
	}

	public void set_step(double val) {
		if (val > 0 && val != step_) {
			step_ = val;
			reset();
		}
	}

	public void set_recovery(boolean flag) {
		if (flag != recovery_) {
			recovery_ = flag;
			reset();
		}
	}

	/** 
	 * Return val modulo mod_, when mod_ > 0. Otherwise, returns val. 
	 */
	private double mod_val(double val) {
		return mod_ > 0 ? Util.modulo(val,mod_) : val;
	}

	/** 
	 * When mod_ == 0, min_val <= max_val. When mod_ > 0, min_val is a value is in [0,mod_]. 
	 * In this case, it is not always true that min_val <= max_val
	 */
	public double min_val(TrafficState ownship) {
		if (circular_) {
			return 0;
		} 
		return rel_ ? mod_val(own_val(ownship)+min_) : min_;
	}

	/** 
	 * Positive distance from current value to minimum value. When mod_ > 0, min_rel is a value in [0,mod_/2]
	 */
	public double min_rel(TrafficState ownship) {
		if (circular_) {
			return mod_/2.0;
		}
		return rel_ ? -min_ : mod_val(own_val(ownship)-min_);
	}

	/** 
	 * When mod_ == 0, min_val <= max_val. When mod_ > 0, max_val is a value in [0,mod_]. 
	 * In this case, it is not always true that min_val <= max_val
	 */
	public double max_val(TrafficState ownship) {
		if (circular_) {
			return mod_;
		}     
		return rel_ ? mod_val(own_val(ownship)+max_) : max_;
	}

	/**
	 * Positive distance from current value to maximum value. When mod_ > 0, max_rel is a value in [0,mod_/2]
	 */
	public double max_rel(TrafficState ownship) {
		if (circular_) {
			return mod_/2.0;
		}
		return rel_ ? max_ : mod_val(max_ - own_val(ownship));
	}

	public boolean check_input(TrafficState ownship) {
		if (checked_ < 0) {
			checked_ = 0;
			if (ownship.isValid() && step_ > 0 && Double.isFinite(min_) && Double.isFinite(max_)) {
				double val = own_val(ownship);
				if (rel_ ? min_ <= 0.0 && max_ >= 0.0 : 
					min_ <= val && val <= max_) {
					if (mod_ >= 0.0 && (mod_ == 0.0 || 
							(Util.almost_leq(max_-min_,mod_) &&
									(rel_ ? Util.almost_leq(max_,mod_/2.0): Util.almost_leq(max_,mod_))))) {
						checked_ = 1;
						circular_ = mod_ > 0 && Util.almost_equals(max_-min_,mod_);
					}
				}
			}
		}
		return checked_ > 0;
	}

	public boolean kinematic_conflict(KinematicBandsCore core, TrafficState ac, 
			Detection3D detector, double alerting_time) {
		List<TrafficState> alerting_set = new ArrayList<TrafficState>();
		alerting_set.add(ac);
		return check_input(core.ownship) && 
				any_red(detector,Detection3D.NoDetector,core.criteria_ac(),core.epsilonH(),core.epsilonV(),
						0,alerting_time,core.ownship,alerting_set);
	}

	public int length(KinematicBandsCore core) {   
		update(core);
		return ranges_.size();
	}

	public Interval interval(KinematicBandsCore core, int i) {
		if (i < 0 || i >= length(core)) {
			return Interval.EMPTY;
		}
		return ranges_.get(i).interval;
	}

	public BandsRegion region(KinematicBandsCore core, int i) {
		if (i < 0 || i >= length(core)) {
			return BandsRegion.UNKNOWN;
		} else {
			return ranges_.get(i).region;
		}
	}

	/*
	 *  Returns true if the first interval extends to the last interval. This happens
	 *  when mod_ > 0, the low value is 0, and the max value is mod_.  
	 */
	private boolean rollover() {
		return mod_ > 0 && 
				Util.almost_equals(ranges_.get(0).interval.low,0) &&
				Util.almost_equals(ranges_.get(ranges_.size()-1).interval.up,mod_);
	}

	/** 
	 * Return index where val is found, -1 if invalid input, >= length if not found 
	 */
	public int rangeOf(KinematicBandsCore core, double val) {
		if (check_input(core.ownship)) {
			val = mod_val(val);
			int last_index = length(core)-1;
			boolean rov = rollover();
			for (int i=0; i <= last_index; ++i) {
				boolean none = ranges_.get(i).region.isResolutionBand();	
				int order_i = ranges_.get(i).region.order();
				boolean lb_close = none ||
						(i > 0 && order_i <= ranges_.get(i-1).region.order()) ||
						(i == 0 && rov && order_i <= ranges_.get(last_index).region.order());
				boolean ub_close = none ||
						(i < last_index && order_i <= ranges_.get(i+1).region.order()) ||
						(i == last_index && rov && order_i <= ranges_.get(0).region.order());
				if (ranges_.get(i).interval.in(val,lb_close,ub_close)) {
					return i;
				} 
			}
			if (rov) {
				if (Util.almost_equals(val,0)) {
					return 0;
				}
			} else {
				if (Util.almost_equals(val,min_val(core.ownship))) {
					return 0;
				}
				if (Util.almost_equals(val,max_val(core.ownship))) {
					return last_index;
				}			
			}
		}
		return -1;
	}

	/**
	 *  Reset cached values 
	 */
	public void reset() {
		outdated_ = true;
		checked_ = -1;
		ranges_.clear();
		recovery_time_ = Double.NaN;
		resolutions_.clear();
	}

	/**
	 *  Update cached values 
	 */
	private void update(KinematicBandsCore core) {
		if (outdated_) {
			for (int alert_level=1; alert_level <= core.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
				if (alert_level-1 >= peripheral_acs_.size()) {
					peripheral_acs_.add(new ArrayList<TrafficState>());
				} else {
					peripheral_acs_.get(alert_level-1).clear();
				}
				if (core.parameters.alertor.getLevel(alert_level).getRegion().isConflictBand()) {
					peripheral_aircraft(core,alert_level);
				}
			}
			if (check_input(core.ownship)) {
				compute(core);
			} 
			outdated_ = false;
		}
	}

	/**
	 *  Force computation of kinematic bands
	 */
	public void force_compute(KinematicBandsCore core) {
		reset();
		update(core);
	}

	/**
	 * Put in peripheral_acs_ the list of aircraft predicted to be in conflict for the given alert level 
	 * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
	 */
	private void peripheral_aircraft(KinematicBandsCore core, int alert_level) {
		Detection3D detector = core.parameters.alertor.getLevel(alert_level).getDetector();
		double T = core.parameters.alertor.getLevel(alert_level).getAlertingTime();
		for (int i = 0; i < core.traffic.size(); ++i) {
			TrafficState ac = core.traffic.get(i);
			ConflictData det = detector.conflictDetection(core.own_s(),core.own_v(),ac.get_s(),ac.get_v(),0,T);
			if (!det.conflict() && kinematic_conflict(core,ac,detector,T)) {
				peripheral_acs_.get(alert_level-1).add(ac);
			}
		}
	}

	/**
	 * Return list of peripheral aircraft for a given alert level.
	 * Requires: 0 <= alert_level <= alertor.size(). If alert_level is 0, 
	 * conflict_level is used.
	 */
	public List<TrafficState> peripheralAircraft(KinematicBandsCore core, int alert_level) {
		update(core);
		if (alert_level == 0) {
			alert_level = core.parameters.alertor.conflictAlertLevel();
		}
		if (alert_level >= 1 && alert_level <= core.parameters.alertor.mostSevereAlertLevel()) {
			return peripheral_acs_.get(alert_level-1);
		}
		return TrafficState.INVALIDL;
	}

	/**
	 * Return time to recovery. Return NaN if bands are not saturated and negative infinity 
	 * when bands are saturated but no recovery within early alerting time.
	 */
	public double timeToRecovery(KinematicBandsCore core) {   
		update(core);
		return recovery_time_;
	}

	/**
	 * Return list of bands ranges 
	 */
	public List<BandsRange> ranges(KinematicBandsCore core) {
		update(core);
		return ranges_;
	}

	/** 
	 * Ensure that the intervals are "complete", filling in missing intervals and ensuring the 
	 * bands end at the  proper bounds. 
	 * Requires none_sets to be a non-empty list and size(none_sets) == size(regions)
	 */
	private void color_bands(List<IntervalSet> none_sets, List<BandsRegion> regions,
			KinematicBandsCore core, boolean recovery) {

		double min = min_val(core.ownship);
		double max = max_val(core.ownship);

		// Lists colored bounds
		List<ColoredValue> l1 = new ArrayList<ColoredValue>();
		List<ColoredValue> l2 = new ArrayList<ColoredValue>();

		BandsRegion green = recovery? BandsRegion.RECOVERY : BandsRegion.NONE;

		if (mod_ == 0 || min <= max) {
			l1.add(new ColoredValue(min,BandsRegion.UNKNOWN));
			l1.add(new ColoredValue(max,regions.get(regions.size()-1)));
		} else {
			// When mod !=0 && min > max, there are two lists of colored bounds
			l1.add(new ColoredValue(0,BandsRegion.UNKNOWN));
			l1.add(new ColoredValue(max,regions.get(regions.size()-1)));
			l2.add(new ColoredValue(min,BandsRegion.UNKNOWN));
			l2.add(new ColoredValue(mod_,regions.get(regions.size()-1)));
		}

		int last_level = recovery ? none_sets.size()-1 : 0;

		// Color levels from most severe to less severe
		for (int level = none_sets.size()-1; level >= last_level; --level) {
			BandsRegion lb_color = regions.get(level); 
			BandsRegion ub_color = level == last_level ? green : regions.get(level-1);
			for (int i=0; i < none_sets.get(level).size(); ++i) {
				Interval ii = none_sets.get(level).getInterval(i);
				if (ii.up <= max) {
					ColoredValue.insert(l1,ii,lb_color,ub_color);
				} else {
					ColoredValue.insert(l2,ii,lb_color,ub_color);
				}       
			}
		}

		ranges_.clear();
		ColoredValue.toBands(ranges_, l1);
		if (mod_ != 0 && min > max) {
			ColoredValue.toBands(ranges_, l2);
		}
	}

	/** 
	 * Compute recovery bands. Returns recovery time.
	 */ 
	private double compute_recovery_bands(IntervalSet noneset, KinematicBandsCore core,List<TrafficState> alerting_set) {
		double recovery_time = Double.NEGATIVE_INFINITY;
		int recovery_level = core.parameters.alertor.conflictAlertLevel();
		Detection3D detector = core.parameters.alertor.getLevel(recovery_level).getDetector();
		double T = core.parameters.alertor.getLevel(recovery_level).getEarlyAlertingTime();
		TrafficState repac = core.recovery_ac();
		CDCylinder cd3d = CDCylinder.mk(core.parameters.getHorizontalNMAC(),core.parameters.getVerticalNMAC());
		none_bands(noneset,cd3d,Detection3D.NoDetector,repac,core.epsilonH(),core.epsilonV(),0,T,core.ownship,alerting_set);
		if (!noneset.isEmpty()) {
			// If solid red, nothing to do. No way to kinematically escape using vertical speed without intersecting the
			// NMAC cylinder
			cd3d = CDCylinder.mk(core.minHorizontalRecovery(),core.minVerticalRecovery());
			Optional<Detection3D> ocd3d = Optional.of((Detection3D)cd3d);
			double factor = 1-core.parameters.getCollisionAvoidanceBandsFactor();
			while (cd3d.getHorizontalSeparation() > core.parameters.getHorizontalNMAC() || cd3d.getVerticalSeparation() > core.parameters.getVerticalNMAC()) {
				none_bands(noneset,cd3d,Detection3D.NoDetector,repac,core.epsilonH(),core.epsilonV(),0,T,core.ownship,alerting_set);
				boolean solidred = noneset.isEmpty();
				if (solidred && !core.parameters.isEnabledCollisionAvoidanceBands()) {
					return recovery_time;
				} else if (!solidred) {
					// Find first green band
					double pivot_red = 0;
					double pivot_green = T+1;
					double pivot = pivot_green-1;
					while ((pivot_green-pivot_red) > 0.5) {
						none_bands(noneset,detector,ocd3d,repac,core.epsilonH(),core.epsilonV(),pivot,T,core.ownship,alerting_set);
						solidred = noneset.isEmpty();
						if (solidred) {
							pivot_red = pivot;
						} else {
							pivot_green = pivot;
						}
						pivot = (pivot_red+pivot_green)/2.0;
					}
					if (pivot_green <= T) {
						recovery_time = Math.min(T,pivot_green+core.parameters.getRecoveryStabilityTime());
					} else {
						recovery_time = pivot_red;
					}
					none_bands(noneset,detector,ocd3d,repac,core.epsilonH(),core.epsilonV(),
							recovery_time,T,core.ownship,alerting_set);
					solidred = noneset.isEmpty();
					if (solidred) {
						recovery_time = Double.NEGATIVE_INFINITY;
					}
					if (!solidred || !core.parameters.isEnabledCollisionAvoidanceBands()) {
						return recovery_time;
					}
				}
				cd3d.setHorizontalSeparation(cd3d.getHorizontalSeparation()*factor);
				cd3d.setVerticalSeparation(cd3d.getVerticalSeparation()*factor);
			}
		}
		return recovery_time;
	}

	/** 
	 * Compute bands for one level. Return recovery time (NaN if recover bands are not computed)
	 */
	private double compute_level(IntervalSet noneset, KinematicBandsCore core, int alert_level) {        
		double min = min_val(core.ownship);
		double max = max_val(core.ownship);
		List<TrafficState> alerting_set = new ArrayList<TrafficState>();
		alerting_set.addAll(peripheral_acs_.get(alert_level-1));
		alerting_set.addAll(core.conflictAircraft(alert_level)); 
		if (alerting_set.isEmpty()) {
			if (mod_ == 0 || min <= max) {
				noneset.almost_add(min,max);
			} else {
				noneset.almost_add(min, mod_);
				noneset.almost_add(0,max);
			}
		} else {
			compute_none_bands(noneset,core,alert_level,core.criteria_ac());
			if (recovery_ && alert_level == core.parameters.alertor.conflictAlertLevel()) {
				// Compute recovery bands
				if (noneset.isEmpty()) {
					return compute_recovery_bands(noneset,core,alerting_set);
				} else if (instantaneous_bands() && core.timeIntervalOfViolation(alert_level).low == 0) {
					return 0;
				}
			}
		}
		return Double.NaN;
	}

	/** 
	 * Compute all bands.
	 */
	private void compute(KinematicBandsCore core) {
		recovery_time_ = Double.NaN;
		resolutions_.clear();
		List<IntervalSet> none_sets = new ArrayList<IntervalSet>();
		List<BandsRegion> regions = new ArrayList<BandsRegion>();
		boolean recovery = false;
		int alert_level;
		for (alert_level=1; alert_level <= core.parameters.alertor.mostSevereAlertLevel() && !recovery; ++alert_level) {
			BandsRegion region = core.parameters.alertor.getLevel(alert_level).getRegion();
			if (region.isConflictBand()) {
				IntervalSet noneset = new IntervalSet();
				double recovery_time = compute_level(noneset,core,alert_level);
				if (!Double.isNaN(recovery_time)) {
					recovery = true;
					recovery_time_ = recovery_time;
					region = core.parameters.alertor.getLevel(core.lastConflictAlertLevel()).getRegion();
				}	
				none_sets.add(noneset);
				regions.add(region);
				resolutions_.add(find_resolution(core,noneset)); 
			} else {
				resolutions_.add(new Interval(Double.NaN,Double.NaN));
			}
		}
		for (;alert_level <= core.parameters.alertor.mostSevereAlertLevel();++alert_level) {
			// Add [-oo,+oo] resolutions for level > conflict_level in case of recovery bands
			resolutions_.add(new Interval(Double.NEGATIVE_INFINITY,Double.POSITIVE_INFINITY));
		}
		color_bands(none_sets,regions,core,recovery);
	}

	private Interval find_resolution(KinematicBandsCore core, IntervalSet noneset) {
		double l = Double.NEGATIVE_INFINITY;
		double u = Double.POSITIVE_INFINITY;
		if (!noneset.isEmpty()) {
			// There is a resolution
			double val = own_val(core.ownship);
			for (int i=0; i < noneset.size(); ++i) {
				if (noneset.getInterval(i).inCC(val)) {
					// There is no conflict
					l = Double.NaN;
					u = Double.NaN;
					break;
				} else if (noneset.getInterval(i).up < val) {
					if (i+1==noneset.size()) {
						l = noneset.getInterval(i).up;
						if (mod_ > 0) {
							u = noneset.getInterval(0).low;
							if (Util.almost_geq(mod_val(u-val),mod_/2.0)) {
								u = Double.POSITIVE_INFINITY; 
							}
						}
						break;
					} else if (val < noneset.getInterval(i+1).low) {
						l = noneset.getInterval(i).up;
						u = noneset.getInterval(i+1).low;
						break;
					}
				} else if (val < noneset.getInterval(i).low) {
					if (i==0) {
						if (mod_ > 0) {
							l = noneset.getInterval(noneset.size()-1).up;
							if (Util.almost_geq(mod_val(val-l),mod_/2.0)) {
								u = Double.NEGATIVE_INFINITY; 
							}							
						}
						u = noneset.getInterval(i).low;
						break;
					}
				}
			}
		}
		return new Interval(l,u);
	}

	/** 
	 * Returns resolution maneuver for given alert level and direction.
	 * Return NaN if there is no conflict or if input is invalid.
	 * Return positive/negative infinity if there is no resolution to the 
	 * right/up and negative infinity if there is no resolution to the left/down.
	 * Requires: 0 <= alert_level <= alertor.size(). If alert_level is 0, 
	 * conflict_level is used.
	 * Region of alert_level should be a conflict band type, e.g., NEAR, MID, or FAR. 
	 */
	public double compute_resolution(KinematicBandsCore core, int alert_level, boolean dir) {
		if (alert_level == 0) {
			alert_level = core.parameters.alertor.conflictAlertLevel();
		}
		if (length(core) > 0 && 1 <= alert_level && 
				alert_level <= core.parameters.alertor.mostSevereAlertLevel() &&
				core.parameters.alertor.getLevel(alert_level).getRegion().isConflictBand()) {
			Interval resolution = resolutions_.get(alert_level-1);
			if (dir) {
				return resolution.up;
			} else {
				return resolution.low;
			}
		}
		return Double.NaN;
	}

	/**
	 * Compute preferred direction, for given alert level, based on resolution that is closer
	 * to current value.
	 */
	public boolean preferred_direction(KinematicBandsCore core, int alert_level) {
		double up = compute_resolution(core,alert_level,true);
		double down = compute_resolution(core,alert_level,false);
		double val = own_val(core.ownship);
		if (!Double.isFinite(up)) {
			return false;
		} else if (!Double.isFinite(down)) {
			return true;
		}
		return Util.almost_leq(mod_val(up-val),mod_val(val-down));
	}

	/**
	 * Return last time to maneuver, in seconds, for ownship with respect to traffic
	 * aircraft ac for conflict alert level. Return NaN if the ownship is not in conflict with aircraft ac within 
	 * early alerting time. Return negative infinity if there is no time to maneuver.
	 * Note: 1 <= alert_level <= alertor.size()
	 */
	public double last_time_to_maneuver(KinematicBandsCore core, TrafficState ac) {
		if (check_input(core.ownship)) {
			int conflict_level = core.parameters.alertor.conflictAlertLevel();
			Detection3D detector = core.parameters.alertor.getLevel(conflict_level).getDetector();
			double T = core.parameters.alertor.getLevel(conflict_level).getEarlyAlertingTime();
			ConflictData det = detector.conflictDetection(core.own_s(),core.own_v(),ac.get_s(),ac.get_v(),0,T);
			if (det.conflict()) {
				double pivot_red = det.getTimeIn();
				if (pivot_red == 0) {
					return Double.NEGATIVE_INFINITY;
				} 
				TrafficState own = core.ownship;
				List<TrafficState> traffic = new ArrayList<TrafficState>();
				double pivot_green = 0;
				double pivot = pivot_green;    
				while ((pivot_red-pivot_green) > 0.5) {
					TrafficState ownship  = own.linearProjection(pivot); 
					TrafficState intruder = ac.linearProjection(pivot);
					traffic.clear();
					traffic.add(intruder);
					if (detector.violation(ownship.get_s(),ownship.get_v(),intruder.get_s(),intruder.get_v()) ||
							all_red(detector,Detection3D.NoDetector,core.criteria_ac(),0,0,0,T,ownship,traffic)) {
						pivot_red = pivot;
					} else {
						pivot_green = pivot;
					}
					pivot = (pivot_red+pivot_green)/2.0;
				}
				if (pivot_green == 0) {
					return Double.NEGATIVE_INFINITY;
				} else {
					return pivot_green;
				}
			}
		}
		return Double.NaN;
	}

	private int maxdown(TrafficState ownship) {
		int down = (int)Math.ceil(min_rel(ownship)/get_step());
		if (mod_ > 0 && Util.almost_greater(down*get_step(),mod_/2.0)) {
			--down;
		}
		return down;
	}

	private int maxup(TrafficState ownship) {
		int up = (int)Math.ceil(max_rel(ownship)/get_step());
		if (mod_ > 0 && Util.almost_greater(up*get_step(),mod_/2.0)) {
			--up;
		}    
		return up;
	}

	/**
	 *  This function scales the interval, add a constant, and constraint the intervals to min and max.
	 *  The function takes care of modulo logic, in the case of circular bands.
	 */
	public void toIntervalSet(IntervalSet noneset, List<Integerval> l, double scal, double add, 
			double min, double max) {
		noneset.clear();
		for (int i=0; i < (int) l.size(); ++i) {
			Integerval ii = l.get(i);
			double lb = scal*ii.lb+add;
			double ub = scal*ii.ub+add;
			if (mod_ == 0)  {
				lb = Math.max(min,lb);
				ub = Math.min(max,ub);
				noneset.almost_add(lb,ub);
			} else {
				lb = mod_val(lb);
				ub = mod_val(ub);
				if (Util.almost_equals(lb,ub)) {
					// In this case the range is the whole interval
					if (min <= max) {
						noneset.almost_add(min,max);
					} else {
						noneset.almost_add(min,mod_);
						noneset.almost_add(0,max);
					}
				} else if (min <= max && lb <= ub) {
					noneset.almost_add(Math.max(min,lb),Math.min(max,ub));
				} else if (min <= max) {
					Interval mm = new Interval (min,max);
					Interval lbmax = new Interval(lb,mod_).intersect(mm);
					Interval minub = new Interval(0,ub).intersect(mm);
					noneset.almost_add(lbmax.low,lbmax.up);
					noneset.almost_add(minub.low,minub.up);
				} else if (lb <= ub) {
					Interval lbub = new Interval(lb,ub);
					Interval lbmax = new Interval(0,max).intersect(lbub);
					Interval minub = new Interval(min,mod_).intersect(lbub);
					noneset.almost_add(lbmax.low,lbmax.up);
					noneset.almost_add(minub.low,minub.up);
				} else {
					noneset.almost_add(Math.max(min,lb),mod_);
					noneset.almost_add(0,Math.min(max,ub));
				}
			}
		}
	}

	public void none_bands(IntervalSet noneset, Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
			int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic) {
		List<Integerval> bands_int = new ArrayList<Integerval>();
		if (instantaneous_bands()) {
			instantaneous_bands_combine(bands_int,conflict_det,recovery_det,B,T,0,B,
					maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv); 
		} else {
			kinematic_bands_combine(bands_int,conflict_det,recovery_det,time_step(ownship),B,T,0,B,
					maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv); 
		}
		toIntervalSet(noneset,bands_int,get_step(),own_val(ownship),min_val(ownship),max_val(ownship));
	}

	public boolean any_red(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
			int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic) {
		return instantaneous_bands() ? 
				any_instantaneous_red(conflict_det,recovery_det,B,T,0,B,
						maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0):
							any_int_red(conflict_det,recovery_det,time_step(ownship),B,T,0,B,
									maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0);
	}

	public boolean all_red(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
			int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic) {
		return instantaneous_bands() ? 
				all_instantaneous_red(conflict_det,recovery_det,B,T,0,B,
						maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0):
							all_int_red(conflict_det,recovery_det,time_step(ownship),B,T,0,B,
									maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0);
	}

	public boolean all_green(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
			int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic) {
		return !any_red(conflict_det,recovery_det,repac,epsh,epsv,B,T,ownship,traffic);
	}

	public boolean any_green(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
			int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic) {
		return !all_red(conflict_det,recovery_det,repac,epsh,epsv,B,T,ownship,traffic);
	}

	/**
	 * This function returns a resolution maneuver that is valid from B to T. 
	 * It returns NaN if there is no conflict and +/- infinity, depending on dir, if there 
	 * are no resolutions. 
	 * The value dir=false is down and dir=true is up. 
	 */
	public double resolution(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
			int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic, 
			boolean dir) {
		int maxn;
		int sign;
		if (dir) {
			maxn = maxup(ownship);
			sign = 1;
		} else {
			maxn = maxdown(ownship);
			sign = -1;
		}
		int ires = first_green(conflict_det,recovery_det,time_step(ownship),B,T,0,B,
				dir,maxn,ownship,traffic,repac,epsh,epsv);
		if (ires == 0) {
			return Double.NaN;
		} else if (ires < 0) {
			return sign*Double.POSITIVE_INFINITY;
		} else {
			return mod_val(own_val(ownship)+sign*ires*get_step());
		}
	}

	// Requires: 1 <= alert_level <= alertor.size()
	private void compute_none_bands(IntervalSet noneset, KinematicBandsCore core, int alert_level,
			TrafficState repac) {
		Detection3D detector = core.parameters.alertor.getLevel(alert_level).getDetector();
		none_bands(noneset,detector,Detection3D.NoDetector,repac,
				core.epsilonH(),core.epsilonV(),0,core.parameters.alertor.getLevel(alert_level).getAlertingTime(),
				core.ownship,peripheral_acs_.get(alert_level-1));
		IntervalSet noneset2 = new IntervalSet();
		none_bands(noneset2,detector,Detection3D.NoDetector,repac,
				core.epsilonH(),core.epsilonV(),0,core.parameters.alertor.getLevel(alert_level).getEarlyAlertingTime(),
				core.ownship,core.conflictAircraft(alert_level)); 
		noneset.almost_intersect(noneset2);
	}

	public String toString() {
		String s = "";
		s+="outdated_ = "+outdated_+"\n";		
		s+="checked_ = "+f.Fmi(checked_)+"\n";		
		for (int i=0; i < peripheral_acs_.size(); ++i) {
			s+="peripheral_acs_["+f.Fmi(i)+"]: "+
					TrafficState.listToString(peripheral_acs_.get(i))+"\n";
		}
		s+="step_ = "+f.FmPrecision(step_)+"\n";
		s+="min_ = "+f.FmPrecision(min_)+"\n";
		s+="max_ = "+f.FmPrecision(max_)+"\n";
		s+="mod_ = "+f.FmPrecision(mod_)+"\n";
		s+="rel_ = "+rel_+"\n";
		s+="circular_ = "+circular_+"\n";
		for (int i = 0; i < ranges_.size(); ++i) {
			s+="ranges_["+f.Fmi(i)+"] = ";
			s+=ranges_.get(i).toString()+"\n";
		} 
		for (int i=0; i < resolutions_.size(); ++i) {
			s+="resolutions_["+f.Fmi(i)+"] = "+resolutions_.get(i).toString()+"\n";
		}
		s+="recovery_time_: "+f.FmPrecision(recovery_time_)+ " [s]";
		return s;
	}

	public String toPVS(int prec) {
		String s = "((:";
		for (int i = 0; i < ranges_.size(); ++i) {
			if (i > 0) { 
				s+=", ";
			} else {
				s+=" ";
			}
			s+=ranges_.get(i).interval.toPVS(prec);
		} 
		s+=" :), (:";
		for (int i = 0; i < ranges_.size(); ++i) {
			if (i > 0) {
				s+=", ";
			} else {
				s+=" ";
			}
			s += ranges_.get(i).region;
		} 
		s+=" :), "+f.FmPrecision(recovery_time_,prec)+"::ereal)";
		return s;
	}

}
