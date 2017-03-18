/*
 * CDSI Core - The core algorithms for conflict detection between an
 * ownship state vector and a traffic aircraft with a flight plan.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;

import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Projection;
//import gov.nasa.larcfm.Util.SimpleProjection;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.GreatCircle;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.f;
//import gov.nasa.larcfm.Util.f;

/** 
 * This class implements the core algorithms for conflict detection
 * between an ownship (modeled with a state vector) and a traffic
 * aircraft trajectory (modeled with a intent).  This class assumes
 * all inputs are in internal units.  As such, this class is most
 * appropriate to be part of a larger system, not as a stand-alone
 * conflict detection algorithm.  For that functionality, see the class
 * CDSI.<p>
 *
 * This class can be used two ways: through an object or through a
 * static method.  The static method approach has the advantage that
 * an object does not need to be created.  The object approach has the
 * advantage that more information is available.  Specifically, the
 * start and end times for every conflict are available in the object
 * approach. <p>
 *
 * In addition, for each of these types of use, the aircraft can be
 * specified in two different coordinate systems: a Euclidean space
 * and a latitude and longitude reference frame. <p>
 */
public final class CDSICore implements ErrorReporter, Detection3DAcceptor {  

	private static CDSICore def = new CDSICore();

	private ArrayList<Double> tin;
	private ArrayList<Double> tout;
	private ArrayList<Double> tca;
	private ArrayList<Double> dist_tca;
	private ArrayList<Integer> segin;
	private ArrayList<Integer> segout;
	private CDSSCore cdsscore;
	private ErrorLog error;

	//  private static final boolean allowVariableDistanceBuffer = true; // set to true to allow a per-leg variation on D,H, based on values in the plan(s)

	//  /** Can the distance parameter change from leg to leg? */
	//  public static boolean allowVariableDistance() { return allowVariableDistanceBuffer; }

	/** 
	 * Create a new conflict detection (state information for the ownship
	 * and intent information for the traffic) object with the given
	 * parameters for the minimum horizontal distance and the minimum
	 * vertical information.
	 */
	public CDSICore() {
		cdsscore = new CDSSCore(new CDCylinder(), 0.0);
		tin = new ArrayList<Double>(10);
		tout = new ArrayList<Double>(10);
		tca = new ArrayList<Double>(10);
		dist_tca = new ArrayList<Double>(10);
		segin = new ArrayList<Integer>(10);
		segout = new ArrayList<Integer>(10);
		error = new ErrorLog("CDSICore");
	}

	public CDSICore(Detection3D cd) {
		cdsscore = new CDSSCore(cd, 0.0);
		tin = new ArrayList<Double>(10);
		tout = new ArrayList<Double>(10);
		tca = new ArrayList<Double>(10);
		dist_tca = new ArrayList<Double>(10);
		segin = new ArrayList<Integer>(10);
		segout = new ArrayList<Integer>(10);
		error = new ErrorLog("CDSICore");
	}

	//  /** Returns the minimum horizontal separation distance */
	//  public double getDistance() {
	//    return cdss.getDistance();
	//  }
	//
	//  /** Returns the minimum vertical separation distance */
	//  public double getHeight() {
	//    return cdss.getHeight();
	//  }

	/**
	 * Returns the conflict detection filter time.
	 * 
	 * @return the conflict detection filter time seconds
	 */
	public double getFilterTime() {
		return cdsscore.getFilterTime();
	}

	//  /** Sets the minimum horizontal separation distance. The distance must be greater than 0. */
	//  public void setDistance(double distance) {
	//    cdss.setDistance(Util.max(distance, 1e-6));
	//  }
	//
	//  /** Sets the minimum vertical separation distance.  The height must be greater than 0.  */
	//  public void setHeight(double height) {
	//    cdss.setHeight(Util.max(height, 1e-6));
	//  }


	/**
	 * Sets the conflict detection filter time.
	 * 
	 * @param cdfilter the conflict detection filter time in seconds.
	 */
	public void setFilterTime(double cdfilter) {
		cdsscore.setFilterTime(cdfilter);
	}

	/** Returns the number of conflicts */
	public int size() {
		return tin.size();
	}

	/** Returns if there were any conflicts */
	public boolean conflict() {
		return tin.size() > 0;
	}

