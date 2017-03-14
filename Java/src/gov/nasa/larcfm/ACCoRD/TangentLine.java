/*
 * TangentLine.java 
 * Release: ACCoRDj-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Computes a vector that is tangent to the protected zone.
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;


/** Tangent line to a circle */
public class TangentLine extends Vect2 {

  private int eps=0;
  
  public TangentLine(TangentLine v) {
    super(v.x, v.y);
    //x   = v.x;
    //y   = v.y;
    eps = v.eps;
  }
  
  /** Q function (relative tangent line) */
  public static Vect2 Q(Vect2 s, double D, int eps) {
    double sq_s = s.sqv();
    double sq_D = Util.sq(D);
    double delta = sq_s -sq_D;
    if (delta >= 0) { 
      double alpha = sq_D/sq_s;
      double beta  = D*Util.sqrt_safe(delta)/sq_s;
      return new Vect2(alpha*s.x+eps*beta*s.y,
          alpha*s.y-eps*beta*s.x);   
    }
    return Vect2.ZERO;
  }
  
  private static double InitTangentLineX(Vect2 s, double D, int eps) {
    double x/*, y*/;
     if (Util.almost_equals(s.sqv(),Util.sq(D)))  {
      x = eps*s.y;
//      y = -eps*s.x;
    } else {
      Vect2 q = Q(s,D,eps);
      x = q.x;
//      y = q.y;
      if (!q.isZero()) {
        x -= s.x;
//        y -= s.y;
      } 
    }
     return x;
  }
  private static double InitTangentLineY(Vect2 s, double D, int eps) {
	    double /*x,*/ y;
	     if (Util.almost_equals(s.sqv(),Util.sq(D)))  {
//	      x = eps*s.y;
	      y = -eps*s.x;
	    } else {
	      Vect2 q = Q(s,D,eps);
//	      x = q.x;
	      y = q.y;
	      if (!q.isZero()) {
//	        x -= s.x;
	        y -= s.y;
	      } 
	    }
	     return y;
	  }
  /** Construct a tangent line */
  public TangentLine(Vect2 s, double D, int eps) {
    super(InitTangentLineX(s,D,eps), InitTangentLineY(s,D,eps));
    this.eps = eps;
  }

  /** Has horizontal criterion been met? */
  public boolean horizontal_criterion(Vect2 v) {
    return eps*det(v) >= 0;    
  }
  
  /** Epsilon value */
  public int get_eps() {
    return eps;
  }
  
  public String toString() {
    return super.toString()+" [eps="+eps+"]";
  }
}
