/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;
import java.util.List;

public class KinematicBandsCore {

	public static TCASTable RA = new TCASTable();

	/* Absolute ownship state */
	public TrafficState ownship;
	/* Absolute list of traffic states */
	public List<TrafficState> traffic;
	/* Kinematic bands parameters */
	public KinematicBandsParameters parameters;
	/* Most urgent aircraft */
	public TrafficState most_urgent_ac; 

	/* Boolean to control re-computation of cached values */
	private boolean outdated_; 
	/* Cached horizontal epsilon for implicit coordination */
	private int epsh_; 
	/* Cached vertical epsilon for implicit coordination */
	private int epsv_; 
	/* The length of conflict_acs_ is greater than or equal to the length of the alert levels. */
	/* Cached list of conflict aircraft per alert level */
	private List<List<TrafficState>> conflict_acs_; 
	/* Cached list of time intervals of violation per alert level */
	private List<Interval> tiov_; //
	/* Last conflict level */
	private int last_conflict_level_;

	public KinematicBandsCore(KinematicBandsParameters params) {
		ownship = TrafficState.INVALID;
		traffic = new ArrayList<TrafficState>(); 
		parameters = new KinematicBandsParameters(params);
		most_urgent_ac = TrafficState.INVALID;
		conflict_acs_ = new ArrayList<List<TrafficState>>();
		tiov_ = new ArrayList<Interval>();
		last_conflict_level_ = 0;
		reset();
	}

	public KinematicBandsCore(KinematicBandsCore core) {
		setKinematicBandsCore(core);
	}

	/**
	 * Set kinematic bands core
	 */
	public void setKinematicBandsCore(KinematicBandsCore core) {
		ownship = core.ownship;
		traffic = new ArrayList<TrafficState>(); 
		traffic.addAll(core.traffic);
		parameters = new KinematicBandsParameters(core.parameters);
		most_urgent_ac = core.most_urgent_ac;
		conflict_acs_ = new ArrayList<List<TrafficState>>();
		tiov_ = new ArrayList<Interval>();
		last_conflict_level_ = 0;
		reset();	
	}

	/**
	 *  Clear ownship and traffic data from this object.   
	 */
	public void clear() {
		ownship = TrafficState.INVALID;
		traffic.clear();
		reset();
	}

	/**
	 *  Reset cached values 
	 */
	public void reset() {
		outdated_ = true;
		epsh_ = 0;
		epsv_ = 0;
		tiov_.clear();
		last_conflict_level_ = 0;
	}

	/**
	 *  Update cached values 
	 */
	private void update() {
		if (outdated_) {
			last_conflict_level_ = 0;
			for (int alert_level=1; alert_level <= parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
				if (alert_level-1 >= conflict_acs_.size()) {
					conflict_acs_.add(new ArrayList<TrafficState>());
				} else {
					conflict_acs_.get(alert_level-1).clear();
				}				
				if (parameters.alertor.getLevel(alert_level).getRegion().isConflictBand()) {
					conflict_aircraft(alert_level);
				}
				if (!conflict_acs_.get(alert_level-1).isEmpty()) {
					last_conflict_level_ = alert_level;
				}
			}
			epsh_ = epsilonH(ownship,most_urgent_ac);
			epsv_ = epsilonV(ownship,most_urgent_ac);
			outdated_ = false;
		} 
	}

	/** 
	 * Returns most severe alert level where there is a conflict aircraft
	 */
	public int lastConflictAlertLevel() {
		update();
		return last_conflict_level_;
	}

	/**
	 *  Returns horizontal epsilon for implicit coordination with respect to criteria ac
	 */
	public int epsilonH() {
		update();
		return epsh_;
	}

	/**
	 *  Returns vertical epsilon for implicit coordination with respect to criteria ac
	 */
	public int epsilonV() {
		update();
		return epsv_;
	}

	/**
	 * Returns actual minimum horizontal separation for recovery bands in internal units. 
	 */
	public double minHorizontalRecovery() {
		double min_horizontal_recovery = parameters.getMinHorizontalRecovery();
		if (min_horizontal_recovery > 0) 
			return min_horizontal_recovery;
		int sl = !hasOwnship() ? 3 : Math.max(3,TCASTable.getSensitivityLevel(ownship.getPosition().alt()));
		return RA.getHMD(sl);
	}

	/** 
	 * Returns actual minimum vertical separation for recovery bands in internal units. 
	 */
	public double minVerticalRecovery() {
		double min_vertical_recovery = parameters.getMinVerticalRecovery();
		if (min_vertical_recovery > 0) 
			return min_vertical_recovery;
		int sl = !hasOwnship() ? 3 : Math.max(3,TCASTable.getSensitivityLevel(ownship.getPosition().alt()));
		return RA.getZTHR(sl);
	}

	public boolean hasOwnship() {
		return ownship.isValid();
	}

	public TrafficState intruder(String id) {
		return TrafficState.findAircraft(traffic,id);
	}