	/** 
	 * Returns the start time of the conflict.  This value is in absolute time.
	 * If there was not a conflict, then this value is meaningless.
	 * @param i the i-th conflict, must be between 0..size()-1
	 */
	public double getTimeIn(int i) {
		if (i < 0 || i >= tin.size()) return Double.POSITIVE_INFINITY;
		return tin.get(i);
	}

	/** 
	 * Returns the end time of the conflict.  This value is in absolute time.
	 * If there was not a conflict, then this value is meaningless.
	 * @param i the i-th conflict, must be between 0..size()-1
	 */
	public double getTimeOut(int i) {
		if (i < 0 || i >= tout.size()) return Double.NEGATIVE_INFINITY;
		return tout.get(i);
	}

	/** 
	 * Returns the segment number in the traffic aircraft's flight plan.
	 * If there was not a conflict, then this value is meaningless.
	 * @param i the i-th conflict, must be between 0..size()-1
	 */
	public int getSegmentIn(int i) {
		if (i < 0 || i >= tin.size()) return 0;
		return segin.get(i);
	}

	/** 
	 * Returns the segment number in the traffic aircraft's flight plan.
	 * If there was not a conflict, then this value is meaningless.
	 * @param i the i-th conflict, must be between 0..size()-1
	 */
	public int getSegmentOut(int i) {
		if (i < 0 || i >= tin.size()) return 0;
		return segout.get(i);
	}

	/** 
	 * Returns an estimate of the time of closest approach.  This value is in absolute time 
	 * (not relative from a waypoint).  This point approximates the point where the two aircraft
	 * are closest.  The definition of closest is not simple.  Specifically, space in the vertical
	 * dimension counts more than space in the horizontal dimension: encroaching in the protected
	 * zone 100 vertically is much more serious than encroaching 100 ft. horizontally. 
	 * 
	 * @param i the i-th conflict, must be between 0..size()-1
	 */
	public double getTimeClosest(int i) {
		if (i < 0 || i >= tin.size()) return 0.0;
		return tca.get(i);
	}

	/** 
	 * Returns the cylindrical distance at the time of closest approach. 
	 * @param i the i-th conflict, must be between 0..size()-1
	 */
	public double getDistanceClosest(int i) {
		if (i < 0 || i >= tin.size()) return 0.0;
		return dist_tca.get(i);
	}



	/**
	 * Returns if there is a conflict between two aircraft: the state
	 * aircraft and the intent aircraft.  The state aircraft is
	 * assumed to move linearly from it position.  The intent aircraft
	 * is assumed to move according to the given flight plan.  Both
	 * aircraft are assumed to be represented in a cartesian reference
	 * frame. <p>
	 *
	 * @param so the position of the state aircraft
	 * @param vo the velocity of the state aircraft
	 * @param t0 the time of the state aircraft when located at (sx,sy,sz).  This can be 0.0 to represent "now"
	 * @param state_horizon the largest time where the state aircraft's position can be predicted.
	 * @param intent the flight plan of the intent aircraft
	 * @param D the minimum horizontal separation distance
	 * @param H the minimum vertical separation distance
	 * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
	 * @param T the time to end looking for conflicts relative to t0
	 * @return true if there is a conflict
	 */
	public static boolean cdsicore_xyz(Vect3 so, Velocity vo, Detection3D cd, double t0, double state_horizon, Plan intent, 
			double B, double T) {
		def.setCoreDetection(cd);
		//    def.setDistance(D);
		//    def.setHeight(H);
		return def.detectionXYZ(so, vo, t0, state_horizon, intent, B, T);
	}

	/**
	 * Returns if there is a conflict between two aircraft: the state
	 * aircraft and the intent aircraft.  The state aircraft is
	 * assumed to move linearly from it position.  The intent aircraft
	 * is assumed to move according to the given flight plan.  Both
	 * aircraft are assumed to be represented in a latitude/longitude reference
	 * frame. <p>
	 *
	 * @param so the latitude/longitude/altitude of the state aircraft
	 * @param vo the velocity of the state aircraft
	 * @param t0 the time of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
	 * @param state_horizon the largest time where the state aircraft's position can be predicted.
	 * @param intent the flight plan of the intent aircraft
	 * @param D the minimum horizontal separation distance
	 * @param H the minimum vertical separation distance
	 * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
	 * @param T the time to end looking for conflicts relative to t0
	 * @return true if there is a conflict
	 */
	public static boolean cdsicore_ll(LatLonAlt so, Velocity vo, Detection3D cd, double t0, double state_horizon,
			Plan intent, double B, double T) {
		def.setCoreDetection(cd);
		//    def.setDistance(D);
		//    def.setHeight(H);
		return def.detectionLL(so, vo, t0, state_horizon, intent, B, T);
	}

