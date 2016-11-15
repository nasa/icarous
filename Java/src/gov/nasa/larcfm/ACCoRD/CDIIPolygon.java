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

import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.f;

import java.text.*;           // for DecimalFormat

/**
 * This class implements the core algorithms for conflict detection
 * between an ownship and a traffic aircraft trajectory, where both
 * trajectories include intent information.  This class assumes all
 * inputs are in internal units.  As such, this class is most
 * appropriate to be part of a larger system, not as a stand-alone
 * conflict detection algorithm.  For that functionality, see the
 * class CDII.<p>
 *
 * This class can be used two ways: through an object or through a
 * static method.  The static method approach has the advantage that
 * an object does not need to be created.  The object approach has the
 * advantage that more information is available.  Specifically, the
 * start and end times for every conflict are available in the object
 * approach. <p>
 * 
 * When using the object method, all method getTimeIn(), etc, assume
 * that the detection() method has been called first.<p>
 */
public final class CDIIPolygon implements ErrorReporter, DetectionPolygonAcceptor {  

  private static CDIIPolygon def = new CDIIPolygon(new CDPolyIter());

  private ArrayList<Double> tin;
  private ArrayList<Double> tout;
  private ArrayList<Double> tcpa;
  private ArrayList<Double> dist_tca;
  private CDSIPolygon cdsi;

  /** 
   * Create a new conflict detection (intent information for both the ownship
   * and traffic) object.
   *
   * @param distance the minimum horizontal separation distance
   * @param height the minimum vertical separation height
   */
  public CDIIPolygon(DetectionPolygon cd) {
    tin = new ArrayList<Double>(10);
    tout = new ArrayList<Double>(10);
    tcpa = new ArrayList<Double>(10);
    dist_tca = new ArrayList<Double>(10);
    cdsi = new CDSIPolygon(cd);
  }

  public CDIIPolygon() {
    this(new CDPolyIter());
  }


//  /**
//   * Returns the conflict detection filter time.
//   * 
//   * @return the conflict detection filter time 
//   */
//  public double getFilterTime() {
//    return cdsi.getFilterTime();
//  }


//  /**
//   * Sets the conflict detection filter time.
//   * 
//   * @param cdfilter the conflict detection filter time.
//   */
//  public void setFilterTime(double cdfilter) {
//    cdsi.setFilterTime(cdfilter);
//  }

  /** Returns the number of conflicts */
  public int size() {
    return tin.size();
  }

  /** Returns if there were any conflicts.  */
  public boolean conflict() {
    return tin.size() > 0;
  }

