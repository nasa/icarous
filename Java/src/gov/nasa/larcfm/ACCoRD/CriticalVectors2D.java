/*
 * CriticalVectors2D.java 
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

import gov.nasa.larcfm.Util.Vect2;

public class CriticalVectors2D {

  public static List<Vect2> tracks(Vect2 s, Vect2 vo, Vect2 vi,
      double D, double B, double T) {
    TangentLine nv;
    List<Vect2> trks = new ArrayList<Vect2>();
    Horizontal nvo;
    if (Horizontal.horizontal_sep(s,D)) {
      for (int eps=-1;eps <= 1; eps+=2) {
        nv = new TangentLine(s,D,eps);
        nvo = Horizontal.trk_line_irt(nv,vo,vi,1);
        if (!nvo.undef()) 
          trks.add(nvo);
        nvo = Horizontal.trk_line_irt(nv,vo,vi,-1);
        if (!nvo.undef())  
          trks.add(nvo);
        nvo = Horizontal.trk_only_circle(s,vo,vi,T,Entry,eps,D);
        if (!nvo.undef())       
          trks.add(nvo);
      }
    }
    if (B > 0) {
      for (int eps=-1;eps <= 1; eps+=2) {
        nvo = Horizontal.trk_only_circle(s,vo,vi,B,Exit,eps,D);
        if (!nvo.undef())       
          trks.add(nvo);
      }   
    } 
    return trks;
  }

  public static List<Vect2> tracks(Vect2 s, Vect2 vo, Vect2 vi,
      double D, double T) {
    return tracks(s,vo,vi,D,0,T);
  }

  public static List<Vect2> groundSpeeds(Vect2 s, Vect2 vo, Vect2 vi,
      double D, double B, double T) {
    TangentLine nv;
    List<Vect2> gss = new ArrayList<Vect2>();
    Horizontal nvo;
    if (Horizontal.horizontal_sep(s,D)) {
      for (int eps=-1;eps <= 1; eps+=2) {
        nv = new TangentLine(s,D,eps);
        nvo = Horizontal.gs_line(nv,vo,vi);
        if   (!nvo.undef())
          gss.add(nvo);
        nvo = Horizontal.gs_only_circle(s,vo,vi,T,Entry,eps,D);
        if (!nvo.undef())       
          gss.add(nvo);
      }
    }      
    if (B > 0) {
      for (int eps=-1;eps <= 1; eps+=2) {
        nvo = Horizontal.gs_only_circle(s,vo,vi,B,Exit,eps,D);
        if (!nvo.undef())       
          gss.add(nvo);
      }   
    } 
    return gss;
  }

  public static List<Vect2> groundSpeeds(Vect2 s, Vect2 vo, Vect2 vi,
      double D, double T) {
    return groundSpeeds(s,vo,vi,D,0,T);
  }

}