	/**
	 * Returns if there is a conflict between two aircraft: the state
	 * aircraft and the intent aircraft.  The state aircraft is
	 * assumed to move linearly from it position.  The intent aircraft
	 * is assumed to move according to the given flight plan. <p>
	 *
	 * @param so the Position of the state aircraft
	 * @param vo the velocity of the state aircraft
	 * @param t0 the time of the state aircraft when located at so with respect to the traffic intent.  This can be 0.0 to represent "now"
	 * @param state_horizon the largest time where the state aircraft's position can be predicted.
	 * @param intent the flight plan of the intent aircraft
	 * @param D the minimum horizontal separation distance
	 * @param H the minimum vertical separation distance
	 * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
	 * @param T the time to end looking for conflicts relative to t0
	 * @return true if there is a conflict
	 */
	public static boolean cdsicore(Position so, Velocity vo, Detection3D cd, double t0, double state_horizon, Plan intent, double B, double T) {
		//      def.setDistance(D);
		//      def.setHeight(H);
		def.setCoreDetection(cd);
		return def.detection(so, vo, t0, state_horizon, intent, B, T);
	}

	/**
	 * Return true if the given position and velocity are in violation with the intent aircraft at time tm.
	 * If tm is outisde the intent information, return false; 
	 * @param so
	 * @param vo
	 * @param intent
	 * @param tm
	 * @return true if violation
	 */
	public boolean violation(Position so, Velocity vo, Plan intent, double tm) {
		if (tm < intent.getFirstTime() || tm > intent.getLastTime()) {
			return false;
		}
		return cdsscore.violation(so, vo, intent.position(tm), intent.velocity(tm));
	}

	/**
	 * Returns if there is a conflict between two aircraft: the state
	 * aircraft and the intent aircraft.  The state aircraft is
	 * assumed to move linearly from it position.  The intent aircraft
	 * is assumed to move according to the given flight plan. <p>
	 *
	 * @param so the position of the state aircraft
	 * @param vo velocity of the state aircraft
	 * @param t0 the time of the state aircraft when located at so with respect to the traffic intent.  This can be 0.0 to represent "now"
	 * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
	 * @param intent the flight plan of the intent aircraft
	 * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
	 * @param T the time to end looking for conflicts relative to t0
	 * @return true if there is a conflict
	 */
	public boolean detection(Position so, Velocity vo, double t0, double state_horizon, Plan intent, double B, double T) {
		if (so.isLatLon()) {
			return detectionLL(so.lla(), vo, t0, state_horizon, intent, B, T);
		} else {
			return detectionXYZ(so.point(), vo, t0, state_horizon, intent, B, T);
		}
	}


