/*
 * CriticalVectors.java 
 * Release: ACCoRDj-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import static gov.nasa.larcfm.ACCoRD.Consts.*;
import java.util.ArrayList;
import java.util.List;

import gov.nasa.larcfm.Util.*;

public class CriticalVectors {

  public static List<Vect2> tracks(Vect3 s, Vect3 vo, Vect3 vi,
      double D, double H, double B, double T) {
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();

    if (Util.almost_equals(vo.z,vi.z) && Math.abs(s.z) < H) {
      return CriticalVectors2D.tracks(s2,vo2,vi2,D,B,T);
    } else if (!Util.almost_equals(vo.z,vi.z)) {
      double b = Math.max(Vertical.Theta_H(s.z,vo.z-vi.z,Entry,H),B);
      double t = Math.min(Vertical.Theta_H(s.z,vo.z-vi.z,Exit,H),T);
      if (b < t) {
        return CriticalVectors2D.tracks(s2,vo2,vi2,D,b,t); 
      }
    }
    return new ArrayList<Vect2>();
  }

  public static List<Vect2> tracks(Vect3 s, Vect3 vo, Vect3 vi,
      double D, double H, double T) {
    return tracks(s,vo,vi,D,H,0,T);
  }

  public static List<Vect2> groundSpeeds(Vect3 s, Vect3 vo, Vect3 vi,
      double D, double H, double B, double T) {
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();

    if (Util.almost_equals(vo.z,vi.z) && Math.abs(s.z) < H) {
      return CriticalVectors2D.groundSpeeds(s2,vo2,vi2,D,B,T);
    } else if (!Util.almost_equals(vo.z,vi.z)) {
      double b = Math.max(Vertical.Theta_H(s.z,vo.z-vi.z,Entry,H),B);
      double t = Math.min(Vertical.Theta_H(s.z,vo.z-vi.z,Exit,H),T);
      if (b < t) {
        return CriticalVectors2D.groundSpeeds(s2,vo2,vi2,D,b,t); 
      }
    }
    return new ArrayList<Vect2>();
  }

  public static List<Vect2> groundSpeeds(Vect3 s, Vect3 vo, Vect3 vi,
      double D, double H, double T) {
    return groundSpeeds(s,vo,vi,D,H,0,T);
  }

  private static List<Double> verticalSpeeds1D(double sz, double viz,
      double H, double B, double T) {
    List<Double> vss = new ArrayList<Double>();
    Vertical nvoz;
    if (Math.abs(sz) < H && B > 0) {
      for (int eps=-1;eps <= 1; eps+=2) {
        nvoz = Vertical.vs_circle_at(sz,viz,B,eps,Exit,H);
        if (!nvoz.undef()) 
          vss.add(nvoz.z);
      }
    } else if (Math.abs(sz) >= H && B > 0) {
      nvoz = Vertical.vs_circle_at(sz,viz,B,-Util.sign(sz),Exit,H);
      if (!nvoz.undef()) 
        vss.add(nvoz.z);
    }
    if (Math.abs(sz) >= H) {
      nvoz = Vertical.vs_circle_at(sz,viz,T,Util.sign(sz),Entry,H);
      if (!nvoz.undef()) 
        vss.add(nvoz.z);
    }
    return vss;
  }

  public static List<Double> verticalSpeeds(Vect3 s, Vect3 vo, Vect3 vi,
      double D, double H, double B, double T) {
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
//f.pln("CriticalVectors.verticalSpeeds "+s + " "+vo+" "+vi);      

    if (vo2.almostEquals(vi2) && s2.sqv() < Util.sq(D)) {
//f.pln("CriticalVectors.verticalSpeeds 1 "+verticalSpeeds1D(s.z,vi.z,H,B,T));      
      return verticalSpeeds1D(s.z,vi.z,H,B,T);
    } else {
      Vect2 v2 = vo2.Sub(vi2);
      if (Horizontal.Delta(s2,v2,D) > 0) {
        double b = Math.max(Horizontal.Theta_D(s2,v2,Entry,D),B);
        double t = Math.min(Horizontal.Theta_D(s2,v2,Exit,D),T);
//f.pln("CriticalVectors.verticalSpeeds 2 "+verticalSpeeds1D(s.z,vi.z,H,b,t)+" b="+b+" t="+t);      
        if (b < t) {
//f.pln("CriticalVectors.verticalSpeeds 3 "+verticalSpeeds1D(s.z,vi.z,H,b,t));      
          return verticalSpeeds1D(s.z,vi.z,H,b,t); 
        }
      }
    }
    return new ArrayList<Double>();  
  }

  public static List<Double> verticalSpeeds(Vect3 s, Vect3 vo, Vect3 vi,
      double D, double H, double T) {
    return verticalSpeeds(s,vo,vi,D,H,0,T);
  }
  
}
