/*
 * Horizontal.java 
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
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;

/** Horizontal solution */
public class Horizontal extends Vect2  {

  /** parameter */
  private double  k;
  
  /** Construct a horizontal solution */
  Horizontal() {
    super(0.0, 0.0);
    k = 0;
  }

  /** Construct a horizontal solution */
  Horizontal(Vect2 v) {
    super(v.x, v.y);
    k = 1;    
    //x = v.x;
    //y = v.y;
  }

  /** Construct a horizontal solution */
  private Horizontal(double kk, Vect2 v) {
    super(v.x, v.y);
    k = kk;
  }
  
  /** Is this solution undefined? */
  public boolean undef() {
    return isZero();
  }

  /** "Solution" indicating no solution: Warning do not test with == use undef instead! */
  static final Horizontal NoHorizontalSolution = new Horizontal();

  /** */
  static Horizontal best_horizontal(Vect2 vo,Horizontal v1,Horizontal v2) {
    if (v1.undef())
      return v2;
    else if (v2.undef() || v1.leq(v2,vo))
      return v1;
    return v2;
  }

  /**
   * Time of closest point of approach.
   * 
   * @param v Vector
   * 
   * @return the time of horizontal closest point of approach between <code>this</code> point and the line defined
   * by the vector <code>v</code>. 
   */
  public static double tcpa(Vect2 s, Vect2 v) {
    if (!v.isZero())
      return -s.dot(v)/v.sqv();
    return 0;
  }

  /**
   * Distance closest point of approach.
   * 
   * @param v Vector
   * 
   * @return the horizontal distance at closest point of approach between <code>this</code> point and the line defined
   * by the vector <code>v</code>. 
   */
  public static double dcpa(Vect2 s, Vect2 v) {
      return v.ScalAdd(tcpa(s,v),s).norm();
  }
  
  /* Horizontal miss distance within lookahead time */
  public static double hmd(Vect2 s, Vect2 v, double T) {
    double t = 0;
    if (s.dot(v) < 0) {
      // aircraft are horizontally converging
      t = Util.min(tcpa(s,v),T);
    }
    return v.ScalAdd(t,s).norm();
  }
  
  /**
   * Intersection time between line and circle.
   * 
   * @param v Vector
   * @param D Diameter of of circle
   * @param eps +-1
   * 
   * @return the time <i>t</i> such that the parametric line <code>s</code>+<i>t</i><code>v</code>
   * intersects the circle of radius <code>D</code>. If <code>eps == -1</code> the returned time is 
   * the entry time; if <code>eps == 1</code> the  returned time is the exit time.
   */
  public static double Theta_D(Vect2 s, Vect2 v, int eps, double D) {
    double a = v.sqv();
    double b = s.dot(v);
    double c = s.sqv()-Util.sq(D);
    return Util.root2b(a,b,c,eps);
  }  

  /**
   * Discriminant of intersection between line and circle.
   * 
   * @param v Vector
   * @param D Diameter of of circle
   * 
   * @return the discriminant of the intersection between the parametric line 
   * <code>s</code>+<i>t</i><code>v</code> and the circle of radius <code>D</code>. 
   * If the discriminant is less than <code>0</code> the intersection doesn't exist;
   * if it is <code>0</code> the line is tangent to the circle; otherwise, the line
   * intersects the circle in two different points. 
   */
  public static double Delta(Vect2 s, Vect2 v, double D) {
    return Util.sq(D)*v.sqv() - Util.sq(s.det(v));
  }

  /** */
  public static boolean almost_horizontal_los(Vect2 s, double D) {
    double sqs = s.sqv();
    double sqD = Util.sq(D);
    return !Util.almost_equals(sqs,sqD) && sqs < sqD;
  }

  /** */
  public static boolean horizontal_sep(Vect2 s, double D) {
    return s.sqv() >= Util.sq(D);
  }

  /** */
  public static boolean horizontal_dir(Vect2 s, Vect2 v, int dir) {
    return dir*s.dot(v) >= 0; 
  }

  /** */
  public static boolean horizontal_dir_at(Vect2 s, Vect2 v,double t,int dir) {
    Vect2 sp = v.ScalAdd(t,s);
    return horizontal_dir(sp,v,dir);
  }
  
   static boolean horizontal_entry(Vect2 s,Vect2 v) {
    return horizontal_dir(s,v,-1);
  }

  /** */
  static Vect2 Vdir(Vect2 s,Vect2 v) {
    Vect2 ps = s.PerpR();
    return ps.Scal(Util.sign(ps.dot(v)));
  }

  /** */
  static Vect2 W0(Vect2 s,double j) {
    if (!s.isZero()) 
      return s.Scal(j / s.sqv());
    return Vect2.ZERO;
  }

