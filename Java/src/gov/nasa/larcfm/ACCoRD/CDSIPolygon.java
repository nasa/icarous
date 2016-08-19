/*
 * CDSI Core - The core algorithms for conflict detection between an
 * ownship state vector and a traffic aircraft with a flight plan.
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

import gov.nasa.larcfm.ACCoRD.CDSSCore;
import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.MovingPolygon3D;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.Projection;
//import gov.nasa.larcfm.Util.SimpleProjection;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.SimplePoly;
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
public final class CDSIPolygon implements ErrorReporter, DetectionPolygonAcceptor {  

  private static CDSIPolygon def = new CDSIPolygon(new CDPolyIter());

  private ArrayList<Double> tin;
  private ArrayList<Double> tout;
  private ArrayList<Double> tca;
  private ArrayList<Double> dist_tca;
  private DetectionPolygon cdss;
  private ErrorLog error;

  private static final boolean allowVariableDistanceBuffer = true; // set to true to allow a per-leg variation on D,H, based on values in the plan(s)

  /** Can the distance parameter change from leg to leg? */
  public static boolean allowVariableDistance() { return allowVariableDistanceBuffer; }

  /** 
   * Create a new conflict detection (state information for the ownship
   * and intent information for the traffic) object with the given
   * parameters for the minimum horizontal distance and the minimum
   * vertical information.
   */
  public CDSIPolygon(DetectionPolygon d) {
    cdss = d;
    tin = new ArrayList<Double>(10);
    tout = new ArrayList<Double>(10);
    tca = new ArrayList<Double>(10);
    dist_tca = new ArrayList<Double>(10);
    error = new ErrorLog("CDSICore");
  }

  public CDSIPolygon() {
    this(new CDPolyIter());
  }

  //    /**
  //     * Returns the conflict detection filter time.
  //     * 
  //     * @return the conflict detection filter time seconds
  //     */
  //  public double getFilterTime() {
  //    return cdss.getFilterTime();
  //  }
  //  
  //
  //
  //    /**
  //     * Sets the conflict detection filter time.
  //     * 
  //     * @param cdfilter the conflict detection filter time in seconds.
  //     */
  //  public void setFilterTime(double cdfilter) {
  //    cdss.setFilterTime(cdfilter);
  //  }

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
  public static boolean cdsicore(Position so, Velocity vo, double t0, double state_horizon, PolyPath intent, double B, double T) {
    return def.detection(so, vo, t0, state_horizon, intent, B, T);
  }

  /**
   * Return true if the given position and velocity are in violation with the intent aircraft at time tm.
   * If tm is outisde the intent information, return false; 
   * @param so
   * @param vo
   * @param intent
   * @param tm
   * @return
   */
  public boolean violation(Position so, Velocity vo, PolyPath intent, double tm) {
    if (tm < intent.getFirstTime() || tm > intent.getLastTime()) {
      return false;
    }

    if (so.isLatLon()) {
      EuclideanProjection proj = Projection.createProjection(so);
      Vect3 so3 = proj.project(so);
      Velocity vo3 = proj.projectVelocity(so, vo);
      return cdss.violation(so3, vo3, intent.position(tm).poly3D(proj));
    } else {
      return cdss.violation(so.point(), vo, intent.position(tm).poly3D(null));
    }
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
  public boolean detection(Position so, Velocity vo, double t0, double state_horizon, PolyPath intent, double B, double T) {
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
  public boolean detectionXYZ(Vect3 so, Velocity vo, double t0, double state_horizon, PolyPath intent, double B, double T) {
    tin.clear();
    tout.clear();
    tca.clear();
    dist_tca.clear();

    double t_base;
    int start_seg;
    boolean cont = false; 
    double BT = 0.0;
    double NT = 0.0;
    double HT = 0.0;    
    if (t0 < intent.getTime(0)) {
      t_base = intent.getTime(0);
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
      if (j == intent.size() - 1 && !intent.isContinuing()) { 
        continue;
      } else { // normal case
        double tend = Double.MAX_VALUE;
        if (j < intent.size()-1) {
          tend = intent.getTime(j+1) - t_base;
        }
        HT = Math.max(0.0, Math.min(state_horizon - (t_base - t0), tend));
        BT = Math.max(0.0, B + t0 - t_base);
        NT = cont ? HT : T + t0 - t_base;
        if (NT >= 0) {
          MovingPolygon3D mpg = intent.getMovingPolygon(t_base, null);
          //          f.pln(" $$>> CDSIPolygon.detectionXYZ: mpg = "+mpg+" BT = "+BT+" NT = "+NT+" HT = "+HT);
          boolean cdssRtn = cdss.conflictDetection(sop, vo, mpg, BT, Math.min(NT, HT));
          //f.pln(" $$>> CDSIPolygon.detectionXYZ: j = "+j+" cdssRtn = "+cdssRtn);
          if ( cdssRtn ) {
            //for (int i = 0; i < intent.getPoly(0).size(); i++) f.pln("ui_reference_point_"+Util.nextCount()+" = "+intent.getPoly(0).getPoint(i).toString8()); 
            //for (int i = 0; i < mpg.horizpoly.size(); i++) f.pln("ui_reference_point_"+Util.nextCount()+" = "+new Position(new Vect3(mpg.horizpoly.polystart.get(i),0)).toString8()); 

            for(int i = 0; i < cdss.getTimesOut().size(); i++) {
              if (Math.abs((t0+B) - (cdss.getTimesOut().get(i)+t_base)) > 0.0000001) { 
                //f.pln("$$$$>> CDSIPolygon.detectionXYZ: in0="+(cdss.getTimesIn().get(0)+t0)+" out0="+(cdss.getTimesOut().get(0)+t0));
                captureOutput(t_base, i, cdss);
              }
            }
          }     
        }
        cont = size() > 0 ? tout.get(tout.size()-1) == HT + t_base : false;     
        t_base = intent.getTime(j+1);        
      }
    }
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
  public boolean detectionLL(LatLonAlt so, Velocity vo, double t0, double state_horizon, PolyPath intent, double B, double T) {
    //f.pln("======== $$$$$$ CDSIPolygon.detectionLL so="+so+" vo="+vo+" to="+t0+" state_horizon="+state_horizon+" intent="+intent.toString()+" B (relto t0) = "+B+" T (rel to t0) = "+T);
    tin.clear();
    tout.clear();
    tca.clear();
    dist_tca.clear();    
    double t_base;
    int start_seg;
    boolean cont = false;
    double BT = 0.0;
    double NT = 0.0;
    double HT = 0.0;    
    if (t0 < intent.getTime(0)) {
      t_base = intent.getTime(0);
      start_seg = 0;
    } else {
      t_base = t0;
      start_seg = intent.getSegment(t0);
      //f.pln(" $$ CDSIPolygon.detectionLL start_seg="+start_seg+" intent start="+intent.getFirstTime()+" intent end="+intent.getLastTime());        
      if (start_seg < 0) {
        //f.pln("CDSIPolygon: start_seg <0!");     
        return false;  // t0 past end of Flight Plan 
      }
    }  
    //f.pln(" $$ CDSIPolygon.detectionLL start_seg="+start_seg+" t_base = "+t_base+" intent ="+intent);        
    for (int j = start_seg; j < intent.size(); j++){ 
      LatLonAlt so2p = GreatCircle.linear_initial(so, vo, t_base-t0);  //CHANGED!!!
      //SimplePoly sip = intent.position(t_base);
      EuclideanProjection proj = Projection.createProjection(so.zeroAlt()); // CHECK THIS!!!  Should it be so2p?
      Vect3 so3 = proj.project(so2p);
      //      f.pln(" $$>> ^^^^^^^^^^^^^^^^ CDSIPolygon.detectionLL: so3 = "+so3);
      //f.pln(" $$>> CDSIPolygon.detectionLL so2p = "+so2p+" proj = "+proj+" sip = "+sip);
      if (j == intent.size() - 1 && !intent.isContinuing()) { 
        //f.pln("CDSIPolygon: j == intent.size() - 1!");      
        continue; // leave loop
      } else { // normal case
        //Poly3D si3 = sip.poly3D(proj);      
        double tend = Double.MAX_VALUE;
        if (j < intent.size()-1) {
          tend = intent.getTime(j+1) - t_base;
        }
        HT = Math.max(0.0, Math.min(state_horizon - (t_base - t0), tend));
        BT = Math.max(0.0, B + t0 - t_base);
        NT = cont ? HT : T + t0 - t_base;      
        Velocity vop = proj.projectVelocity(so2p, vo);  //CHANGED!!!
        //Velocity vip = proj.projectVelocity(sip.centroid(), intent.velocity(t_base));
        //f.pln(" $$ CDSIPolygon.detectionLL BT="+BT+" HT="+HT+" NT="+NT+" cont="+cont);        
        if (NT >= 0) {          
          MovingPolygon3D mpg = intent.getMovingPolygon(t_base, proj);
          //f.pln(" $$>> CDSIPolygon.detectionLL: mpg = "+mpg+" BT = "+BT+" NT = "+NT+" HT = "+HT);
          boolean cdssRtn = cdss.conflictDetection(so3, vop, mpg, BT, Math.min(NT, HT));          
          //          f.pln(" $$>> CDSIPolygon.detectionLL: j = "+j+" cdssRtn = "+cdssRtn);
          if (cdssRtn ) {  // CHANGED!!!
            // filter out when the start time for searching is the the end time of the conflict
            for (int i = 0; i < cdss.getTimesIn().size(); i++) {
              if (Math.abs((t0+B) - (cdss.getTimesOut().get(i)+t_base)) > 0.0000001) { 
                captureOutput(t_base, i, cdss);      
                //f.pln("CDSIPoly: i="+i+" cdss so3="+so3+" vop="+vop+" mpg= ---"+" B="+BT+" T="+(Math.min(NT,HT)));
                //f.pln("CDSIPoly: i="+i+" tin="+(cdss.getTimesIn().get(i))+" tout="+(cdss.getTimesOut().get(i))+" end="+(T)+" horiz="+(state_horizon));                
              }
            }
          } 
        }
        cont = size() > 0 ? tout.get(tout.size()-1) == HT + t_base : false;     
        t_base = intent.getTime(j+1); // ignored if last value
      }
    }
    merge();
    boolean rtn = conflict();
    //    f.pln(" $$>> CDSIPolygon.detectionLL: rtn = "+rtn);
    return rtn;
  }


  private void captureOutput(double t_base, int conf, DetectionPolygon cd) {
    tin.add(cd.getTimesIn().get(conf)+t_base);
    tout.add(cd.getTimesOut().get(conf)+t_base);
    tca.add(cd.getCriticalTimesOfConflict().get(conf) + t_base);
    dist_tca.add(cd.getDistancesAtCriticalTimes().get(conf));
  }


  private void merge() {
    int i = 0;
    while ( i < tin.size() - 1){
      if ( ! Util.almost_less(tout.get(i), tin.get(i+1), Util.PRECISION7)) {
        tin.remove(i+1);
        tout.remove(i);
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
   * @return
   */
  public boolean conflictOnlyXYZ(Vect3 so, Velocity vo, double t0, double state_horizon, PolyPath intent, double B, double T) {
	    double t_base;
	    int start_seg;
	    double BT = 0.0;
	    double NT = 0.0;
	    double HT = 0.0;    
	    if (t0 < intent.getTime(0)) {
	      t_base = intent.getTime(0);
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
	      if (j == intent.size() - 1 && !intent.isContinuing()) { 
	        continue;
	      } else { // normal case
	        double tend = Double.MAX_VALUE;
	        if (j < intent.size()-1) {
	          tend = intent.getTime(j+1) - t_base;
	        }
	        HT = Math.max(0.0, Math.min(state_horizon - (t_base - t0), tend));
	        BT = Math.max(0.0, B + t0 - t_base);
	        NT = T + t0 - t_base;
	        if (NT >= 0) {
	          MovingPolygon3D mpg = intent.getMovingPolygon(t_base, null);
//f.pln("CDSIPolygon.conflictXYZ test");
	          boolean ret = cdss.conflict(sop, vo, mpg, BT, Math.min(NT, HT)); 
//      f.pln("cdsspolygon.conflict("+sop+",\n"+vo+",\n"+mpg+",\n"+BT+",\n"+Math.min(NT, HT)+") = "+ret);	          
//boolean ret2 = CDPolyIter.conflict2(sop, vo, mpg, BT, Math.min(NT, HT));
//if (ret != ret2) {
//	int k = 0;
//	MovingPolygon3D mp3 = mpg.copy();
//	while (k < mp3.size() && mp3.size() > 3) {
//		MovingPolygon3D tmp = mp3.copy();
//		tmp.horizpoly.polystart.remove(k);
//		tmp.horizpoly.polyvel.remove(k);
//		boolean ret3 = cdss.conflict(sop, vo, tmp, BT, Math.min(NT, HT));
//		boolean ret4 = CDPolyIter.conflict2(sop, vo, tmp, BT, Math.min(NT, HT));
//		if (ret3 != ret4) {
//			mp3 = tmp;
//		} else {
//			k++;
//		}
//	}
////	for (double t = BT; t < Math.min(NT, HT); t++) {
////        boolean ret3 = cdss.conflict(sop, vo, mpg, t, t+5.0); 
////        boolean ret4 = CDPolyIter.conflict2(sop, vo, mpg, t, t+5.0);
////		if (ret3 != ret4) f.pln("XYZ time "+(t+t0));
////	}
//	double tt = Math.min(NT, HT);
//	mp3.horizpoly.tend = tt;
//f.pln("CDSIPolygon.conflictXYZ test FAILED old "+ret+"!= new"+ret2+" t0="+t0);
//	f.pln("so="+sop.toStringNP("m", "m", "m", 8));
//	f.pln("vo="+vo.toXYZ("m/s", "m/s", "m/s", 8));
//	f.pln("mp="+mp3);
//	f.pln("B="+BT);
//	f.pln("T="+tt);
//	f.pln("ownship,"+sop.toStringNP("NM", "NM", "ft", 8)+",-,0");
//	f.pln("ownship,"+sop.AddScal((tt-B), vo).toStringNP("NM", "NM", "ft", 8)+",-,"+(tt-B));
//	f.pln(mp3.toOutput("poly"));
//}
	          if (ret) return true;
	        }
	        t_base = intent.getTime(j+1);        
	      }
	    }
	    return false;
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
 * @return
 */
	  public boolean conflictOnlyLL(LatLonAlt so, Velocity vo, double t0, double state_horizon, PolyPath intent, double B, double T) {
	    double t_base;
	    int start_seg;
//f.pln("cdsipolygon.conflictLL T="+T);	    
	    double BT = 0.0;
	    double NT = 0.0;
	    double HT = 0.0;    
	    if (t0 < intent.getTime(0)) {
	      t_base = intent.getTime(0);
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
	      EuclideanProjection proj = Projection.createProjection(so.zeroAlt()); // CHECK THIS!!!  Should it be so2p?
	      Vect3 so3 = proj.project(so2p);
	      if (j == intent.size() - 1 && !intent.isContinuing()) { 
	        continue; // leave loop
	      } else { // normal case
	        double tend = Double.MAX_VALUE;
	        if (j < intent.size()-1) {
	          tend = intent.getTime(j+1) - t_base;
	        } 
	        HT = Math.max(0.0, Math.min(state_horizon - (t_base - t0), tend));
	        BT = Math.max(0.0, B + t0 - t_base);
	        NT = T + t0 - t_base;      
	        Velocity vop = proj.projectVelocity(so2p, vo);  //CHANGED!!!
	        if (NT >= 0) {          
	          MovingPolygon3D mpg = intent.getMovingPolygon(t_base, proj);
//f.pln("CDSIPolygon.conflictLL test");
	          boolean ret = cdss.conflict(so3, vop, mpg, BT, Math.min(NT, HT));
//f.pln("cdsspolygon.conflict("+so3+",\n"+vop+",\n"+mpg+",\n"+BT+",\n"+Math.min(NT, HT)+") = "+ret);	          
//boolean ret2 = CDPolyIter.conflict2(so3, vop, mpg, BT, Math.min(NT, HT));
//if (ret != ret2) {
//		int k = 0;
//		MovingPolygon3D mp3 = mpg.copy();
//		while (k < mp3.size() && mp3.size() > 3) {
//			MovingPolygon3D tmp = mp3.copy();
//			tmp.horizpoly.polystart.remove(k);
//			tmp.horizpoly.polyvel.remove(k);
//			boolean ret3 = cdss.conflict(so3, vop, tmp, BT, Math.min(NT, HT));
//			boolean ret4 = CDPolyIter.conflict2(so3, vop, tmp, BT, Math.min(NT, HT));
//			if (ret3 != ret4) {
//				mp3 = tmp;
//			} else {
//				k++;
//			}
//		}
////	for (double t = BT; t < Math.min(NT, HT); t++) {
////        boolean ret3 = cdss.conflict(so3, vo, mpg, t, t+5.0); 
////        boolean ret4 = CDPolyIter.conflict2(so3, vo, mpg, t, t+5.0);
//////		if (ret3 != ret4) 
////			f.pln("LL time "+(t+t0)+" old="+ret3+" new="+ret4);
////	}
//		double tt = Math.min(NT, HT);
//		mp3.horizpoly.tend = tt;
//	f.pln("CDSIPolygon.conflictLL test FAILED old "+ret+"!= new "+ret2+" t0="+t0);
//	f.pln("so="+so3.toStringNP("m", "m", "m", 8));
//	f.pln("vo="+vop.toXYZ("m/s", "m/s", "m/s", 8));
//	f.pln("mp="+mp3);
//	f.pln("B="+BT);
//	f.pln("T="+tt);
//	f.pln("ownship,"+so3.toStringNP("NM", "NM", "ft", 8)+",-,0");
//	f.pln("ownship,"+so3.AddScal((tt-B), vop).toStringNP("NM", "NM", "ft", 8)+",-,"+(tt-B));
//	f.pln(mp3.toOutput("poly"));
//}
	          if (ret) return true;          
	        }
	        t_base = intent.getTime(j+1); // ignored if last value
	      }
	    }
	    return false;
	  }
  
  
  
  public String toString() {
    return "CDSIPolygon: cd = "+cdss.toString();
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

  public void setCorePolygonDetection(DetectionPolygon d) {
    cdss = d.copy();
    tin.clear();
    tout.clear();
    tca.clear();
    dist_tca.clear();
  }

  public DetectionPolygon getCorePolygonDetection() {
    return cdss;
  }

}