	/**
	 * Returns if there is a conflict between two aircraft: the state
	 * aircraft and the intent aircraft.  The state aircraft is
	 * assumed to move linearly from it position.  The intent aircraft
	 * is assumed to move according to the given flight plan.  Both
	 * aircraft are assumed to be represented in a Euclidean reference
	 * frame at the time t0. <p>
	 *
	 * @param so the position of the state aircraft
	 * @param vo velocity of the state aircraft
	 * @param t0 the time of the state aircraft when located at (so.x,so.y,so.z).  This can be 0.0 to represent "now"
	 * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
	 * @param intent the flight plan of the intent aircraft
	 * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
	 * @param T the time to end looking for conflicts relative to t0
	 * @return true if there is a conflict
	 */
	public boolean detectionXYZ(Vect3 so, Velocity vo, double t0, double state_horizon, Plan intent, double B, double T) {
		//	  f.pln("cdsicore.detectionXYZ B="+B+" T="+T);
		tin.clear();
		tout.clear();
		segin.clear();
		segout.clear();
		tca.clear();
		dist_tca.clear();

		boolean linear = true;     // was Plan.PlanType.LINEAR);

		double t_base;
		int start_seg;
		boolean cont = false; 
		double BT = 0.0;
		double NT = 0.0;
		double HT = 0.0;

		if (t0 < intent.time(0)) {
			t_base = intent.time(0);
			start_seg = 0;
		} else {
			t_base = t0;
			start_seg = intent.getSegment(t0);
			if (start_seg < 0) {
				return false;  // t0 past end of Flight Plan 
			}
		}  

		//    double d = getDistance();
		//    double h = getHeight();

		//System.out.println("$$CDSICoreXYZ1 intent size: "+intent.size()+" t_base: "+t_base);
		for (int j = start_seg; j < intent.size(); j++){ 
			// invariant: t0 <= t_base

			Vect3 sop = so.AddScal((t_base - t0),vo);
			Vect3 sip = intent.position(t_base,linear).point(); // intent.positionXYZ(j).AddScal((t_base - intent.getTime(j)),vi);
			//      Vect3 s = sop.Sub(sip);

			//      if (allowVariableDistanceBuffer) {
			//        cdss.setDistance(d+intent.getSegmentDistanceBuffer(j));
			//        cdss.setHeight(h+intent.getSegmentHeightBuffer(j));
			//      }


			if (j == intent.size() - 1) { 
				continue;
			} else { // normal case
				HT = Util.max(0.0, Util.min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
				BT = Util.max(0.0, B + t0 - t_base);
				NT = cont ? HT : T + t0 - t_base;
				//NT = HT;

				//System.out.println("$$CDSICoreXYZ2 positions: "+sop+" "+sip);
				//System.out.println("$$CDSICoreXYZ3 rel pos and vo: "+s+" "+vo);
				//Vect3 sp = s.AddScal(1.0,vo.Sub(intent.velocity(j)));
				//Vect3 spp = sp.AddScal(1.0, vo.Sub(intent.velocity(j+1)));
				//System.out.println("$$CDSICoreXYZ3B distance: "+sp.vect2().norm()+" "+spp.vect2().norm());
				//System.out.println("$$CDSICoreXYZ4 times: j="+j+" t_base="+t_base+" Bp="+BT+" Tp="+NT+" t_horiz="+HT+" "+cont);
				//        System.out.println("CDSICore.detectionXYZ: vo="+vo+" vi="+intent.initialVelocity(j));
				if (NT >= 0) {
					//cdsscore.setTimeHorizon(HT);
					if (cdsscore.detectionBetween(sop, vo, sip, intent.initialVelocity(j, linear), BT, NT, HT) ) {
						// filter out when the start time for searching is the the end time of the conflict
						if (Math.abs((t0+B) - (cdsscore.getTimeOut()+t_base)) > 0.0000001) { 
							captureOutput(t_base, j);
						}
					}     
				}
				cont = size() > 0 ? tout.get(tout.size()-1) == HT + t_base : false;     
				t_base = intent.time(j+1);        
			}
		}

		//    if (allowVariableDistanceBuffer) {
		//      cdss.setDistance(d);
		//      cdss.setHeight(h);
		//    }

		merge();

		return conflict();
	}



	/**
	 * Returns if there is a conflict between two aircraft: the state
	 * aircraft and the intent aircraft.  The state aircraft is
	 * assumed to move linearly from it position.  The intent aircraft
	 * is assumed to move according to the given flight plan.  Both
	 * aircraft are assumed to be represented in a latitude/longitude reference
	 * frame. <p>
	 *
	 * @param so the latitude/longitude/altitude of the state aircraft
	 * @param vo velocity of the state aircraft
	 * @param t0 the time of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
	 * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
	 * @param intent the flight plan of the intent aircraft
	 * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
	 * @param T the time to end looking for conflicts relative to t0
	 * @return true if there is a conflict
	 */
	public boolean detectionLL(LatLonAlt so, Velocity vo, double t0, double state_horizon, Plan intent, double B, double T) {
		tin.clear();
		tout.clear();
		segin.clear();
		segout.clear();
		tca.clear();
		dist_tca.clear();

		boolean linear = true;     // was Plan.PlanType.LINEAR);

		double t_base;
		int start_seg;
		boolean cont = false;
		double BT = 0.0;
		double NT = 0.0;
		double HT = 0.0;

		if (t0 < intent.time(0)) {
			t_base = intent.time(0);
			start_seg = 0;
		} else {
			t_base = t0;
			start_seg = intent.getSegment(t0);
			if (start_seg < 0) {
				return false;  // t0 past end of Flight Plan 
			}
		}  
		//    double d = getDistance();
		//    double h = getHeight();    
		// System.out.println("$$CDSICoreLL1: intent size: "+intent.size()+" so:"+so+" t_base: "+t_base+" B="+B+" T="+T);
		for (int j = start_seg; j < intent.size(); j++){ 
			// invariant: t0 <= t_base
			//System.out.println("GET THIS!!!!! ---> "+so+"  "+vo+"  "+(t_base-t0));      
			LatLonAlt so2p = GreatCircle.linear_initial(so, vo, t_base-t0);  //CHANGED!!!
			LatLonAlt sip = intent.position(t_base,linear).lla();
			EuclideanProjection proj = Projection.createProjection(so.zeroAlt()); // CHECK THIS!!!  Should it be so2p?
			Vect3 so3 = proj.project(so2p);
			Vect3 si3 = proj.project(sip);      
			if (j == intent.size() - 1) { 
				continue; // leave loop
			} else { // normal case
				HT = Util.max(0.0, Util.min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
				BT = Util.max(0.0, B + t0 - t_base);
				NT = cont ? HT : T + t0 - t_base;      
				//      Velocity vop = vo;
				//      Velocity vip = intent.initialVelocity(t_base);
				Velocity vop = proj.projectVelocity(so2p, vo);  //CHANGED!!!
				Velocity vip = proj.projectVelocity(sip, intent.velocity(t_base, linear));
				//f.pln("$$ CDSICore LL2 positions: "+so2p+" "+sip);
				//f.pln("$$ CDSICore LL5 times: BT="+BT+" NT="+NT+" HT="+HT+" cont="+cont);
				if (NT >= 0) {
					//f.pln(" $$$$ CDSICore: HT = "+HT+" BT = "+BT+" NT = "+NT);
					//cd.setTimeHorizon(HT);
					if ( cdsscore.detectionBetween(so3, vop, si3, vip, BT, NT, HT) ) {  // CHANGED!!!
						// filter out when the start time for searching is the the end time of the conflict
						if (Math.abs((t0+B) - (cdsscore.getTimeOut()+t_base)) > 0.0000001) { 
							captureOutput(t_base, j);
						}
					} 
				}
				cont = size() > 0 ? tout.get(tout.size()-1) == HT + t_base : false;     
				t_base = intent.time(j+1);        
			}
		}

		//    if (allowVariableDistanceBuffer) {
		//      cdss.setDistance(d);
		//      cdss.setHeight(h);
		//    }

		merge();

		return conflict();
	}


	private void captureOutput(double t_base, int seg) {
		segin.add(seg);
		segout.add(seg);
		tin.add(cdsscore.getTimeIn()+t_base);
		tout.add(cdsscore.getTimeOut()+t_base);
		tca.add(cdsscore.timeOfClosestApproach() + t_base);
		dist_tca.add(cdsscore.distanceAtCriticalTime());
	}


	private void merge() {
		int i = 0;
		//double D = cdss.getDistance();
		//double H = cdss.getHeight();
		while ( i < segin.size() - 1){
			//System.out.println("Compare out_i in_i+1: "+tout.get(i)+" "+tin.get(i+1));     
			if ( ! Util.almost_less(tout.get(i), tin.get(i+1), Util.PRECISION7)) {
				//System.out.println("HERE");
				tin.remove(i+1);
				segin.remove(i+1);
				tout.remove(i);
				segout.remove(i);
				if (dist_tca.get(i) < dist_tca.get(i+1)) {
					tca.remove(i+1);
					dist_tca.remove(i+1);
					//dv.remove(i+1);
				} else {
					tca.remove(i);
					dist_tca.remove(i);
					//dv.remove(i);
				}
			} else {
				i++;
			}
		}
	}    

	/**
	 * EXPERIMENTAL
	 * @param so
	 * @param vo
	 * @param t0
	 * @param state_horizon
	 * @param intent
	 * @param B
	 * @param T
	 * @return true if conflict
	 */
	public boolean conflictXYZ(Vect3 so, Velocity vo, double t0, double state_horizon, Plan intent, double B, double T) {
		boolean linear = true;     // was Plan.PlanType.LINEAR);
		double t_base;
		int start_seg;
		double BT = 0.0;
		double NT = 0.0;
		double HT = 0.0;

		if (t0 < intent.time(0)) {
			t_base = intent.time(0);
			start_seg = 0;
		} else {
			t_base = t0;
			start_seg = intent.getSegment(t0);
			if (start_seg < 0) {
				return false;  // t0 past end of Flight Plan 
			}
		}  
		for (int j = start_seg; j < intent.size(); j++){ 
			Vect3 sop = so.AddScal((t_base - t0),vo);
			Vect3 sip = intent.position(t_base,linear).point(); // intent.positionXYZ(j).AddScal((t_base - intent.getTime(j)),vi);
			if (j == intent.size() - 1) { 
				continue;
			} else { // normal case
				HT = Util.max(0.0, Util.min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
				BT = Util.max(0.0, B + t0 - t_base);
				NT = T + t0 - t_base;
				if (NT >= 0) {
					if (cdsscore.conflict(sop, vo, sip, intent.initialVelocity(j, linear), BT, Util.min(NT, HT)) ) return true;
				}
				t_base = intent.time(j+1);        
			}
		}
		return false;
	}



	/**
	 * EXPERIMENTAL
	 */
	public boolean conflictLL(LatLonAlt so, Velocity vo, double t0, double state_horizon, Plan intent, double B, double T) {
		boolean linear = true;     // was Plan.PlanType.LINEAR);

		double t_base;
		int start_seg;
		double BT = 0.0;
		double NT = 0.0;
		double HT = 0.0;

		if (t0 < intent.time(0)) {
			t_base = intent.time(0);
			start_seg = 0;
		} else {
			t_base = t0;
			start_seg = intent.getSegment(t0);
			if (start_seg < 0) {
				return false;  // t0 past end of Flight Plan 
			}
		}  
		for (int j = start_seg; j < intent.size(); j++){ 
			LatLonAlt so2p = GreatCircle.linear_initial(so, vo, t_base-t0);  //CHANGED!!!
			LatLonAlt sip = intent.position(t_base,linear).lla();
			EuclideanProjection proj = Projection.createProjection(so.zeroAlt()); // CHECK THIS!!!  Should it be so2p?
			Vect3 so3 = proj.project(so2p);
			Vect3 si3 = proj.project(sip);      
			if (j == intent.size() - 1) { 
				continue; // leave loop
			} else { // normal case
				HT = Util.max(0.0, Util.min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
				BT = Util.max(0.0, B + t0 - t_base);
				NT = T + t0 - t_base;      
				Velocity vop = proj.projectVelocity(so2p, vo);  //CHANGED!!!
				Velocity vip = proj.projectVelocity(sip, intent.velocity(t_base, linear));
				if (NT >= 0) {
					if ( cdsscore.conflict(so3, vop, si3, vip, BT, Util.min(NT, HT)) ) return true;
				}
				t_base = intent.time(j+1);        
			}
		}
		return false;
	}


	/**
	 * EXPERIMANTAL
	 * Return the time of closest approach and "distance" at that time (as defined by the detection algorithm), or a negative value if they never overlap
	 * Eventually blend this into the main detection with a flag?
	 * @return tca and distance
	 */
	public Pair<Double,Double>urgency(Position so, Velocity vo, double t0, double state_horizon, Plan intent, double B, double T) {
		if (so.isLatLon()) return urgencyLL(so.lla(), vo, t0, state_horizon, intent, B, T);
		else return urgencyXYZ(so.point(), vo, t0, state_horizon, intent, B, T);
	}

	public Pair<Double,Double>urgencyXYZ(Vect3 so, Velocity vo, double t0, double state_horizon, Plan intent, double B, double T) {
		boolean linear = true;     // was Plan.PlanType.LINEAR);
		double t_base;
		int start_seg;
		boolean cont = false; 
		double BT = 0.0;
		double NT = 0.0;
		double HT = 0.0;

		double dist = Double.MAX_VALUE;
		double tca = -1.0;

		if (t0 < intent.time(0)) {
			t_base = intent.time(0);
			start_seg = 0;
		} else {
			t_base = t0;
			start_seg = intent.getSegment(t0);
			if (start_seg < 0) {
				return Pair.make(tca,dist);  // no such info. 
			}
		}  
		for (int j = start_seg; j < intent.size(); j++){ 
			Vect3 sop = so.AddScal((t_base - t0),vo);
			Vect3 sip = intent.position(t_base,linear).point(); // intent.positionXYZ(j).AddScal((t_base - intent.getTime(j)),vi);
			if (j == intent.size() - 1) { 
				continue;
			} else { // normal case
				HT = Util.max(0.0, Util.min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
				BT = Util.max(0.0, B + t0 - t_base);
				NT = cont ? HT : T + t0 - t_base;
				if (NT >= 0) {
					cdsscore.detectionBetween(sop, vo, sip, intent.initialVelocity(j, linear), BT, NT, HT);
					double mydist = cdsscore.distanceAtCriticalTime();
					double mytca = cdsscore.timeOfClosestApproach();
					if (mydist < dist) {
						dist = mydist;
						tca = mytca;
					}

				}
				cont = size() > 0 ? tout.get(tout.size()-1) == HT + t_base : false;     
				t_base = intent.time(j+1);        
			}
		}
		return Pair.make(tca,dist);
	}

	public Pair<Double,Double>urgencyLL(LatLonAlt so, Velocity vo, double t0, double state_horizon, Plan intent, double B, double T) {
		boolean linear = true;     // was Plan.PlanType.LINEAR);

		double t_base;
		int start_seg;
		boolean cont = false;
		double BT = 0.0;
		double NT = 0.0;
		double HT = 0.0;

		double dist = Double.MAX_VALUE;
		double tca = -1.0;

		if (t0 < intent.time(0)) {
			t_base = intent.time(0);
			start_seg = 0;
		} else {
			t_base = t0;
			start_seg = intent.getSegment(t0);
			if (start_seg < 0) {
				return Pair.make(tca,dist);
			}
		}  
		for (int j = start_seg; j < intent.size(); j++){ 
			LatLonAlt so2p = GreatCircle.linear_initial(so, vo, t_base-t0);  //CHANGED!!!
			LatLonAlt sip = intent.position(t_base,linear).lla();
			EuclideanProjection proj = Projection.createProjection(so.zeroAlt()); // CHECK THIS!!!  Should it be so2p?
			Vect3 so3 = proj.project(so2p);
			Vect3 si3 = proj.project(sip);      
			if (j == intent.size() - 1) { 
				continue; // leave loop
			} else { // normal case
				HT = Util.max(0.0, Util.min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
				BT = Util.max(0.0, B + t0 - t_base);
				NT = cont ? HT : T + t0 - t_base;      
				Velocity vop = proj.projectVelocity(so2p, vo);  //CHANGED!!!
				Velocity vip = proj.projectVelocity(sip, intent.velocity(t_base, linear));
				if (NT >= 0) {
					cdsscore.detectionBetween(so3, vop, si3, vip, BT, NT, HT);
					double mydist = cdsscore.distanceAtCriticalTime();
					double mytca = cdsscore.timeOfClosestApproach();
					if (mydist < dist) {
						dist = mydist;
						tca = mytca;
					}
				}
				cont = size() > 0 ? tout.get(tout.size()-1) == HT + t_base : false;     
				t_base = intent.time(j+1);        
			}
		}

		return Pair.make(tca,dist);
	}


	public String toString() {
		return "CDSICore: cd = "+cdsscore.toString();
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

	public void setCoreDetection(Detection3D d) {
		cdsscore.setCoreDetection(d);
		def.cdsscore.setCoreDetection(d);
		tin.clear();
		tout.clear();
		tca.clear();
		dist_tca.clear();
		segin.clear();
		segout.clear();
	}

	public Detection3D getCoreDetection() {
		return cdsscore.getCoreDetection();
	}

}