  /** Solve the following equation on k and l:
   *   k*nv = l*vo-vi.
   */
  public static Horizontal gs_only_line(Vect2 nv,Vect2 vo,Vect2 vi) {
    double det_vo_v = vo.det(nv);
    if (det_vo_v != 0) {
      double l = Util.max(0,vi.det(nv) / det_vo_v);
      double k = vi.det(vo) / det_vo_v;
      Horizontal gso = new Horizontal(vo.Scal(l));
      gso.k = k;
//      if (gso.norm() > 1E7) {
//        f.pln("$$$$$$$$$$$$$$$$$ Horizontal.gs_only_line: gso.norm() = "+gso.norm()+" gso.undef() = "+gso.undef()
//             +" k = "+k+"  det_vo_v = "+det_vo_v);
//        f.pln("$$$$$$$$$$$$$$$$$ Horizontal.gs_only_line   nv = "+nv+ " vo = "+vo+" vi = "+vi);
//      }
      //f.pln("$$$$$$$$$$$$$$$$$ Horizontal.gs_only_line: gso = "+gso);
      return gso;
    }
    return NoHorizontalSolution;
  }

  /** */
  public static Horizontal gs_line(Vect2 nv,Vect2 vo,Vect2 vi) {
    Horizontal gso = gs_only_line(nv,vo,vi);
    if (gso.k < 0) 
      gso = new Horizontal(gso.k, Vect2.ZERO);
      //gso.zero();
    return gso;
  }

  /** */
  public static Horizontal gs_only_dot(Vect2 u,Vect2 vo,Vect2 vi,double j) {
    return gs_only_line(Vdir(u,vo.Sub(vi)),vo,vi.Add(W0(u,j)));
  }

  /** */
  public static Horizontal gs_only_vertical(Vect2 s, Vect2 vo, Vect2 vi, 
      double th, int dir, double D) {
    Vect2 v = vo.Sub(vi);
    if (Delta(s,v,D) > 0) {
      double td = Theta_D(s,v,dir,D);
      if (td > 0) {
        Vect2 p = v.ScalAdd(td,s);
        return gs_only_dot(p.Scal(th),vo,vi,Util.sq(D)-s.dot(p));
      }
    }
    return NoHorizontalSolution;
  }

  /** */
  public static Horizontal gs_vertical(Vect3 s, Vect3 vo, Vect3 vi, TangentLine l, int epsv,
      double D, double H) {
    if (!Util.almost_equals(vo.z,vi.z)) {
      Vect3  v   = vo.Sub(vi);
      int    dir = Math.abs(s.z) >= H ? epsv*Util.sign(s.z) : Entry;
      double t   = Vertical.Theta_H(s.z,vo.z-vi.z,-dir,H);
      if (t > 0 && epsv == Util.sign(s.z + t*v.z)) {
        Horizontal nvo2 = gs_only_vertical(s.vect2(),vo.vect2(),vi.vect2(),t,dir,D);
        if (almost_horizontal_los(s.vect2(),D) || 
            l.horizontal_criterion(nvo2.Sub(vo.vect2())))
          return nvo2;
      }
    }
    return NoHorizontalSolution;
  }

  /** */
  public static Horizontal gs_only(TangentLine nv, Vect3 s, Vect3 vo, Vect3 vi,int epsv,
      double D, double H) {
    return best_horizontal(vo.vect2(),gs_line(nv,vo.vect2(),vi.vect2()),
        gs_vertical(s,vo,vi,nv,epsv,D,H));
  }

  /** */
  public static Horizontal gs_only_circle(Vect2 s,Vect2 vo,Vect2 vi,
      double t,int dir,int irt,double D) {
    Vect2 w = s.Sub(vi.Scal(t));
    double a = Util.sq(t)*vo.sqv();                
    double b = t*(w.dot(vo));
    double c = w.sqv()-Util.sq(D);
    double l = Util.root2b(a,b,c,irt);
    if (!Double.isNaN(l)) {
      Vect2 nvo = vo.Scal(Util.max(l,0));
      if (horizontal_dir_at(s,nvo.Sub(vi),t,dir))
        return new Horizontal(nvo);
    }
    return NoHorizontalSolution;
  }  

  /** */
  public static Horizontal gs_circle(Vect3 s,Vect3 vo,Vect3 vi,
      int dir,int irt,double D,double H) {
    if (!Util.almost_equals(vo.z,vi.z)) {
      double t = Vertical.Theta_H(s.z,vo.z-vi.z,-dir,H);
      return gs_only_circle(s.vect2(),vo.vect2(),vi.vect2(),t,dir,irt,D);
    }
    return NoHorizontalSolution;
  }  

