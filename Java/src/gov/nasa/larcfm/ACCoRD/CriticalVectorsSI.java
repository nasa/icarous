/*
 * CriticalVectorsSI.java 
 *
 * Contact: George Hagen
 * Organization: NASA/Langley Research Center
 *
 * This class computes the critical vectors for computing bands with traffic fp.
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;


import gov.nasa.larcfm.ACCoRD.CriticalVectors;
import gov.nasa.larcfm.Util.EuclideanProjection;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Projection;
//import gov.nasa.larcfm.Util.SimpleProjection;
//import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
//import gov.nasa.larcfm.Util.GreatCircle;
import gov.nasa.larcfm.Util.LatLonAlt;
//import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;
import java.util.List;


public class CriticalVectorsSI{

  // note: must use XYZ coordinates!
  public static List<Vect2> tracks(Vect3 so, Vect3 vo, double to, Plan fp,
      double D, double H, double B, double T) {
      
      ArrayList<Vect2> lst = new ArrayList<Vect2>();
      
      for(int i = 0; i < fp.size()-1; i++) {
        double fi_st = fp.time(i);
        double fi_et = fp.time(i+1);
        if(fi_et > B+to && fi_st < T+to) {
		  boolean linear = true;     // was linear);
          Vect3 vi = fp.initialVelocity(i, linear);
          double st = Util.max(fi_st,B+to);
          double et = Util.min(fi_et,T+to);
          Vect3 si = fp.point(i).point();
          Vect3 si_o = si.Sub(vi.Scal(fi_st-to));
          Vect3 s = so.Sub(si_o);
          double rel_st = st-to;
          double rel_et = et-to;
          lst.addAll(CriticalVectors.tracks(s,vo,vi,D,H,rel_st,rel_et));
        }
      }
    return lst;
  }

  

  public static List<Vect2> groundSpeeds(Vect3 so, Vect3 vo, double to, Plan fp,
      double D, double H, double B, double T) {
    ArrayList<Vect2> lst = new ArrayList<Vect2>();
    
    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
		boolean linear = true;     // was linear);
        Vect3 vi = fp.initialVelocity(i, linear);
        double st = Util.max(fi_st,B+to);
        double et = Util.min(fi_et,T+to);
        Vect3 si = fp.point(i).point();
        Vect3 si_o = si.Sub(vi.Scal(fi_st-to));
        Vect3 s = so.Sub(si_o);
        double rel_st = st-to;
        double rel_et = et-to;
        
        lst.addAll(CriticalVectors.groundSpeeds(s,vo,vi,D,H,rel_st,rel_et));
      }
    }
    return lst;
  }

  

  public static List<Double> verticalSpeeds(Vect3 so, Vect3 vo, double to, Plan fp,
      double D, double H, double B, double T) {
    ArrayList<Double> lst = new ArrayList<Double>();
    
    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
  		boolean linear = true;     // was linear);
        Vect3 vi = fp.initialVelocity(i, linear);
        double st = Util.max(fi_st,B+to);
        double et = Util.min(fi_et,T+to);
        Vect3 si = fp.point(i).point();
        Vect3 si_o = si.Sub(vi.Scal(fi_st-to));
        Vect3 s = so.Sub(si_o);
        double rel_st = st-to;
        double rel_et = et-to;
        
        lst.addAll(CriticalVectors.verticalSpeeds(s,vo,vi,D,H,rel_st,rel_et));
      }
    }
    return lst;
  }


  public static List<Vect2> tracksLL(double lat, double lon, double alt, Velocity vo, double to, Plan fp,
                                     double D, double H, double B, double T) {
    ArrayList<Vect2> lst = new ArrayList<Vect2>();
    
    LatLonAlt so = LatLonAlt.mk(lat, lon, alt);
    EuclideanProjection proj = Projection.createProjection(so.zeroAlt()); // this will preserve altitudes in Vect3 space
    Velocity vop = proj.projectVelocity(so, vo);
    
    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
  		boolean linear = true;     // was linear);
        Velocity vi = fp.initialVelocity(i, linear);
        //double trk = vi.vect2().track();
        //double gs = vi.vect2().norm();
        double st = Util.max(fi_st,B+to);
        double et = Util.min(fi_et,T+to);
        
        LatLonAlt si = fp.point(i).lla();
        double dt = fi_st - to;
		
//		Velocity vop = vo;
//		Velocity vip = vi;

//EuclideanProjection proj = Projection.getProjection(si);
//Velocity vip = proj.projectVelocity(si, vi);
//LatLonAlt si_o = GreatCircle.linear_initial(si, vip, -dt); // CHANGED!!!
//Vect3 s = proj.project(so).Sub(proj.project(si_o));
//Velocity vop = proj.projectVelocity(so, vo);

        
        Velocity vip = proj.projectVelocity(si, vi);
//        LatLonAlt si_o = GreatCircle.linear_initial(si, vip, -dt); // CHANGED!!!
        Vect3 s = proj.project(so).Sub(proj.project(si).AddScal(-dt, vip));
//        Velocity vop = proj.projectVelocity(so, vo);

        
        double rel_st = st-to;
        double rel_et = et-to;
        lst.addAll(CriticalVectors.tracks(s,vop,vip,D,H,rel_st,rel_et));  //CHANGED!!!
      }
    }
    return lst;
  }
  
  public static List<Vect2> groundSpeedsLL(double lat, double lon, double alt, Velocity vo, double to, Plan fp,
                                           double D, double H, double B, double T) {
    ArrayList<Vect2> lst = new ArrayList<Vect2>();

    LatLonAlt so = LatLonAlt.mk(lat, lon, alt);
    EuclideanProjection proj = Projection.createProjection(so.zeroAlt()); // this will preserve altitudes in Vect3 space
    Velocity vop = proj.projectVelocity(so, vo);

    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
  		boolean linear = true;     // was linear);
        Velocity vi = fp.initialVelocity(i, linear);
        double st = Util.max(fi_st,B+to);
        double et = Util.min(fi_et,T+to);
        
        LatLonAlt si = fp.point(i).lla();
        double dt = fi_st - to;

//        Velocity vop = vo;
//        Velocity vip = vi;

//EuclideanProjection proj = Projection.getProjection(si);
//Velocity vip = proj.projectVelocity(si, vi);
//LatLonAlt si_o = GreatCircle.linear_initial(si, vip, -dt); // CHANGED!!!
//Vect3 s = proj.project(so).Sub(proj.project(si_o));
//Velocity vop = proj.projectVelocity(so, vo);

        
        Velocity vip = proj.projectVelocity(si, vi);
//        LatLonAlt si_o = GreatCircle.linear_initial(si, vip, -dt); // CHANGED!!!
        Vect3 s = proj.project(so).Sub(proj.project(si).AddScal(-dt, vip));
//      Velocity vop = proj.projectVelocity(so, vo);

 
        double rel_st = st-to;
        double rel_et = et-to;
        
        lst.addAll(CriticalVectors.groundSpeeds(s,vop,vip,D,H,rel_st,rel_et)); //CHANGED!!!
      }
    }
    return lst;
  }
  
  public static List<Double> verticalSpeedsLL(double lat, double lon, double alt, Velocity vo, double to, Plan fp,
                                            double D, double H, double B, double T) {
    ArrayList<Double> lst = new ArrayList<Double>();
    
    LatLonAlt so = LatLonAlt.mk(lat, lon, alt);
    EuclideanProjection proj = Projection.createProjection(so.zeroAlt()); // this will preserve altitudes in Vect3 space

    Velocity vop = proj.projectVelocity(so, vo);
  
    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
  		boolean linear = true;     // was linear);
        Velocity vi = fp.initialVelocity(i, linear);
        double st = Util.max(fi_st,B+to);
        double et = Util.min(fi_et,T+to);
        
        LatLonAlt si = fp.point(i).lla();
        double dt = fi_st - to;
		
//        Velocity vop = vo;
//        Velocity vip = vi;

//EuclideanProjection proj = Projection.getProjection(si);
//Velocity vip = proj.projectVelocity(si, vi);
//LatLonAlt si_o = GreatCircle.linear_initial(si, vip, -dt); // CHANGED!!!
//Vect3 s = proj.project(so).Sub(proj.project(si_o));
//Velocity vop = proj.projectVelocity(so, vo);

        Velocity vip = proj.projectVelocity(si, vi);
//		LatLonAlt si_o = GreatCircle.linear_initial(si, vip, -dt); // CHANGED!!!
        Vect3 s = proj.project(so).Sub(proj.project(si).AddScal(-dt, vip));
//      Velocity vop = proj.projectVelocity(so, vo);
       
        double rel_st = st-to;
        double rel_et = et-to;
        
        lst.addAll(CriticalVectors.verticalSpeeds(s,vop,vip,D,H,rel_st,rel_et)); //CHANGED!!!
      }
    }
    return lst;
  }
  
  
}