  /** 
   * Returns the start time of the conflict.  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getTimeIn(int i) {
    if (i < 0 || i >= tin.size()) return 0.0;
    return tin.get(i);
  }

  /** 
   * Returns the end time of the conflict.  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getTimeOut(int i) {
    if (i < 0 || i >= tin.size()) return 0.0;
    return tout.get(i);
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
    return tcpa.get(i);
  }

  /** 
   * Returns the distance index at the time of closest approach. 
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getDistanceClosest(int i) {
    if (i < 0 || i >= tin.size()) return 0.0;
    return dist_tca.get(i);
  }


  /**
   * Is there a conflict at any time in the interval from start to
   * end (inclusive). This method assumes that the detection()
   * method has been called first.
   *
   * @param start the time to begin looking for conflicts
   * @param end the time to end looking for conflicts
   * @return true if there is a conflict
   */
  public boolean conflictBetween(double start, double end) {
    boolean rtn = false;
    for (int k = 0; k < size(); k++) {
      double tmIn  = getTimeIn(k);
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
   * Returns if there is a conflict between two aircraft: the ownship and
   * the traffic aircraft.  <p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param D the minimum horizontal separation distance
   * @param H the minimum vertical separation distance
   * @param B the absolute time to start looking for conflicts
   * @param T the absolute time to end looking for conflicts
   * @return true if there is a conflict
   */
  public static boolean cdiicore(Plan ownship, PolyPath traffic, double B, double T) {
    return def.detection(ownship, traffic, B, T);
  }

  /**
   * Return true if there is a violation between two aircraft at time tm.
   * If tm is outside either of the plans' times, this will return false.
   * @param ownship
   * @param traffic
   * @param tm
   * @return
   */
  public boolean violation(Plan ownship, PolyPath traffic, double tm) {
    if (tm < ownship.getFirstTime() || tm > ownship.getLastTime()) {
      return false;
    }
    return cdsi.violation(ownship.position(tm), ownship.velocity(tm), traffic, tm);
  }

  
  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param B the absolute time to start looking for conflicts (no time information will be returned prior to the start of the ownship segment containing B)
   * @param T the absolute time to end looking for conflicts
   * @return true if there is a conflict
   */
  public boolean detection(Plan ownship, PolyPath traffic, double B, double T) {   
//f.pln("CDIIPolygon.detection B="+B+" T="+T);	  
    tin.clear();
    tout.clear();
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
   * This version calculates the "true" time in and time out for all conflicts that overlap with [B,T].
   * It is less efficient than the normal detection() algorithm and should only be called if accurate time in information is necessary 
   * when B might be within a loss of separation region.
   */
  public boolean detectionExtended(Plan ownship, PolyPath traffic, double B, double T) {    
    if (!detection(ownship, traffic, Math.max(ownship.getFirstTime(), traffic.getFirstTime()), Math.min(ownship.getLastTime(), traffic.getLastTime()))) {
      return false;
    }
    int i = 0;
    while (i < size()) {
      if (getTimeIn(i) > T || getTimeOut(i) < B) {
        tin.remove(i);
        tout.remove(i);
        tcpa.remove(i);
        dist_tca.remove(i);
      } else {
        i++;
      }
    }
    return size() > 0;
  }

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   * 
   * This will not return any timing information prior to the owhship segment containing B.
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param B the absolute time to start looking for conflicts.
   * @param T the absolute time to end looking for conflicts.
   * @return true if there is a conflict
   */
  private boolean detectionXYZ(Plan ownship, PolyPath traffic, double B, double T) {
    boolean cont = false;                     // what is this?
    int start = B > ownship.getLastTime() ? ownship.size()-1 : Math.max(0,ownship.getSegment(B));
    // calculated end segment.  Only continue search if we are currently in a conflict
    int end = T > ownship.getLastTime() ? ownship.size()-1 : Math.max(0,ownship.size()-1);
    for (int i = start; i < ownship.size(); i++){
      // truncate the search at Tend if not in a conflict
      if (i <= end || cont) {
        Vect3 so = ownship.point(i).point();
		boolean linear = true;     // was Plan.PlanType.LINEAR);
        Velocity vo = ownship.initialVelocity(i, linear);
        double t_base = ownship.getTime(i);   // ownship start of leg
        double nextTime;                      // ownship end of range
        if (i == ownship.size() - 1) {
          nextTime = 0.0;                   // set to 0
        } else {
          nextTime = ownship.getTime(i + 1);  // if in the plan, set to the end of leg
        }
        double HT = nextTime - t_base;        
        double BT = Math.max(0,B - t_base); // Math.max(0,B-(t_base - t0));
        double NT = cont ? HT : T - t_base; // - (t_base - t0); 

        if (NT < 0.0) {
          continue;
        }
        //f.pln(" $$>> CDIIPolygon: detectionXYZ: call cdsi.detectionXYZ! for i = "+i);
        cdsi.detectionXYZ(so, vo, t_base, HT, traffic, BT, NT);
        captureOutput(cdsi, i);
        cont = size() > 0 ? tout.get(tout.size()-1) == HT + t_base : false;     

      }
    }
    merge();
    return conflict();
  }

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   * 
   * This will not return any timing information prior to the ownship segment containing B.
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param B the absolute time to start looking for conflicts.
   * @param T the absolute time to end looking for conflicts.
   * @return true if there is a conflict
   */
  private boolean detectionLL(Plan ownship, PolyPath traffic, double B, double T) {
    boolean cont = false;
//f.pln(" $$ CDIIPolygon.detectionLL ownship="+ownship.toString()+" traffic="+traffic.toString()+" B = "+B+" T= "+T);
    int start = B > ownship.getLastTime() ? ownship.size()-1 : Math.max(0,ownship.getSegment(B));
    // calculated end segment.  Only continue search if we are currently in a conflict
    int end = T > ownship.getLastTime() ? ownship.size()-1 : Math.max(0,ownship.size()-1);
    for (int i = start; i < ownship.size(); i++){ 
      // truncate the search at Tend if not in a conflict
      if (i <= end || cont) {
        LatLonAlt llo = ownship.point(i).lla();
        double t_base = ownship.getTime(i);
        double nextTime;
        if (i == ownship.size() - 1) {
    		nextTime = 0.0;
        } else {
          nextTime = ownship.getTime(i + 1);  
        }
        double HT = nextTime - t_base;      // state-based time horizon (relative time)
        double BT = Math.max(0,B - t_base); // begin time to look for conflicts (relative time)
        double NT = cont ? HT : T - t_base; // end time to look for conflicts (relative time)
        if (NT < 0.0) {
          continue;
        }
		boolean linear = true;     // was Plan.PlanType.LINEAR);
        Velocity vo = ownship.velocity(t_base, linear);           // CHANGED!!!
//System.out.println("$$CDIIPolygon times: i="+i+" BT = "+BT+"  NT = "+NT+" HT = "+HT+" B = "+B);
        //        System.out.println("$$CDIICore LL: "+llo+" "+vo+" "+t_base);       
        cdsi.detectionLL(llo, vo, t_base, HT, traffic, BT, NT);
//        f.pln(" $$## CDIIPolygon.detectionLL: cdsi = "+cdsi);
        captureOutput(cdsi, i);
        cont = size() > 0 ? tout.get(tout.size()-1) == HT + t_base : false;     
      }
    }

    merge();

    return conflict();
  }

  void captureOutput(CDSIPolygon cdsi, int seg) {
    for (int index = 0; index < cdsi.size(); index++) {
      double vtin = cdsi.getTimeIn(index);
      double vtout = cdsi.getTimeOut(index);
      double vtcpa = cdsi.getTimeClosest(index);
      double vd = cdsi.getDistanceClosest(index);
      tin.add(vtin);
      tout.add(vtout);
      tcpa.add(vtcpa);
      dist_tca.add(vd);
    }
  }

  private void merge() {
    int i = 0;
    while ( i < tin.size() - 1){
      if ( ! Util.almost_less(tout.get(i), tin.get(i+1), Util.PRECISION7)) {
        tin.remove(i+1);
        tout.remove(i);
        if (dist_tca.get(i) < dist_tca.get(i+1)) {
          tcpa.remove(i+1);
          dist_tca.remove(i+1);
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
   * Perform a "quick" conflict-only check without storing statistics
   * @param ownship
   * @param traffic
   * @param B
   * @param T
   * @return
   */
  public boolean conflictDetectionOnly(Plan ownship, PolyPath traffic, double B, double T) {    
	    if (ownship.isLatLon() != traffic.isLatLon()) {
	      return false;
	    }
	    if (ownship.isLatLon()) {
	      return conflictOnlyLL(ownship, traffic, B, T);
	    } else {
	      return conflictOnlyXYZ(ownship, traffic, B, T);
	    }
	  }


  /**
   * EXPERIMENTAL
   * @param ownship
   * @param traffic
   * @param B
   * @param T
   * @return
   */
  private boolean conflictOnlyXYZ(Plan ownship, PolyPath traffic, double B, double T) {
	    int start = B > ownship.getLastTime() ? ownship.size()-1 : Math.max(0,ownship.getSegment(B));
	    int end = T > ownship.getLastTime() ? ownship.size()-1 : Math.max(0,ownship.size()-1);
	    for (int i = start; i < ownship.size(); i++){
	      if (i <= end) {
	        Vect3 so = ownship.point(i).point();
			boolean linear = true;     // was Plan.PlanType.LINEAR);
	        Velocity vo = ownship.initialVelocity(i, linear);
	        double t_base = ownship.getTime(i);   // ownship start of leg
	        double nextTime;                      // ownship end of range
	        if (i == ownship.size() - 1) {
	          nextTime = 0.0;                   // set to 0
	        } else {
	          nextTime = ownship.getTime(i + 1);  // if in the plan, set to the end of leg
	        }
	        double HT = nextTime - t_base;        
	        double BT = Math.max(0,B - t_base); // Math.max(0,B-(t_base - t0));
	        double NT = T - t_base; // - (t_base - t0); 

	        if (NT < 0.0) {
	          continue;
	        }
	        if (cdsi.conflictOnlyXYZ(so, vo, t_base, HT, traffic, BT, NT)) return true;
	      }
	    }
	    return false;
	  }

  /**
   * EXPERIMENTAL
   * @param ownship
   * @param traffic
   * @param B
   * @param T
   * @return
   */
	  private boolean conflictOnlyLL(Plan ownship, PolyPath traffic, double B, double T) {
	    int start = B > ownship.getLastTime() ? ownship.size()-1 : Math.max(0,ownship.getSegment(B));
	    int end = T > ownship.getLastTime() ? ownship.size()-1 : Math.max(0,ownship.size()-1);
	    for (int i = start; i < ownship.size(); i++){ 
	      if (i <= end) {
	        LatLonAlt llo = ownship.point(i).lla();
	        double t_base = ownship.getTime(i);
	        double nextTime;
	        if (i == ownship.size() - 1) {
	    		nextTime = 0.0;
	        } else {
	          nextTime = ownship.getTime(i + 1);  
	        }
	        double HT = nextTime - t_base;      // state-based time horizon (relative time)
	        double BT = Math.max(0,B - t_base); // begin time to look for conflicts (relative time)
	        double NT = T - t_base; // end time to look for conflicts (relative time)
	        if (NT < 0.0) {
	          continue;
	        }
			boolean linear = true;     // was Plan.PlanType.LINEAR);
	        Velocity vo = ownship.velocity(t_base, linear);           // CHANGED!!!
	        if (cdsi.conflictOnlyLL(llo, vo, t_base, HT, traffic, BT, NT)) return true;
	      }
	    }
	    return false;
	  }
  
  private static final DecimalFormat Frm = new DecimalFormat("0.0000");

  private String Fm4(double v) {
    return Frm.format(v);
  }

  public String toString() {
    String str = "CDIIPolygon: size() = "+size();
    if (size() > 0) {
      str = str + "\n TimeIn(k)  Timeout(k) \n";
    } else {
      str = str + " (no conflicts)\n";
    }
    for (int k = 0; k < size(); k++) {
      str = str + "      "+Fm4(getTimeIn(k))+"    "+Fm4(getTimeOut(k))+"\n";
    }
    str = str + "CDSIPolygon = "+cdsi.toString();
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

  public void setCorePolygonDetection(DetectionPolygon d) {
    cdsi.setCorePolygonDetection(d);
    tin.clear();
    tout.clear();
    tcpa.clear();
    dist_tca.clear();
  }

  public DetectionPolygon getCorePolygonDetection() {
    return cdsi.getCorePolygonDetection();
  }

}

