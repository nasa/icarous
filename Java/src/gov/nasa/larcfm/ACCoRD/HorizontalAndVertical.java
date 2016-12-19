/*
 * HorizontalAndVertical.java 
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

import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;

/** Horizontal solution */
public class HorizontalAndVertical extends Vect3  {
   
  /** Construct a horizontal solution */
  HorizontalAndVertical() {
    super(0.0, 0.0, 0.0);
  }

  /** Construct a horizontal solution */
  HorizontalAndVertical(Vect3 v) {
    super(v.x, v.y, v.z);
 }
  
  /** Is this solution undefined? */
  public boolean undef() {
    return isZero();
  }

  /** "Solution" indicating no solution: Warning do not test with == use undef instead! */
  static final HorizontalAndVertical NoHorizontalAndVerticalSolution = new HorizontalAndVertical();

  /* Solve the following equation on k and l:
   *   k*nv = l*vo-vi.
   */
  /** Solve the following equation on k and l:
   *   k*nv = l*vo-vi.
   */
  public static HorizontalAndVertical speed_only(Vect2 nv,Vect3 vo,Vect2 vi) {
    Vect2 vo2 = vo.vect2();
    double det_vo_v = vo2.det(nv);
    if (det_vo_v != 0) {
      double l = Util.max(0,vi.det(nv) / det_vo_v);
      double k = vi.det(vo2) / det_vo_v;
      if (k > 0 && l > 0) {
        HorizontalAndVertical gso = new HorizontalAndVertical(vo.Scal(l));
        return gso;
      }
    }
    return NoHorizontalAndVerticalSolution;
  }
  
  public String toString() {
    if (undef())
      return "Undef";
    return super.toString();
  }

}
