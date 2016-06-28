/*
 * CDIICore - the core algorithm for conflict detection between two aircraft with intent information for each.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.DebugSupport;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.f;

import java.text.*; // for DecimalFormat

/**
 * This class implements the core algorithms for conflict detection between an
 * ownship and a traffic aircraft trajectory, where both trajectories include
 * intent information. This class assumes all inputs are in internal units. As
 * such, this class is most appropriate to be part of a larger system, not as a
 * stand-alone conflict detection algorithm. For that functionality, see the
 * class CDII.
 * <p>
 *
 * This class can be used two ways: through an object or through a static
 * method. The static method approach has the advantage that an object does not
 * need to be created. The object approach has the advantage that more
 * information is available. Specifically, the start and end times for every
 * conflict are available in the object approach.
 * <p>
 * 
 * When using the object method, all method getTimeIn(), etc, assume that the
 * detection() method has been called first.
 * <p>
 */
public final class CDIICore implements ErrorReporter, Detection3DAcceptor {

	private static CDIICore def = new CDIICore();

	private ArrayList<Double> tin;
	private ArrayList<Double> tout;
	private ArrayList<Double> tcpa;
	private ArrayList<Double> dist_tca;
	private ArrayList<Integer> segin;
	private ArrayList<Integer> segout;
	private CDSICore cdsi;

	/**
	 * Create a new conflict detection (intent information for both the ownship
	 * and traffic) object.
	 *
	 * @param distance
	 *            the minimum horizontal separation distance
	 * @param height
	 *            the minimum vertical separation height
	 */
	public CDIICore(Detection3D cd) {
		tin = new ArrayList<Double>(10);
		tout = new ArrayList<Double>(10);
		tcpa = new ArrayList<Double>(10);
		dist_tca = new ArrayList<Double>(10);
		segin = new ArrayList<Integer>(10);
		segout = new ArrayList<Integer>(10);
		cdsi = new CDSICore(cd);
	}

	public CDIICore() {
		tin = new ArrayList<Double>(10);
		tout = new ArrayList<Double>(10);
		tcpa = new ArrayList<Double>(10);
		dist_tca = new ArrayList<Double>(10);
		segin = new ArrayList<Integer>(10);
		segout = new ArrayList<Integer>(10);
		cdsi = new CDSICore(new CDCylinder());
	}

	// /** Returns the minimum horizontal separation distance */
	// public double getDistance() {
	// return cdsi.getDistance();
	// }
	//
	// /** Returns the minimum vertical separation distance */
	// public double getHeight() {
	// return cdsi.getHeight();
	// }

	/**
	 * Returns the conflict detection filter time.
	 * 
	 * @return the conflict detection filter time
	 */
	public double getFilterTime() {
		return cdsi.getFilterTime();
	}

	// /** Sets the minimum horizontal separation distance */
	// public void setDistance(double distance) {
	// cdsi.setDistance(distance);
	// }
	//
	// /** Sets the minimum vertical separation distance */
	// public void setHeight(double height) {
	// cdsi.setHeight(height);
	// }

	/**
	 * Sets the conflict detection filter time.
	 * 
	 * @param cdfilter
	 *            the conflict detection filter time.
	 */
	public void setFilterTime(double cdfilter) {
		cdsi.setFilterTime(cdfilter);
	}

	/** Returns the number of conflicts */
	public int size() {
		return tin.size();
	}

	/** Returns if there were any conflicts. */
	public boolean conflict() {
		return tin.size() > 0;
	}

	/**
	 * Returns the start time of the conflict. This value is in absolute time.
	 * If there was not a conflict, then this value is meaningless.
	 * 
	 * @param i
	 *            the i-th conflict, must be between 0..size()-1
	 */
	public double getTimeIn(int i) {
		if (i < 0 || i >= tin.size())
			return 0.0;
		return tin.get(i);
	}

