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
import java.util.Iterator;
import java.util.List;
import java.util.Optional;

abstract public class KinematicRealBands extends KinematicIntegerBands {

  private boolean outdated_; // Boolean to control re-computation of cached values
  private int checked_;  // Cached status of input values. Negative unchecked, 0 unvalid, 1 valid
  private List<TrafficState> preventive_; // Cached list of preventive aircraft
  private List<Interval> intervals_;      // Cached list of intervals
  private List<BandsRegion> regions_;     // Cached list of regions
  private double recovery_time_;          // Cached recovery time 
  // recovery_time_ is the time needed to recover from violation. 
  // Negative infinity means no recovery; NaN means no recovery bands.

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
  private boolean recovery_; // Do compute recovery bands

  public KinematicRealBands(double min, double max, boolean rel, double mod, double step, boolean recovery) {
    outdated_ = true;
    checked_ = -1;
    preventive_ = new ArrayList<TrafficState>();
    intervals_ = new ArrayList<Interval>();
    regions_ = new ArrayList<BandsRegion>();
    recovery_time_ = Double.NaN;
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

  public void reset_min() {
    min_ = Double.NaN;
    reset();
  }

  public void reset_max() {
    max_ = Double.NaN;
    reset();
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

  private double mod_val(double val) {
    return mod_ > 0 ? Util.modulo(val,mod_) : val;
  }

  /** 
   * Returned value is in [0,mod_]. When mod_ == 0, min_val() <= max_val(). 
   * When mod_ > 0, it is not always true that min_val() <= max_val()
   */
  public double min_val(TrafficState ownship) {
    if (circular_) {
      return 0;
    } 
    return rel_ ? mod_val(own_val(ownship)+min_) : min_;
  }

  /** 
   * Return a positive number in [0,mod_/2]
   */
  public double min_rel(TrafficState ownship) {
    if (circular_) {
      return mod_/2.0;
    }
    return rel_ ? -min_ : mod_val(own_val(ownship)-min_);
  }

  /** 
   * Returned value is in [0,mod_]. When mod_ == 0, min_val() <= max_val(). 
   * When mod_ > 0, it is not always true that min_val() <= max_val()
   */
  public double max_val(TrafficState ownship) {
    if (circular_) {
      return mod_;
    }     
    return rel_ ? mod_val(own_val(ownship)+max_) : max_;
  }

  /**
   * Return a positive number in [0,mod_/2]
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

  public boolean kinematic_conflict(KinematicBandsCore core, TrafficState ac) {
    List<TrafficState> alerting_set = new ArrayList<TrafficState>();
    alerting_set.add(ac);
    return check_input(core.ownship) &&  
        any_red(core.detector,Detection3D.NoDetector,core.conflict_ac(),core.epsilonH(),core.epsilonV(),
            0,core.alertingTime(),core.ownship,alerting_set);
  }

  public int length(KinematicBandsCore core) {   
    update(core);
    return intervals_.size();
  }

  public Interval interval(KinematicBandsCore core, int i) {
    if (i < 0 || i >= length(core)) {
      return Interval.EMPTY;
    }
    return intervals_.get(i);
  }

  public BandsRegion region(KinematicBandsCore core, int i) {
    if (i < 0 || i >= length(core)) {
      return BandsRegion.UNKNOWN;
    } else {
      return regions_.get(i);
    }
  }

  /** 
   * Return index where val is found, -1 if invalid input, >= length if not found 
   */
  public int band(KinematicBandsCore core, double val) {
    int i=-1;
    if (check_input(core.ownship)) {
      val = mod_val(val);
      double min = min_val(core.ownship);
      double max = max_val(core.ownship);
      int zero_pos = -1;
      for (i=0; i < length(core); ++i) {
        boolean none = regions_.get(i).isResolutionBand();
        boolean lb_close = none || (!circular_ && Util.almost_equals(intervals_.get(i).low,min));
        boolean ub_close = none || (!circular_ && Util.almost_equals(intervals_.get(i).up,max));
        if (intervals_.get(i).in(val,lb_close,ub_close)) {
          return i;
        } else if (mod_ > 0 && Util.almost_equals(val,0)) {
          if (none && Util.almost_equals(intervals_.get(i).up,mod_)) {
            return i;
          } else if (Util.almost_equals(intervals_.get(i).low,0)) {
            zero_pos = i;
          }
        } 
      }
      if (zero_pos >= 0) {
        i = zero_pos;
      }
    }
    return i;
  }

  /**
   *  Reset cached values 
   */
  public void reset() {
    outdated_ = true;
    checked_ = -1;
    preventive_.clear();
    intervals_.clear();
    regions_.clear();
    recovery_time_ = Double.NaN;
  }

  /**
   *  Update cached values 
   */
  private void update(KinematicBandsCore core) {
    if (outdated_) {
      if (check_input(core.ownship)) {
        compute(core);
      } else {
        reset();
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

  private void preventive_aircraft(List<TrafficState> preventive, KinematicBandsCore core) {
    preventive.clear();
    for (int i = 0; i < core.traffic.size(); ++i) {
      TrafficState ac = core.traffic.get(i);
      ConflictData det = core.checkConflict(ac,0,core.alertingTime());
      if (!det.conflict() && kinematic_conflict(core,ac)) {
        preventive.add(ac);
      }
    }
  }

  /**
   *  Return list of preventive aircraft 
   */
  public List<TrafficState> preventiveAircraft(KinematicBandsCore core) {
    update(core);
    return preventive_;
  }

  /**
   * Return time to recovery. Return NaN if bands are not saturated and negative infinity 
   * when bands are saturated but no recovery within max_recovery_time.
   */
  public double timeToRecovery(KinematicBandsCore core) {   
    update(core);
    return recovery_time_;
  }

  /**
   *  Return list of interval ranges 
   */
  public List<Interval> intervals(KinematicBandsCore core) {
    update(core);
    return intervals_;
  }

  /**
   *  Return list of regions 
   */
  public List<BandsRegion> regions(KinematicBandsCore core) {
    update(core);
    return regions_;
  }

  /** 
   * Ensure that the intervals are "complete", filling in missing intervals and ensuring the 
   * bands end at the  proper bounds. The parameters bands is a set of no-conflict bands.
   */
  private void color_bands(TrafficState ownship, IntervalSet bands, BandsRegion red, BandsRegion green) {
    intervals_.clear();
    regions_.clear();
    double min = min_val(ownship);
    double max = max_val(ownship);
    if (bands.isEmpty()) {
      intervals_.add(new Interval(min,max));
      regions_.add(red);
    } else {
      double l = min;
      int i=0;
      for (; i < bands.size(); ++i) {
        Interval ii = bands.getInterval(i);
        if (Util.almost_less(l,ii.low)) {
          intervals_.add(new Interval(l,ii.low));
          regions_.add(red);
        }
        double u = max;
        if (Util.almost_less(ii.up,max)) {
          u = ii.up;
        }
        intervals_.add(new Interval(ii.low,u));
        regions_.add(green);
        l=u;
      }
      if (Util.almost_less(l,max)) {
        intervals_.add(new Interval(l,max));
        regions_.add(red);
      }
    }
  }

  /** 
   * Compute recovery bands.
   */ 
  private void compute_recovery_bands(IntervalSet noneset, KinematicBandsCore core,List<TrafficState> alerting_set) {
    recovery_time_ = Double.NEGATIVE_INFINITY;
    double T = core.maxRecoveryTime(); 
    TrafficState repac = core.recovery_ac();
    CDCylinder cd3d = CDCylinder.mk(ACCoRDConfig.NMAC_D,ACCoRDConfig.NMAC_H);
    none_bands(noneset,cd3d,Detection3D.NoDetector,repac,core.epsilonH(),core.epsilonV(),0,T,core.ownship,alerting_set);
    if (!noneset.isEmpty()) {
      // If solid red, nothing to do. No way to kinematically escape using vertical speed without intersecting the
      // NMAC cylinder
      cd3d = CDCylinder.mk(core.minHorizontalRecovery(),core.minVerticalRecovery());
      Optional<Detection3D> ocd3d = Optional.of((Detection3D)cd3d);
      while (cd3d.getHorizontalSeparation() > ACCoRDConfig.NMAC_D || cd3d.getVerticalSeparation() > ACCoRDConfig.NMAC_H) {
        none_bands(noneset,cd3d,Detection3D.NoDetector,repac,core.epsilonH(),core.epsilonV(),0,T,core.ownship,alerting_set);
        boolean solidred = noneset.isEmpty();
        if (solidred && !core.ca_bands) {
          return;
        } else if (!solidred) {
          // Find first green band
          double pivot_red = 0;
          double pivot_green = T+1;
          double pivot = pivot_green-1;
          while ((pivot_green-pivot_red) > 0.5) {
            none_bands(noneset,core.detector,ocd3d,repac,core.epsilonH(),core.epsilonV(),pivot,T,core.ownship,alerting_set);
            solidred = noneset.isEmpty();
            if (solidred) {
              pivot_red = pivot;
            } else {
              pivot_green = pivot;
            }
            pivot = (pivot_red+pivot_green)/2.0;
          }
          if (pivot_green <= T) {
            recovery_time_ = Math.min(T,pivot_green+core.recovery_stability_time);
          } else {
            recovery_time_ = pivot_red;
          }
          none_bands(noneset,core.detector,ocd3d,repac,core.epsilonH(),core.epsilonV(),recovery_time_,T,core.ownship,alerting_set);
          solidred = noneset.isEmpty();
          if (solidred) {
            recovery_time_ = Double.NEGATIVE_INFINITY;
          }
          if (!solidred || !core.ca_bands) {
            return;
          }
        }
        cd3d.setHorizontalSeparation(cd3d.getHorizontalSeparation()*0.8);
        cd3d.setVerticalSeparation(cd3d.getVerticalSeparation()*0.8);
      }
    }
  }

  /** 
   * Compute all bands.
   */
  private void compute(KinematicBandsCore core) {
    recovery_time_ = Double.NaN;
    preventive_aircraft(preventive_,core);
    List<TrafficState> alerting_set = new ArrayList<TrafficState>();
    alerting_set.addAll(preventive_);
    alerting_set.addAll(core.correctiveAircraft());
    IntervalSet noneset = new IntervalSet();
    if (alerting_set.isEmpty()) {
      double min = min_val(core.ownship);
      double max = max_val(core.ownship);
      if (mod_ == 0 || min <= max) {
        noneset.almost_add(min,max);
      } else {
        noneset.almost_add(min, mod_);
        noneset.almost_add(0,max);
      }
    } else {
      compute_none_bands(noneset,core,core.conflict_ac(),preventive_);
      boolean solidred = noneset.isEmpty();
      if (solidred) {
        if (recovery_) {
          compute_recovery_bands(noneset,core,alerting_set);
        } 
      }
    }
    color_bands(core.ownship,noneset,BandsRegion.NEAR,
        Double.isFinite(recovery_time_) ? BandsRegion.RECOVERY:BandsRegion.NONE);
  }

  /** 
   * Compute resolution maneuver that is valid for lookahead time. Return NaN if there is no conflict, 
   * positive infinity if there is no resolution to the right/up and negative infinity if there is no 
   * resolution to the left/down.
   */
  public double compute_resolution(KinematicBandsCore core, boolean dir) {
    return resolution(core.detector,Detection3D.NoDetector,core.conflict_ac(),core.epsilonH(),core.epsilonV(),0,
        core.lookahead_time,core.ownship,core.traffic,dir);
  }

  /**
   * Return last time to maneuver, in seconds, for ownship with respect to traffic
   * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within 
   * lookahead time. Return negative infinity if there is no time to maneuver.
   */
  public double last_time_to_maneuver(KinematicBandsCore core, TrafficState ac) {
    ConflictData det = core.checkConflict(ac,0,core.lookahead_time);
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
        if (all_red(core.detector,Detection3D.NoDetector,core.conflict_ac(),0,0,0,core.lookahead_time,ownship,traffic)) {
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
    } else {
      return Double.NaN;
    }
  }

  private int maxdown(TrafficState ownship) {
    int down = (int)Math.ceil(min_rel(ownship)/get_step())+1;
    if (mod_ > 0 && Util.almost_greater(down*get_step(),mod_/2.0)) {
      --down;
    }
    return down;
  }

  private int maxup(TrafficState ownship) {
    int up = (int)Math.ceil(max_rel(ownship)/get_step())+1;
    if (mod_ > 0 && Util.almost_greater(up*get_step(),mod_/2.0)) {
      --up;
    }    
    return up;
  }

  /**
   *  This function scales the interval, add a constant, and constraint the intervals to min and max. 
   **/ 
  public void toIntervalSet(IntervalSet noneset, List<Integerval> l, double scal, double add, double min, double max) {
    noneset.clear();
    for (Iterator<Integerval> i=l.iterator(); i.hasNext(); ) {
      Integerval ii = i.next();
      double lb = scal*ii.lb+add;
      double ub = scal*ii.ub+add;
      if (mod_ == 0 && min <= ub && lb <= max)  {
        noneset.almost_add(Math.max(min,lb),Math.min(max,ub));
      } else if (mod_ > 0) {
        if (0 <= lb && ub <= mod_) {
          noneset.almost_add(lb,ub);
        } else if (ub < 0 || lb > mod_) {
          noneset.almost_add(mod_val(lb),mod_val(ub));
        } else {
          if (lb < 0) {
            noneset.almost_add(mod_val(lb),mod_);
            lb = 0;
          }
          if (ub > mod_) {
            noneset.almost_add(0,mod_val(ub));
            ub = mod_;
          }
          noneset.almost_add(lb,ub);
        }
      }
    }
  }

  public void none_bands(IntervalSet noneset, Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
      int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic) {
    List<Integerval> bands_int = new ArrayList<Integerval>();
    kinematic_bands_combine(bands_int,conflict_det,recovery_det,time_step(ownship),B,T,0,B,
        maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv); 
    toIntervalSet(noneset,bands_int,get_step(),own_val(ownship),min_val(ownship),max_val(ownship));
  }

  public boolean any_red(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
      int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic) {
    return any_int_red(conflict_det,recovery_det,time_step(ownship),B,T,0,B,
        maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0);
  }

  public boolean all_red(Detection3D conflict_det, Optional<Detection3D> recovery_det, TrafficState repac, 
      int epsh, int epsv, double B, double T, TrafficState ownship, List<TrafficState> traffic) {
    return all_int_red(conflict_det,recovery_det,time_step(ownship),B,T,0,B,
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

  private void compute_none_bands(IntervalSet noneset, KinematicBandsCore core, TrafficState repac, List<TrafficState> preventive) {
    none_bands(noneset,core.detector,Detection3D.NoDetector,repac,core.epsilonH(),core.epsilonV(),0,core.alertingTime(),core.ownship,preventive);
    IntervalSet noneset2 = new IntervalSet();
    none_bands(noneset2,core.detector,Detection3D.NoDetector,repac,core.epsilonH(),core.epsilonV(),0,core.lookahead_time,core.ownship,core.correctiveAircraft());
    noneset.almost_intersect(noneset2);
  }

  public String toString() {
    String s = "";
    for (int i = 0; i < intervals_.size(); ++i) {
      s+=intervals_.get(i).toString(4)+" "+regions_.get(i)+"\n";
    } 
    s+="Time to recovery: "+f.Fm4(recovery_time_)+ " [s]";
    return s;
  }

  public String toPVS(int prec) {
    String s = "((:";
    for (int i = 0; i < intervals_.size(); ++i) {
      if (i > 0) { 
        s+=", ";
      } else {
        s+=" ";
      }
      s+=intervals_.get(i).toPVS(prec);
    } 
    s+=" :), (:";
    for (int i = 0; i < regions_.size(); ++i) {
      if (i > 0) {
        s+=", ";
      } else {
        s+=" ";
      }
      s += regions_.get(i).toPVS();
    } 
    s+=" :), "+f.FmPrecision(recovery_time_,prec)+"::ereal)";
    return s;
  }

}