  /* Solve the following equation on k:
   * || k*nv + vi || = || vo ||.
   */
  /** Solve the following equation on k:
   * || k*nv + vi || = || vo ||.
   */
  public static Horizontal trk_only_line_irt(Vect2 nv,Vect2 vo,Vect2 vi,int irt) {
    double a = nv.sqv();
    double b = nv.dot(vi);
    double c = vi.sqv() - vo.sqv();
    double k = Util.root2b(a,b,c,irt);
    if (!Double.isNaN(k)) {
      //Horizontal trko = new Horizontal(nv);
      //trko.k = k;
      //trko.scal(k);
      //trko.add(vi);
      //return trko;
      return new Horizontal(k, nv.ScalAdd(k,vi));
    }
    return NoHorizontalSolution;
  }

  /** */
  public static Horizontal trk_only_line(Vect2 nv,Vect2 vo,Vect2 vi) {
    return best_horizontal(vo,trk_only_line_irt(nv,vo,vi,1),
        trk_only_line_irt(nv,vo,vi,-1));
  }

  /** */
  public static Horizontal trk_line_irt(Vect2 nv,Vect2 vo,Vect2 vi,int irt) {
    Horizontal trko = trk_only_line_irt(nv,vo,vi,irt);
    if (trko.k < 0) {
      trko = new Horizontal(trko.k, Vect2.ZERO);
      //trko.zero();
    }
    return trko;
  }

  /** */
  public static Horizontal trk_line(Vect2 nv,Vect2 vo,Vect2 vi) {
    return best_horizontal(vo,trk_line_irt(nv,vo,vi,1),
        trk_line_irt(nv,vo,vi,-1));
  }

  /** */
  public static Horizontal trk_only_dot(Vect2 u,Vect2 vo,Vect2 vi,double j,int irt) {
    return trk_only_line_irt(Vdir(u,vo.Sub(vi)),vo,vi.Add(W0(u,j)),irt);
  }

  /** */
  public static Horizontal trk_only_vertical(Vect2 s, Vect2 vo, Vect2 vi, 
      double th, int dir, int irt, double D) {
    Vect2 v = vo.Sub(vi);
    if (Delta(s,v,D) > 0) {
      double td = Theta_D(s,v,dir,D);
      if (td > 0) {
        Vect2 p = v.ScalAdd(td,s);
        return trk_only_dot(p.Scal(th),vo,vi,Util.sq(D)-s.dot(p),irt);
      }
    }
    return NoHorizontalSolution;
  }

  /** */
  public static Horizontal trk_vertical_irt(Vect3 s, Vect3 vo, Vect3 vi, TangentLine l, int epsv,
      int irt, double D, double H) {
    if (!Util.almost_equals(vo.z,vi.z)) {
      Vect3  v   = vo.Sub(vi);
      int    dir = Math.abs(s.z) >= H ? epsv*Util.sign(s.z) : Entry;
      double t   = Vertical.Theta_H(s.z,vo.z-vi.z,-dir,H);
      if (t > 0 && epsv == Util.sign(s.z + t*v.z)) {
        Horizontal nvo2 = trk_only_vertical(s.vect2(),vo.vect2(),vi.vect2(),t,dir,irt,D);
        if (almost_horizontal_los(s.vect2(),D) || 
            l.horizontal_criterion(nvo2.Sub(vo.vect2())))
          return nvo2;
      }
    }
    return NoHorizontalSolution;
  }
  
  /** */
  public static Horizontal trk_vertical(Vect3 s, Vect3 vo, Vect3 vi, TangentLine l, int epsv,
      double D, double H) {
    return best_horizontal(vo.vect2(),trk_vertical_irt(s,vo,vi,l,epsv,1,D,H),
        trk_vertical_irt(s,vo,vi,l,epsv,-1,D,H));
  }

  /** */
  public static Horizontal trk_only(TangentLine nv, Vect3 s, Vect3 vo, Vect3 vi,int epsv,
      double D, double H) {
    return best_horizontal(vo.vect2(),trk_line(nv,vo.vect2(),vi.vect2()),
        trk_vertical(s,vo,vi,nv,epsv,D,H));
  }

  /** */
  public static Horizontal trk_only_circle(Vect2 s,Vect2 vo,Vect2 vi,
      double t,int dir,int irt,double D) {
    if (t > 0) {
      Vect2 w = s.AddScal(-t, vi); // s.Sub(vi.Scal(t));
      double e = (Util.sq(D) - s.sqv() - Util.sq(t)*(vo.sqv()-vi.sqv())) / (2*t);
      if (!s.almostEquals(vi.Scal(t))) {
        Horizontal nvo = Horizontal.trk_only_dot(w,vo,vi,e,irt);
        if (horizontal_dir_at(s,nvo.Sub(vi),t,dir))
          return nvo;
      }
    }
    return NoHorizontalSolution;
  }