	/**
	 * Returns the end time of the conflict. This value is in absolute time. If
	 * there was not a conflict, then this value is meaningless.
	 * 
	 * @param i
	 *            the i-th conflict, must be between 0..size()-1
	 */
	public double getTimeOut(int i) {
		if (i < 0 || i >= tout.size())
			return 0.0;
		return tout.get(i);
	}

	/**
	 * Returns the segment number of the entry into a conflict from the ownship
	 * aircraft's flight plan. If there was not a conflict, then this value is
	 * meaningless.
	 * 
	 * @param i
	 *            the i-th conflict, must be between 0..size()-1
	 */
	public int getSegmentIn(int i) {
		if (i < 0 || i >= segin.size())
			return 0;
		return segin.get(i);
	}

	/**
	 * Returns the segment number of the exit from a conflict from the ownship
	 * aircraft's flight plan. If there was not a conflict, then this value is
	 * meaningless.
	 * 
	 * @param i
	 *            the i-th conflict, must be between 0..size()-1
	 */
	public int getSegmentOut(int i) {
		if (i < 0 || i >= segout.size())
			return 0;
		return segout.get(i);
	}

	/**
	 * Returns an estimate of the time of closest approach. This value is in
	 * absolute time (not relative from a waypoint). This point approximates the
	 * point where the two aircraft are closest. The definition of closest is
	 * not simple. Specifically, space in the vertical dimension counts more
	 * than space in the horizontal dimension: encroaching in the protected zone
	 * 100 vertically is much more serious than encroaching 100 ft.
	 * horizontally.
	 * 
	 * @param i
	 *            the i-th conflict, must be between 0..size()-1
	 */
	public double getTimeClosest(int i) {
		if (i < 0 || i >= tcpa.size())
			return 0.0;
		return tcpa.get(i);
	}

	/**
	 * Returns the distance index at the time of closest approach.
	 * 
	 * @param i
	 *            the i-th conflict, must be between 0..size()-1
	 */
	public double getDistanceClosest(int i) {
		if (i < 0 || i >= dist_tca.size())
			return 0.0;
		return dist_tca.get(i);
	}

	/**
	 * Is there a conflict at any time in the interval from start to end
	 * (inclusive). This method assumes that the detection() method has been
	 * called first.
	 *
	 * @param start
	 *            the time to begin looking for conflicts
	 * @param end
	 *            the time to end looking for conflicts
	 * @return true if there is a conflict
	 */
	public boolean conflictBetween(double start, double end) {
		boolean rtn = false;
		for (int k = 0; k < size(); k++) {
			double tmIn = getTimeIn(k);
			double tmOut = getTimeOut(k);
			if (start <= tmIn && tmIn < end) {
				rtn = true;
				break;
			}
			if (start < tmOut && tmOut <= end) {
				rtn = true;
				break;
			}
			if (tmIn <= start && end <= tmOut) {
				rtn = true;
				break;
			}
		}
		return rtn;
	}

	/**
	 * Returns if there is a conflict between two aircraft: the ownship and the
	 * traffic aircraft.
	 * <p>
	 *
	 * @param ownship
	 *            the trajectory intent of the ownship
	 * @param traffic
	 *            the trajectory intent of the traffic
	 * @param D
	 *            the minimum horizontal separation distance
	 * @param H
	 *            the minimum vertical separation distance
	 * @param B
	 *            the absolute time to start looking for conflicts
	 * @param T
	 *            the absolute time to end looking for conflicts
	 * @return true if there is a conflict
	 */
	public static boolean cdiicore(Plan ownship, Plan traffic, Detection3D cd,
			double B, double T) {
		def.setCoreDetection(cd);
		return def.detection(ownship, traffic, B, T);
	}

	/**
	 * Return true if there is a violation between two aircraft at time tm. If
	 * tm is outside either of the plans' times, this will return false.
	 * 
	 * @param ownship
	 * @param traffic
	 * @param tm
	 * @return
	 */
	public boolean violation(Plan ownship, Plan traffic, double tm) {
		if (tm < ownship.getFirstTime() || tm > ownship.getLastTime()) {
			return false;
		}
		return cdsi.violation(ownship.position(tm), ownship.velocity(tm),
				traffic, tm);
	}