	public boolean hasTraffic() {
		return traffic.size() > 0;
	}

	public Position trafficPosition(int i) {
		return traffic.get(i).getPosition();
	}

	public Velocity trafficVelocity(int i) {
		return traffic.get(i).getVelocity();
	}

	public Vect3 own_s() {
		return ownship.get_s();
	}

	public Velocity own_v() {
		return ownship.get_v();
	}

	public Vect3 traffic_s(int i) {
		return traffic.get(i).get_s();
	}

	public Velocity traffic_v(int i) {
		return traffic.get(i).get_v();
	}

	/**
	 * Put in conflict_acs_ the list of aircraft predicted to be in conflict for the given alert level.
	 * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
	 */
	private void conflict_aircraft(int alert_level) {
		Detection3D detector = parameters.alertor.getLevel(alert_level).getDetector();
		double tin  = Double.POSITIVE_INFINITY;
		double tout = Double.NEGATIVE_INFINITY;
		for (int i = 0; i < traffic.size(); ++i) {
			TrafficState ac = traffic.get(i);
			ConflictData det = detector.conflictDetection(own_s(),own_v(),ac.get_s(),ac.get_v(),
					0,parameters.getLookaheadTime());
			if (det.conflict()) {
				if (det.getTimeIn() <= parameters.alertor.getLevel(alert_level).getAlertingTime()) {
					conflict_acs_.get(alert_level-1).add(ac);
				} 
				tin = Math.min(tin,det.getTimeIn());
				tout = Math.max(tout,det.getTimeOut());
			} 
		}
		tiov_.add(new Interval(tin,tout));
	}

	/**
	 * Return list of conflict aircraft for a given alert level.
	 * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
	 */
	public List<TrafficState> conflictAircraft(int alert_level) {
		update();
		if (alert_level >= 1 && alert_level <= parameters.alertor.mostSevereAlertLevel()) {
			return conflict_acs_.get(alert_level-1);
		}
		return TrafficState.INVALIDL;
	}

	/**
	 * Return time interval of violation for given alert level
	 * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
	 */
	public Interval timeIntervalOfViolation(int alert_level) {
		update();
		if (alert_level >= 1 && alert_level <= parameters.alertor.mostSevereAlertLevel()) {
			return tiov_.get(alert_level-1);
		}
		return Interval.EMPTY;
	}

	public static int epsilonH(TrafficState ownship, TrafficState ac) {
		if (ownship.isValid() && ac.isValid()) {
			Vect2 s = ownship.get_s().Sub(ac.get_s()).vect2();
			Vect2 v = ownship.get_v().Sub(ac.get_v()).vect2();   
			return CriteriaCore.horizontalCoordination(s,v);
		} else {
			return 0;
		}
	}

	public static int epsilonV(TrafficState ownship, TrafficState ac) {
		if (ownship.isValid() && ac.isValid()) {
			Vect3 s = ownship.get_s().Sub(ac.get_s());
			return CriteriaCore.verticalCoordinationLoS(s,ownship.get_v(),ac.get_v(),
					ownship.getId(), ac.getId());
		} else {
			return 0;
		}
	}

	public TrafficState criteria_ac() {
		return parameters.isEnabledConflictCriteria() ? most_urgent_ac : TrafficState.INVALID;
	}

	public TrafficState recovery_ac() {
		return parameters.isEnabledRecoveryCriteria() ? most_urgent_ac : TrafficState.INVALID;
	}

	public String toString() {
		int precision = Constants.get_output_precision();
		String s="";
		s+="## KinematicBandsCore Parameters\n";
		s+=parameters.toString();
		s+="## KinematicBandsCore Internals\n";
		s+="outdated_ = "+outdated_+"\n";		
		s+="most_urgent_ac_ = "+most_urgent_ac.getId()+"\n";
		s+="epsh_ = "+f.Fmi(epsh_)+"\n";
		s+="epsv_ = "+f.Fmi(epsv_)+"\n";		
		for (int i=0; i < conflict_acs_.size(); ++i) {
			s+="conflict_acs_["+f.Fmi(i)+"]: "+
					TrafficState.listToString(conflict_acs_.get(i))+"\n";
		}
		for (int i=0; i < tiov_.size(); ++i) {
			s+="tiov_["+f.Fmi(i)+"]: "+
					tiov_.get(i).toString(precision)+"\n";
		}
		s+="## Ownship and Traffic\n";
		s+="NAME sx sy sz vx vy vz time\n";
		s+="[none] [m] [m] [m] [m/s] [m/s] [m/s] [s]\n";
		s+=ownship.getId()+", "+ownship.get_s().formatXYZ(precision,"",", ","")+
				", "+own_v().formatXYZ(precision,"",", ","")+", 0\n";
		for (int i = 0; i < traffic.size(); i++) {
			s+=traffic.get(i).getId()+", "+traffic_s(i).formatXYZ(precision,"",", ","")+
					", "+traffic_v(i).formatXYZ(precision,"",", ","")+", 0\n";
		}
		return s;
	}

}