  /** */
  public static Horizontal trk_circle(Vect3 s,Vect3 vo,Vect3 vi,
      int dir,int irt,double D,double H) {
    if (!Util.almost_equals(vo.z, vi.z)) {
      double t = Vertical.Theta_H(s.z,vo.z-vi.z,-dir,H);
      return trk_only_circle(s.vect2(),vo.vect2(),vi.vect2(),t,dir,irt,D);
    }
    return NoHorizontalSolution;
  }  

  /**
   * Solve the following equation on k and l:
   *   nv * (k*nv-v) = 0, where v = vo-vi.
   */
  public static Horizontal opt_trk_gs_line(Vect2 nv,Vect2 vo,Vect2 vi) {
    if (!nv.isZero()) {
      //Horizontal trkgs = new Horizontal(nv);
      Vect2 v = vo.Sub(vi);
      double k = nv.dot(v) / nv.sqv();
      //trkgs.k = k;
      //trkgs.scal(k);
      //trkgs.add(vi);
      //return trkgs;
      return new Horizontal(k, nv.ScalAdd(k,vi));
    }
    return NoHorizontalSolution;
  }

  /** */
  public static Horizontal opt_line(Vect2 nv,Vect2 vo,Vect2 vi) {
    Horizontal opt = opt_trk_gs_line(nv,vo,vi);
    if (opt.k < 0)
      opt = new Horizontal(opt.k, Vect2.ZERO);
      //opt.zero();
    return opt;
  }

  /** */
  public static Horizontal opt_trk_gs(TangentLine nv, Vect3 s, Vect3 vo, Vect3 vi,int epsv,
      double D, double H) {
    return best_horizontal(vo.vect2(),opt_trk_gs_line(nv,vo.vect2(),vi.vect2()),
        opt_vertical(s,vo,vi,nv,epsv,D,H));
  }

  /** */
  public static Horizontal opt_trk_gs_dot(Vect2 u,Vect2 vo,Vect2 vi,double j) {
    return opt_trk_gs_line(Vdir(u,vo.Sub(vi)),vo,vi.Add(W0(u,j)));
  }

  /** */
  public static Horizontal opt_trk_gs_vertical(Vect2 s, Vect2 vo, Vect2 vi, 
      double th, int dir, double D) {
    Vect2 v = vo.Sub(vi);
    if (Delta(s,v,D) > 0) {
      double td = Theta_D(s,v,dir,D);
      if (td > 0) {
        Vect2 p = v.ScalAdd(td,s);
        opt_trk_gs_dot(p.Scal(th),vo,vi,Util.sq(D)-s.dot(p));
      }
    }
    return NoHorizontalSolution;
  }

  /** */
  public static Horizontal opt_vertical(Vect3 s, Vect3 vo, Vect3 vi, TangentLine l, int epsv,
      double D, double H) {
    if (!Util.almost_equals(vo.z,vi.z)) {
      Vect3  v   = vo.Sub(vi);
      int    dir = Math.abs(s.z) >= H ? epsv*Util.sign(s.z) : Entry;
      double t   = Vertical.Theta_H(s.z,vo.z-vi.z,-dir,H);
      if (t > 0 && epsv == Util.sign(s.z + t*v.z)) {
        Horizontal nvo2 = opt_trk_gs_vertical(s.vect2(),vo.vect2(),vi.vect2(),t,dir,D);
        if (almost_horizontal_los(s.vect2(),D) || 
            l.horizontal_criterion(nvo2.Sub(vo.vect2())))
          return nvo2;
      }
    }
    return NoHorizontalSolution;
  }

  
  public static boolean hasEpsilonCriticalPointTrack(Vect2 s, Vect2 vo, Vect2 vi, int irt) {
    Horizontal nvo = trk_only_dot(s.PerpR(), vo, vi, 0, irt);
    return nvo != NoHorizontalSolution;
  }

  // if nvo has NoHorizontalSolution, the return is meaningless
  public static double epsilonCriticalPointTrack(Vect2 s, Vect2 vo, Vect2 vi, int irt) {
    Horizontal nvo = trk_only_dot(s.PerpR(), vo, vi, 0, irt);
    if (nvo == NoHorizontalSolution) return -3*Math.PI;
    return nvo.trk();
  }
  
  public static boolean epsilonCriticalPointGSIsValid(Vect2 s, Vect2 vo, Vect2 vi) {
    Horizontal nvo = gs_only_dot(s.PerpR(), vo, vi, 0);
    return nvo != NoHorizontalSolution;
  }

  // if nvo has NoHorizontalSolution, the return is meaningless
  public static double epsilonCriticalPointGS(Vect2 s, Vect2 vo, Vect2 vi) {
    Horizontal nvo = gs_only_dot(s.PerpR(), vo, vi, 0);
    if (nvo == NoHorizontalSolution) return -1.0;
    return nvo.norm();
  }

  
  public String toString() {
    if (undef())
      return "Undef";
    return super.toString();
  }

}