	/**
	 * Returns if there is a conflict between two aircraft: the ownship and the
	 * traffic aircraft.
	 * <p>
	 *
	 * @param ownship
	 *            the trajectory intent of the ownship
	 * @param traffic
	 *            the trajectory intent of the traffic
	 * @param B
	 *            the absolute time to start looking for conflicts (no time
	 *            information will be returned prior to the start of the ownship
	 *            segment containing B)
	 * @param T
	 *            the absolute time to end looking for conflicts
	 * @return true if there is a conflict
	 */
	public boolean detection(Plan ownship, Plan traffic, double B, double T) {
		//f.pln(" $$$$ CDIICore.detection: B = "+B+" T = "+T);
		tin.clear();
		tout.clear();
		segin.clear();
		segout.clear();
		tcpa.clear();
		dist_tca.clear();
		if (ownship.isLatLon() != traffic.isLatLon()) {
			return false;
		}
		if (ownship.isLatLon()) {
			return detectionLL(ownship, traffic, B, T);
		} else {
			return detectionXYZ(ownship, traffic, B, T);
		}
	}
	
	/**
	 * This version calculates the "true" time in and time out for all conflicts
	 * that overlap with [B,T]. It is less efficient than the normal detection()
	 * algorithm and should only be called if accurate time in information is
	 * necessary when B might be within a loss of separation region.
	 */
	public boolean detectionExtended(Plan ownship, Plan traffic, double B, double T) {
		if (!detection(ownship, traffic,
				Math.max(ownship.getFirstTime(), traffic.getFirstTime()),
				Math.min(ownship.getLastTime(), traffic.getLastTime()))) {
			return false;
		}
		int i = 0;
		while (i < size()) {
			if (getTimeIn(i) > T || getTimeOut(i) < B) {
				tin.remove(i);
				tout.remove(i);
				segin.remove(i);
				segout.remove(i);
				tcpa.remove(i);
				dist_tca.remove(i);
			} else {
				i++;
			}
		}
		return size() > 0;
	}

	/** Returns true iff there is a loss of separation between position so and position si assuming cylinder parameters D,H
	 * 
	 * @param so ownship position
	 * @param si traffic position
	 * @param D  required horizontal separation
	 * @param H  required vertical separation
	 * @return
	 */
	static public boolean LoS(Position so, Position si, double D, double H) {
		boolean viol = false;
		double s = so.distanceH(si);
		double sz = so.distanceV(si);
		if (s < D && sz < H) {
			viol = true;
		} 
		return viol;
	}
			
	
	/**
	 * Sets internal ArrayList of times which represent times of loss of separation (assuming cylinder detection)
	 * between ownship and traffic plans, else returns -1 to indicate no loss of
	 * separation over the flight plans. Considers only the intersection of
	 * times from both Plans.
	 * 
	 * @param ownship  Plan
	 * @param intruder Plan
	 * @return ArrayList of non-negative numbers representing times at which
	 *         ownship and intruder lost separation, or -1 if no such time(s)
	 *         exist.
	 */
	public boolean iterDetection(Plan ownship, Plan traffic, double tStart, double tStop, double tStep, double tol) {		
		//f.pln(" $$$$ iterDetection: tStart = "+tStart+" tStop = "+tStop);
		tin.clear();
		tout.clear();
		segin.clear();
		segout.clear();
		tcpa.clear();
		dist_tca.clear();	
        Detection3D cd = getCoreDetection(); 
	    if ( ! (cd instanceof CDCylinder)) {
	    	System.out.println("iterDetection: incorrect detection core for this function");
	    	System.exit(1);
	    }
		double D = ((CDCylinder)cd).getHorizontalSeparation();	    	
	    double H = ((CDCylinder)cd).getVerticalSeparation();				
	    if (tStart < ownship.getFirstTime()) tStart = ownship.getFirstTime();
	    if (tStart < traffic.getFirstTime()) tStart = traffic.getFirstTime();
	    if (tStop > ownship.getLastTime()) tStop = ownship.getLastTime();
	    if (tStop > traffic.getLastTime()) tStop = traffic.getLastTime();  	    
		boolean foundTin = false;
		boolean foundTout = false;
		for (double t = tStart; t <= tStop; t = t + tStep) {
			Position so = ownship.position(t);
			Position si = traffic.position(t);
			boolean viol = LoS(so, si, D, H);
			if (viol && !foundTin) {
				foundTin = true;
				foundTout = false;
				if (t == tStart) {
					tin.add(t);
				} else {
					double tb = t;
					double ta = tb - 1.0;
					double tc = t - 0.5;
					while ((tb - ta) / 2.0 >= tol) {
						tc = (ta + tb) / 2.0;
						so = ownship.position(tc);
						si = traffic.position(tc);
						if (LoS(so,si,D,H)) tb = tc;
						else ta = tc;
					}
					tin.add(tc);
				}
			}
			if (!viol && foundTin && !foundTout) {
				foundTin = false;
				foundTout = true;
				if (t == tStop) {
					tout.add(t);
				} else {
					double tb = t;
					double ta = tb - 1.0;
					double tc = t - 0.5;
					while ((tb - ta) / 2.0 >= tol) {
						tc = (ta + tb) / 2.0;
						so = ownship.position(tc);
						si = traffic.position(tc);
						if (LoS(so,si,D,H)) ta = tc;
						else tb = tc;
					}
					tout.add(tc);
				}
			}
		}
		return size() > 0;
	}

	/**
	 * Returns if there is a conflict between two aircraft: the ownship and the
	 * traffic aircraft.
	 * <p>
	 * 
	 * This will not return any timing information prior to the owhship segment
	 * containing B.
	 *
	 * @param ownship
	 *            the trajectory intent of the ownship
	 * @param traffic
	 *            the trajectory intent of the traffic
	 * @param B
	 *            the absolute time to start looking for conflicts.
	 * @param T
	 *            the absolute time to end looking for conflicts.
	 * @return true if there is a conflict
	 */
	private boolean detectionXYZ(Plan ownship, Plan traffic, double B, double T) {
//f.pln("cdiicore.detectionXYZ B="+B+" T="+T);		
		boolean cont = false; // what is this?
		boolean linear = true;     // was Plan.PlanType.LINEAR);
		int start = B > ownship.getLastTime() ? ownship.size() - 1 : Math.max(0, ownship.getSegment(B));
		// calculated end segment. Only continue search if we are currently in a
		// conflict
		int end = T > ownship.getLastTime() ? ownship.size() - 1 : Math.max(0, ownship.size() - 1);
		for (int i = start; i < ownship.size(); i++) {
			// truncate the search at Tend if not in a conflict
			if (i <= end || cont) {
				Vect3 so = ownship.point(i).point();
				Velocity vo = ownship.initialVelocity(i,linear); 
				double t_base = ownship.getTime(i); // ownship start of leg
				// GEH : special case!!!
				if (t_base < B) {
					so = so.AddScal(B-t_base, vo);
					t_base = B;
				}
				double nextTime; // ownship end of range
				if (i == ownship.size() - 1) {
					nextTime = 0.0; // set to 0
				} else {
					nextTime = ownship.getTime(i + 1); // if in the plan, set to
														// the end of leg
				}
				double HT = nextTime - t_base;
				double BT = Math.max(0, B - t_base); // Math.max(0,B-(t_base - t0));
//f.pln("cdiicore.detectionXYZ B="+B+" t_base="+t_base+" BT="+BT);				
				double NT = cont ? HT : T - t_base; // - (t_base - t0);
				if (NT < 0.0) {
					continue;
				}
				// if (CDSICore.allowVariableDistance()) {
				// cdsi.setDistance(d+ownship.getSegmentDistanceBuffer(i));
				// cdsi.setHeight(h+ownship.getSegmentHeightBuffer(i));
				// }

				// System.out.println("$$CDIICore1 xyz: t_base = "+t_base+" HT = "+HT+" BT = "+BT+"  NT = "+NT);
				// System.out.println("$$CDIICore2 xyz: "+so+" "+vo+" "+t_base+" "+getTimeOut(tin.size()-1)+" "+cont);
				// Vect3 r = so.AddScal(225.4 - to, vo);
				// System.out.println("$$CDIICore3 xyz: "+r.cyl_norm(cdsi.getDistance(),
				// cdsi.getHeight()));
				// r = so.AddScal(213.1 - to, vo);
				// System.out.println("$$CDIICore4 xyz: "+r.cyl_norm(cdsi.getDistance(),
				// cdsi.getHeight()));
				// System.out.println("$$CDIICore5 xyz: "+cdsi.size());
				cdsi.detectionXYZ(so, vo, t_base, HT, traffic, BT, NT);
				captureOutput(cdsi, i);
				cont = size() > 0 ? tout.get(tout.size() - 1) == HT + t_base
						: false;
			}
		}
		// if (CDSICore.allowVariableDistance()) {
		// cdsi.setDistance(d);
		// cdsi.setHeight(h);
		// }
		merge();
		return conflict();
	}

	/**
	 * Returns if there is a conflict between two aircraft: the ownship and the
	 * traffic aircraft.
	 * <p>
	 * 
	 * This will not return any timing information prior to the ownship segment
	 * containing B.
	 *
	 * @param ownship
	 *            the trajectory intent of the ownship
	 * @param traffic
	 *            the trajectory intent of the traffic
	 * @param B
	 *            the absolute time to start looking for conflicts.
	 * @param T
	 *            the absolute time to end looking for conflicts.
	 * @return true if there is a conflict
	 */
	private boolean detectionLL(Plan ownship, Plan traffic, double B, double T) {
		boolean cont = false;
		boolean linear = true;     // was Plan.PlanType.LINEAR);

		// double d = getDistance();
		// double h = getHeight();

		int start = B > ownship.getLastTime() ? ownship.size() - 1 : Math.max(0, ownship.getSegment(B));
		// calculated end segment. Only continue search if we are currently in a
		// conflict
		int end = T > ownship.getLastTime() ? ownship.size() - 1 : Math.max(0, ownship.size() - 1);
		for (int i = start; i < ownship.size(); i++) {
			// truncate the search at Tend if not in a conflict
			if (i <= end || cont) {
				LatLonAlt llo = ownship.point(i).lla();
				double t_base = ownship.getTime(i);
				// GEH : special case!!!
				if (t_base < B) {
					llo = ownship.position(B,linear).lla();
					t_base = B;
				}

				double nextTime;
				if (i == ownship.size() - 1) {
					nextTime = 0.0;
				} else {
					nextTime = ownship.getTime(i + 1);
				}
				double HT = nextTime - t_base; // state-based time horizon
												// (relative time)
				double BT = Math.max(0, B - t_base); // begin time to look for
														// conflicts (relative
														// time)
				double NT = cont ? HT : T - t_base; // end time to look for
													// conflicts (relative time)
				if (NT < 0.0) {
					continue;
				}
				Velocity vo = ownship.velocity(t_base, linear); // CHANGED!!!
				//f.pln("$$CDIICore times: i="+i+" BT = "+BT+"  NT = "+NT+" HT = "+HT+" B = "+B);
				//f.pln("$$CDIICore LL: "+llo+" "+vo+" "+t_base);
				cdsi.detectionLL(llo, vo, t_base, HT, traffic, BT, NT);
				captureOutput(cdsi, i);
				cont = size() > 0 ? tout.get(tout.size() - 1) == HT + t_base : false;
			}
		}
		// if (CDSICore.allowVariableDistance()) {
		// cdsi.setDistance(d);
		// cdsi.setHeight(h);
		// }

		merge();

		return conflict();
	}

	void captureOutput(CDSICore cdsi, int seg) {
		// System.out.println("size "+cdsi.size());
		for (int index = 0; index < cdsi.size(); index++) {
			double vtin = cdsi.getTimeIn(index);
			double vtout = cdsi.getTimeOut(index);
			double vtcpa = cdsi.getTimeClosest(index);
			double vd = cdsi.getDistanceClosest(index);
			// double vdv = cdsi.getClosestVert(index);
			segin.add(seg);
			segout.add(seg);
			tin.add(vtin);
			tout.add(vtout);
			tcpa.add(vtcpa);
			dist_tca.add(vd);
			// dv.add(vdv);

			// System.out.println("$$CDIICore times "+vtin+" "+vtout+" "+index);

		}
	}

	private void merge() {
		int i = 0;
		while (i < segin.size() - 1) {
			if (!Util.almost_less(tout.get(i), tin.get(i + 1), Util.PRECISION7)) {
				tin.remove(i + 1);
				segin.remove(i + 1);
				tout.remove(i);
				segout.remove(i);
				if (dist_tca.get(i) < dist_tca.get(i + 1)) {
					tcpa.remove(i + 1);
					dist_tca.remove(i + 1);
				} else {
					tcpa.remove(i);
					dist_tca.remove(i);
				}
			} else {
				i++;
			}
		}
	}


	/**
	 * EXPERIMENTAL
	 * Perform a "quick" conflict detection test only.  This will not record or modify existing timing information, and should be at least as fast as the static cdiicore() call.
	 * @param ownship
	 * @param traffic
	 * @param B
	 * @param T
	 * @return
	 */
	public boolean conflictDetection(Plan ownship, Plan traffic, double B, double T) {
		if (ownship.isLatLon() != traffic.isLatLon()) {
			return false;
		}
		if (ownship.isLatLon()) {
			return conflictLL(ownship, traffic, B, T);
		} else {
			return conflictXYZ(ownship, traffic, B, T);
		}
	}

	private boolean conflictXYZ(Plan ownship, Plan traffic, double B, double T) {
		boolean linear = true;     // was Plan.PlanType.LINEAR);
		int start = B > ownship.getLastTime() ? ownship.size() - 1 : Math.max(
				0, ownship.getSegment(B));
		int end = T > ownship.getLastTime() ? ownship.size() - 1 : Math.max(0,
				ownship.size() - 1);
		for (int i = start; i < ownship.size(); i++) {
			if (i <= end) {
				Vect3 so = ownship.point(i).point();
				Velocity vo = ownship.initialVelocity(i,linear); 
				double t_base = ownship.getTime(i); // ownship start of leg
				double nextTime; // ownship end of range
				if (i == ownship.size() - 1) {
					nextTime = 0.0; // set to 0
				} else {
					nextTime = ownship.getTime(i + 1); // if in the plan, set to
														// the end of leg
				}
				double HT = nextTime - t_base;
				double BT = Math.max(0, B - t_base); // Math.max(0,B-(t_base -
				double NT = T - t_base; // - (t_base - t0);
				if (NT < 0.0) {
					continue;
				}
				if (cdsi.conflictXYZ(so, vo, t_base, HT, traffic, BT, NT)) return true;
			}
		}
		return false;
	}

	private boolean conflictLL(Plan ownship, Plan traffic, double B, double T) {
		boolean linear = true;     // was Plan.PlanType.LINEAR);
		int start = B > ownship.getLastTime() ? ownship.size() - 1 : Math.max(
				0, ownship.getSegment(B));
		int end = T > ownship.getLastTime() ? ownship.size() - 1 : Math.max(0,
				ownship.size() - 1);
		for (int i = start; i < ownship.size(); i++) {
			if (i <= end) {
				LatLonAlt llo = ownship.point(i).lla();
				double t_base = ownship.getTime(i);
				double nextTime;
				if (i == ownship.size() - 1) {
					nextTime = 0.0;
				} else {
					nextTime = ownship.getTime(i + 1);
				}
				double HT = nextTime - t_base; // state-based time horizon
				double BT = Math.max(0, B - t_base); // begin time to look for
				double NT = T - t_base; // end time to look for
				if (NT < 0.0) {
					continue;
				}
				Velocity vo = ownship.velocity(t_base, linear); // CHANGED!!!
				if (cdsi.conflictLL(llo, vo, t_base, HT, traffic, BT, NT)) return true;
			}
		}
		return false;
	}
	
	/**
	 * Experimental: calculate tca and dist score
	 * @return
	 */
	public Pair<Double,Double> urgency(Plan ownship, Plan traffic, double B, double T) {
		boolean cont = false;
		boolean linear = true;     // was Plan.PlanType.LINEAR);

		int start = B > ownship.getLastTime() ? ownship.size() - 1 : Math.max(0, ownship.getSegment(B));
		int end = T > ownship.getLastTime() ? ownship.size() - 1 : Math.max(0, ownship.size() - 1);
		
		Pair<Double,Double> best = Pair.make(-1.0, Double.MAX_VALUE);
		
		for (int i = start; i < ownship.size(); i++) {
			if (i <= end || cont) {
				Position llo = ownship.point(i).position();
				double t_base = ownship.getTime(i);
				if (t_base < B) {
					llo = ownship.position(B,linear);
					t_base = B;
				}

				double nextTime;
				if (i == ownship.size() - 1) {
					nextTime = 0.0;
				} else {
					nextTime = ownship.getTime(i + 1);
				}
				double HT = nextTime - t_base;
				double BT = Math.max(0, B - t_base);
				double NT = cont ? HT : T - t_base;
				if (NT < 0.0) {
					continue;
				}
				Velocity vo = ownship.velocity(t_base, linear); // CHANGED!!!
				Pair<Double,Double> ret = cdsi.urgency(llo, vo, t_base, HT, traffic, BT, NT);
				
				if (ret.first >= 0 && ret.second < best.second) best = ret;
				
				cont = size() > 0 ? tout.get(tout.size() - 1) == HT + t_base : false;
			}
		}
		return best;
	}

	
	private static final DecimalFormat Frm = new DecimalFormat("0.0000");

	private String Fm4(double v) {
		return Frm.format(v);
	}

	public String toString() {
		String str = "CDII: size() = " + size();
		if (size() > 0) {
			str = str
					+ "\n TimeIn(k)  Timeout(k) SegmentIn(k)  SegmentOut(k) \n";
		} else {
			str = str + " (no conflicts)\n";
		}
		for (int k = 0; k < size(); k++) {
			str = str + "      " + Fm4(getTimeIn(k)) + "    "
					+ Fm4(getTimeOut(k)) + "       " + getSegmentIn(k)
					+ "              " + getSegmentOut(k) + " \n";
		}
		str = str + "CDSI = " + cdsi.toString();
		return str;
	}

	// ErrorReporter Interface Methods

	public boolean hasError() {
		return cdsi.hasError();
	}

	public boolean hasMessage() {
		return cdsi.hasMessage();
	}

	public String getMessage() {
		return cdsi.getMessage();
	}

	public String getMessageNoClear() {
		return cdsi.getMessageNoClear();
	}

	public void setCoreDetection(Detection3D d) {
		cdsi.setCoreDetection(d);
		def.cdsi.setCoreDetection(d);
		tin.clear();
		tout.clear();
		tcpa.clear();
		dist_tca.clear();
		segin.clear();
		segout.clear();
	}

	public Detection3D getCoreDetection() {
		return cdsi.getCoreDetection();
	}

}
