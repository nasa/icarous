/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.Util;
//import gov.nasa.larcfm.Util.Kinematics;
//import gov.nasa.larcfm.Util.Repulsion;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.ACCoRD.Horizontal;

/**
 * This class provides criteria for
 * 
 *      horizontal conflict               (See ACCoRD PVS theory: horizontal_criterion.pvs)
 *      vertical conflict                 (See ACCoRD PVS theory: vertical_criterion.pvs)
 *      horizontal loss of separation     (See ACCoRD PVS theory: repulsive.pvs)
 *      vertical loss of separation       (See ACCoRD PVS Theory: vertical_los_generic.pvs)
 */
public class CriteriaCore {

  /**
   * Horizontal coordination.
   * 
   * @param s the relative position
   * @param v the relative velocity
   * 
   * @return the horizontal coordination sign
   *   -1 corresponds to turning right in the relative system
   *   +1 corresponds to turning left in the relative system
   */
  public static int horizontalCoordination(Vect2 s, Vect2 v) { 
    return Util.sign(v.det(s));
  }

  /**
   * Horizontal coordination.
   * 
   * @param s the relative position
   * @param v the relative velocity
   * 
   * @return the horizontal coordination sign
   *   -1 corresponds to turning right in the relative system
   *   +1 corresponds to turning left in the relative system
   */
  public static int horizontalCoordination(Vect3 s, Vect3 v) { 
    return horizontalCoordination(s.vect2(),v.vect2());
  }

  /**
   * Vertical coordination.
   * 
   * @param s the relative position
   * @param vo the ownship velocity
   * @param vi the traffic aircraft velocity
   * @param D the horizontal distance
   * @param H the vertical distance
   * @param ownship the name of the ownship
   * @param traffic the name of the traffic aircraft
   * 
   * @return the vertical coordination sign.
   *  -1 corresponds to reducing current vertical speed, 
   *  +1 corresponds to increasing current vertical speed
   */
  public static int verticalCoordination(Vect3 s, Vect3 vo, Vect3 vi, double D, double H, String ownship, String traffic) {
    if (CD3D.LoS(s,D,H)) {
      return verticalCoordinationLoS(s,vo,vi,ownship,traffic);
    } else {
      Vect3 v = vo.Sub(vi);
      return verticalCoordinationConflict(s,v,D,ownship,traffic);
    }
  }

  // This function is defined in PVS theory "space_3D"
  static int verticalCoordinationConflict(Vect3 s, Vect3 v, double D, String ownship, String traffic) {
    Vect2 s2 = s.vect2();
    Vect2 v2 = v.vect2();
    double a = v2.sqv();
    double b = s2.dot(v2);
    double c = s2.sqv()-Util.sq(D);
    double d = Util.sq(b)-a*c;
    if (Util.almost_equals(v.z,0) || v2.isZero() ||
        d < 0 || eq(v.z,Util.sq(b)-a*c,s.z*a-v.z*b)) {
      return breakSymmetry(s, ownship, traffic);
    }
    if (gt(v.z,Util.sq(b)-a*c,s.z*a-v.z*b)) {
      return -1;
    }
    return 1;
  }

  // This function is located in PVS theory "vertical_los":  verticalCoordinationLoS(s,v) : MACRO Sign = dv_to_ec(vertical_decision_vect)(s,v)
  public static int verticalCoordinationLoS(Vect3 s, Vect3 vo, Vect3 vi, String ownship, String traffic) {
    int epsv;
    epsv = losr_vs_dir(s,vo,vi,ACCoRDConfig.NMAC_D, ACCoRDConfig.NMAC_H,ownship,traffic);
    return epsv;

  }

  /**
   * An algebraic way to determine a*sqrt(b) = e.
   * @return true, if a*sqrt(b) = e
   */
  private static boolean eq(double a,double b,double e) {
    return a*e >= 0 && Util.sq(a)*b == Util.sq(e);
  }

  /**
   * An algebraic way to determine a*sqrt(b) > e.
   * @return true, if a*sqrt(b) > e
   */

  private static boolean gt(double a,double b,double e) {
    return a >= 0 ? e < 0 || Util.sq(a)*b > Util.sq(e):
      e < 0 && Util.sq(a)*b < Util.sq(e); 
  }

  private static double R(Vect2 sp, double D) {
    return Math.sqrt(sp.sqv()-Util.sq(D))/D;
  }

  /** The fundamental horizontal criterion (Conflict Case)
   * @param sp  relative position           assumed to be horizontally separated    (Sp_vect2  : TYPE = (horizontal_sep?))
   * @param v   relative velocity
   * @param D   protection zone diameter
   * @param epsh +1 or -1
   */
  public static boolean horizontal_criterion(Vect2 sp, Vect2 v, double D, int epsh) {
    return sp.dot(v) >= R(sp,D)*epsh*sp.det(v);
  }

  /* Derived from PVS function: vertical_criterion.closed_region_3D? */
  private static boolean closed_region_3D(Vect3 s,Vect3 p,int eps, int dir, Vect3 v, double D, double H) {
    Vect2 s2 = s.vect2();
    Vect2 v2 = v.vect2();
    Vect2 p2 = p.vect2();
    double t = (Util.sq(D)-s2.dot(p2))/(v2.dot(p2));
    return v2.dot(p2) != 0 && Util.sign(v2.dot(p2)) == dir && t >= 0 &&  eps*(s.z + t*v.z) >= H &&
        ((Math.abs(s.z) >= H && dir == eps*Util.sign(s.z)) || (Math.abs(s.z) < H && dir == -1));
  }

  /** vertical criterion  (Conflict Case) (currently only used in VisualCriteriaCore) 
   *  derived from PVS function: vertical_criterion.vertical_criterion?
   * @param epsv  vertical coordination parameter
   * @param s  relative position         
   * @param v   relative velocity
   * @param nv  new relative velocity
   * @param D   protection zone diameter
   * @param H   protection zone height
   */
  public static boolean vertical_criterion(int epsv, Vect3 s, Vect3 v, Vect3 nv, double D, double H) {
    int Entry = -1;
    int dir = Entry;
    if (Math.abs(s.z) >= H) dir = epsv*Util.sign(s.z); 
    Vect2 s2 = s.vect2();
    Vect2 v2 = v.vect2();
    Vect3 p = new Vect3(s2.Add(v2.Scal(Horizontal.Theta_D(s2,v2,dir,D))), epsv*H);
    //f.pln(Horizontal.Delta(s2,v2,D)+" "+Horizontal.Theta_D(s2,v2,dir,D));
    return ((v.x == 0 && v.y == 0) && epsv*nv.z >= 0 && epsv*s.z >= H)
        || (Horizontal.Delta(s2,v2,D) > 0 && Horizontal.Theta_D(s2,v2,dir,D) > 0
            && closed_region_3D(s,p,epsv,dir,nv,D,H));
  }

  /*package*/ static boolean vertical_los(double sz, double H) {
    return Math.abs(sz) < H;
  }

  private static boolean horizontal_criterion_0(Vect2 sp, int eps, Vect2 v, double D) {
    if (Util.almost_equals(v.norm(),0.0,Util.PRECISION13))  {
      v = Vect2.ZERO;      // criterion_3D's last term passes in nv - v which is exactly 0 for vertical solutions  
    }
    return horizontal_criterion(sp,v,D,eps);
  }

  private static boolean horizontal_los(Vect2 s, double D) {
    return s.sqv() < Util.sq(D);
  }

  // from PVS
  public static boolean criterion_3D(Vect3 sp, Velocity v, int epsH, int epsV, Velocity nv, double D, double H) {
    return (Horizontal.horizontal_sep(sp.vect2(),D) && horizontal_criterion_0(sp.vect2(),epsH,nv.vect2(),D)) ||
        (vertical_criterion(epsV,sp,v,nv,D,H) &&
            (horizontal_los(sp.vect2(),D) || 
                horizontal_criterion_0(sp.vect2(),epsH,(nv.Sub(v)).vect2(),D)));
  }


  // ***************************************** Repulsive Criteria ******************************************

  // caD and caH are the diameter and height of a collision avoidance zone, e.g., 350ft x 100 ft
  private static Vect3 vertical_decision_vect(Vect3 s, Vect3 vo, Vect3 vi, double caD, double caH) {
    Vect2 s2 = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    Vect3 v = vo.Sub(vi);
    if ((!s.isZero() && CD3D.cd3d(s,vo,vi,caD,caH)) || Util.almost_equals(vo.z,vi.z)) {
      return s;
    } else if (vo2.almostEquals(vi2) || s.isZero())  {
      return v;
    } else if (s2.dot(v.vect2()) <= 0) {
      return s.AddScal(CD2D.tcpa(s2,vo2,vi2),v);
    } else {
      return s;
    }
  }

  // Compute an absolute repulsive vertical direction
  private static int losr_vs_dir(Vect3 s, Vect3 vo, Vect3 vi, double caD, double caH, String ownship, String traffic){
    int rtn = breakSymmetry(vertical_decision_vect(s,vo,vi,caD,caH),ownship,traffic);;
    return rtn;
  }

  /** 
   * [CAM] The method horizontal_old_repulsive_criterion was previously called horizontal_los_criterion, 
   * but the name horizontal_repulsive_criterion is more appropriate.    
   * 
   * Checks whether a horizontal solution is repulsive or divergent in the turn direction indicated by nvo 
   * (2D geometry). Derived from PVS predicate: repulsive.repulsive_criteria(s,v,eps)(nv): bool
   *    @param s      relative position of the ownship
   *    @param vo     velocity of the ownship aircraft
   *    @param vi     velocity of the traffic aircraft
   *    @param nvo    resolution velocity of the traffic aircraft
   *    @param eps    CR3D.horizontalCoordination
   */
  public static boolean horizontal_old_repulsive_criterion(Vect2 s,  Vect2 vo, Vect2 vi, Vect2 nvo, int eps) {
    Vect2 v = vo.Sub(vi);
    Vect2 nv = nvo.Sub(vi);
    boolean rtn = !s.isZero() && !nv.isZero() &&
        eps*s.det(v) <= 0 && eps*s.det(nv) <= 0 
        && ((s.dot(v) < 0 &&  eps*nv.det(v) < 0)
            || (s.dot(v) >= 0 && (!v.isZero() || s.dot(nv) >= 0) &&
            (v.isZero() || s.dot(nv) > s.dot(v))));
    //printRepulsiveCriteria2DTerms(s,vo,vi,nvo,eps);
    return rtn;
  }

  /** 
   * [CAM] This method replaces horizontal_old_repulsive_criterion. This definition is independent 
   * and coordinated for both state-based and iterative algorithms.  This method is intended to be used
   * with kinematic solvers, that is it is called iteratively with nvo progressing with each iteration.
   * 
   * Checks whether a horizontal solution is repulsive or divergent in the turn direction indicated by nvo 
   * (2D geometry). Derived from PVS predicate: repulsive.repulsive_criteria(s,v,eps)(nv): bool
   *    @param s      relative position of the ownship
   *    @param vo     velocity of the ownship aircraft
   *    @param vi     velocity of the traffic aircraft
   *    @param nvo    resolution velocity of the traffic aircraft
   *    @param eps    CR3D.horizontalCoordination
   */
  public static boolean horizontal_new_repulsive_criterion(Vect2 s,  Vect2 vo, Vect2 vi, Vect2 nvo, int eps) {
    Vect2 v = vo.Sub(vi);
    Vect2 nv = nvo.Sub(vi);
    boolean rtn = !s.isZero() && !nv.isZero() &&
        eps*s.det(v) <= 0 && eps*s.det(nv) < 0 
        && ((s.dot(v) < 0 &&  eps*nv.det(v) < 0)
            || (s.dot(v) >= 0
            && (!v.isZero() || s.dot(nv) >= 0) 
            && (v.isZero() || s.dot(nv) > s.dot(v))
            && eps*nv.det(v) <= 0));    
    return rtn;
  }

  public static boolean horizontal_repulsive_criterion(Vect2 s,  Vect2 vo, Vect2 vi, Vect2 nvo, int eps) {
    if (ACCoRDConfig.NewRepulsiveCrit) 
      return horizontal_new_repulsive_criterion(s,vo,vi,nvo,eps);
    else
      return horizontal_old_repulsive_criterion(s,vo,vi,nvo,eps);
  }

  // for debugging 
  public static void print_horizontal_los_terms_SI(Vect2 s,  Vect2 vo, Vect2 vi, Vect2 nvo, int eps) {
    Vect2 v = vo.Sub(vi);
    Vect2 nv = nvo.Sub(vi);
    boolean rtn1 = !s.isZero() && !nv.isZero()  &&
        eps*s.det(v) <= 0 && eps*s.det(nv) < 0;
    boolean rtn2 = (s.dot(v) < 0 &&  eps*nv.det(v) < 0);
    boolean rtn3 =  (s.dot(v) >= 0 
        && (!v.isZero() || s.dot(nv) >= 0) 
        && (v.isZero() || s.dot(nv) > s.dot(v))
        && eps*nv.det(v) <= 0);   
    f.pln("#### repulsiveCriteria, nvo = "+nvo+" vo = "+vo+" vi = "+vi);
    f.pln("#### repulsiveCriteria: s = "+s+" eps*s.det(v) <= 0 = "+(eps*s.det(v) <= 0)+" eps*s.det(nv) <= 0 = "+(eps*s.det(nv) <= 0));
    f.pln("#### repulsiveCriteria: s.dot(v) < 0 = "+(s.dot(v) < 0)+ "  eps*nv.det(v) < 0 = "+(eps*nv.det(v) < 0));
    f.pln("#### repulsiveCriteria: eps = "+eps+ " s.dot(nv) >= 0 = "+(s.dot(nv) >=0));
    f.pln("#### repulsiveCriteria: (s.dot(v) >=0 && s.dot(nv) >s.dot(v)) = "+((s.dot(v) >=0 && s.dot(nv) >s.dot(v))));
    f.pln("#### repulsiveCriteria: s.det(v) = "+s.det(v)+"s.dot(v) = "+s.dot(v)+"  nv.det(v) = " +nv.det(v)+"  s.dot(nv) = " +s.dot(nv));
    f.pln("#### rtn1 = "+rtn1+" rtn2 = "+rtn2+" rtn3 = "+rtn3);
  }

  public static void print_horizontal_los_terms_SI(Vect3 s,  Vect3 vo, Vect3 vi, Vect3 nvo, int eps) {
    print_horizontal_los_terms_SI(s.vect2(),vo.vect2(),vi.vect2(),nvo.vect2(),eps);
  }

  // for debugging 
  public static void print_horizontal_los_terms(Vect2 s,  Vect2 vo, Vect2 vi, Vect2 nvo, int eps) {
    Vect2 v = vo.Sub(vi);
    Vect2 nv = nvo.Sub(vi);
    boolean rtn = eps*s.det(v) <= 0 && eps*s.det(nv) <= 0 
        && (((s.dot(v) < 0 &&  eps*nv.det(v) < 0))
            || (s.dot(v) >= 0 && s.dot(nv) > s.dot(v)));
    f.pln("#### repulsiveCriteria, nvo = "+nvo+" vo = "+vo+" vi = "+vi);
    f.pln("#### repulsiveCriteria: s = "+f.sStr(s)+" eps*s.det(v) <= 0 = "+(eps*s.det(v) <= 0)+" eps*s.det(nv) <= 0 = "+(eps*s.det(nv) <= 0));
    f.pln("#### repulsiveCriteria: s.dot(v) < 0 = "+(s.dot(v) < 0)+ "  eps*nv.det(v) < 0 = "+(eps*nv.det(v) < 0));
    f.pln("#### repulsiveCriteria: eps = "+eps+ " s.dot(nv) >= 0 = "+(s.dot(nv) >=0));
    f.pln("#### repulsiveCriteria: (s.dot(v) >=0 && s.dot(nv) >s.dot(v)) = "+((s.dot(v) >=0 && s.dot(nv) >s.dot(v))));
    f.pln("#### repulsiveCriteria: s.det(v) = "+s.det(v)+" s.dot(v) = "+s.dot(v)+"  nv.det(v) =" +nv.det(v)+"  s.dot(nv) =" +s.dot(nv));
    f.pln("#### rtn = "+rtn);
  }

  // for debugging
  public static void printRepulsiveCriteriaTerms(Vect3 s,  Vect3 vo, Vect3 vi, Vect3 nvo, int eps) {
    print_horizontal_los_terms(s.vect2(),vo.vect2(),vi.vect2(),nvo.vect2(),eps);
  }

  /**
   * Checks whether a horizontal solution is repulsive or divergent in the turn direction indicated by nvo3
   *    @param s3      relative position of the ownship
   *    @param vo3     velocity of the ownship aircraft
   *    @param vi3     velocity of the traffic aircraft
   *    @param nvo3    resolution velocity of the traffic aircraft
   *    @param epsh     CR3D.horizontalCoordination
   */
  public static boolean horizontalRepulsiveCriterion(Vect3 s3,  Velocity vo3, Velocity vi3, Velocity nvo3, int epsh) {
    return horizontal_repulsive_criterion(s3.vect2(),vo3.vect2(), vi3.vect2(),nvo3.vect2(),epsh);
  }

  //  vs_bound_crit?(s,v,eps)(nv): bool =
  //  IF eps*v`z>0
  //    THEN (eps*nv`z>=eps*v`z) AND -eps*v`z*(vect2(nv)*vect2(v)) + eps*nv`z*sqv(vect2(v)) >=0
  //  ELSE eps*nv`z>=0
  //  ENDIF

  private static boolean vs_bound_crit(Vect3 s, Vect3 v, Vect3 nv, int eps) {
    Vect2 v2 = v.vect2();
    boolean rtn;
    if (eps*v.z > 0)
      rtn = eps*nv.z > eps*v.z && -eps*v.z*nv.vect2().dot(v2) + eps*nv.z*v2.sqv() >= 0;
      else
        rtn = eps*nv.z >= 0;
        //f.pln(">>>>>>>>>>>>> vs_bound_crit: eps = "+f.Fmi(eps)+" rtn = "+f.bool2str(rtn));
        return rtn;
  }    

  //    min_rel_vert_speed(sz,vz,eps,MinRelVertSpeed): {x:nnreal | abs(sz)<H IMPLIES x>0} =
  //      IF eps*vz <=0 THEN MinRelVertSpeed
  //      ELSE max(MinRelVertSpeed,abs(vz)) 
  //      ENDIF

  private static double min_rel_vert_speed(double sz, double vz, int eps, double minrelvs) {
    double rtn;
    if (eps*vz <= 0)
      rtn = minrelvs;
    else
      rtn = Util.max(minrelvs,Math.abs(vz));
    return rtn;
  }

  //    z_los_crit?(s,v,eps,MinRelVertSpeed)(nv): bool =
  //      abs(s`z)<H AND
  //      vs_bound_crit?(s,v,eps)(nv) AND
  //      eps*nv`z >= min_rel_vert_speed(s`z,v`z,eps,MinRelVertSpeed)

  /** 
   * [CAM] This method replaces vertical_old_repulsive_criterion. It's intended to be used for kinematic, 
   * iterative maneuvers. This criterion is like vs_bound_crit? but removes the else branch that restricts 
   * some vertical maneuvers.
   * 
   *    @param s      relative position of the ownship
   *    @param vo     velocity of the ownship aircraft
   *    @param vi     velocity of the traffic aircraft
   *    @param nvo    resolution velocity of the traffic aircraft
   *    @param eps    Vertical coordination
   */
  public static boolean vertical_new_repulsive_criterion(Vect3 s, Vect3 vo, Vect3 vi, Vect3 nvo, int eps) {
    Vect3 v = vo.Sub(vi);
    Vect3 nv = nvo.Sub(vi);
    Vect2 v2 = v.vect2();
    return eps*nv.z > eps*v.z && -eps*v.z*nv.vect2().dot(v2) + eps*nv.z*v2.sqv() >= 0;
  }

  /**
   * [CAM] The method vertical_old_repulsive_criterion was previously called vertical_los_criterion, but the name 
   * vertical_repulsive_criterion is more appropriate. This criterion should only be used for state-based  
   * instantaneous maneuvers.  
   */
  private static boolean vertical_old_repulsive_criterion(Vect3 s, Vect3 vo, Vect3 vi, Vect3 nvo, int eps, double H, double minrelvs) {
    Vect3 v = vo.Sub(vi);
    Vect3 nv = nvo.Sub(vi);
    return Math.abs(s.z) < H &&
        vs_bound_crit(s,v,nv,eps) &&
        eps*nv.z >= min_rel_vert_speed(s.z,v.z,eps,minrelvs);
  }

  /** Checks whether a velocity vector satisfies the repulsive LoS criteria
   * @param s         relative position of the ownship
   * @param vo        velocity of the ownship aircraft
   * @param vi        velocity of the traffic aircraft
   * @param nvo       resolution velocity of the traffic aircraft
   * @param H         protection zone height
   * @param minrelvs  minimum relative exit speed
   * @param epsv      vertical coordination parameter
   */
  public static boolean verticalRepulsiveCriterion(Vect3 s, Vect3 vo, Vect3 vi, Vect3 nvo, double H, double minrelvs, int epsv) {
    if (ACCoRDConfig.NewRepulsiveCrit) {
      return vertical_new_repulsive_criterion(s,vo,vi,nvo,epsv);
    } else {
      return vertical_old_repulsive_criterion(s,vo,vi,nvo,epsv,H,minrelvs);
    }
  }

  /** Perform a symmetry calculation */
  private static int breakSymmetry(Vect3 s, String ownship, String traffic) {
    //f.pln(" $$$$ breakSymmetry: s.z = "+Units.str("ft",s.z));
    if (Util.almost_equals(s.z,0)) {
      StringBuffer own = new StringBuffer(ownship);
      ownship = own.reverse().toString();
      StringBuffer traf = new StringBuffer(traffic);
      traffic = traf.reverse().toString();
      return Util.less_or_equal(ownship,traffic) ? 1 : -1;
    } else if (s.z > 0) {
      return 1;
    } else {
      return -1;
    }
  }

  private static boolean trkChanged(Velocity vo, Velocity nvo) {
    return Math.abs(vo.trk() - nvo.trk()) > Units.from("deg",0.001);
  }

  private static boolean gsChanged(Velocity vo, Velocity nvo) {
    return Math.abs(vo.gs() - nvo.gs()) > Units.from("kn",0.001);
  }

  private static boolean vsChanged(Velocity vo, Velocity nvo) {
    return Math.abs(vo.vs() - nvo.vs()) > Units.from("fpm",0.001);
  }

  /**
   *  true iff the velocity vector nvo satisfies the implicit coordination criteria
   *    @param s        relative position of the ownship
   *    @param vo       velocity of the ownship aircraft
   *    @param vi       velocity of the traffic aircraft
   *    @param nvo      velocity vector to be checked
   *    @param minRelVs desired minimum relative exit ground speed (used in LoS only)
   *    @param D        diameter of the protection zone
   *    @param H        height of the protection zone
   *
   */
  public static boolean criteria(Vect3 s, Velocity vo, Velocity vi, Velocity nvo, double minRelVs, double D, double H, int epsh, int epsv) { 
    if (horizontal_los(s.vect2(),D) && vertical_los(s.z,H)) {
      boolean horizChange = trkChanged(vo,nvo) || gsChanged(vo,nvo);
      boolean vertChange = vsChanged(vo,nvo);
      boolean vlc;
      vlc = verticalRepulsiveCriterion(s,vo,vi,nvo,H, minRelVs, epsv);
      boolean hlc;
      hlc = horizontalRepulsiveCriterion(s,  vo, vi, nvo, epsh);
      if (horizChange && vertChange) return hlc && vlc;
      else if (horizChange) return hlc;
      else if (vertChange) return vlc;
      else return (hlc || vlc);
    } else {
      Velocity v = Velocity.make(vo.Sub(vi));
      Velocity nv = Velocity.make(nvo.Sub(vi));
      return criterion_3D(s,v,epsh,epsv,nv,D,H);
    }
  }

  // -------------------- repulsive iterative search directions --------------------------------

  // Compute a new track only vector that is one step to the dir of vo
  // dir = 1 is right
  static Vect2 incr_trk_vect(Vect2 vo, double step, int dir) {
    return vo.Scal(Math.cos(dir*step)).Add(vo.PerpR().Scal(Math.sin(dir*step)));
  }

  // Compute an absolute repulsive track direction (or none)
  // dir = 1 is right
  public static int losr_trk_iter_dir(Vect2 s, Vect2 vo, Vect2 vi, double step, int eps) {
    boolean rtn1 = horizontal_repulsive_criterion(s,vo,vi,incr_trk_vect(vo,step,1),eps);
    boolean rtnM1 = horizontal_repulsive_criterion(s,vo,vi,incr_trk_vect(vo,step,-1),eps);
    if (rtn1)
      return 1;
    if (rtnM1)
      return -1;
    return 0;
  }

  /** Return an absolute repulsive track search direction (or none)
   *  Assumes velocity vo is non-zero.
   * @return +1 search is increasing, -1 search is decreasing
   */
  public static int trkSearchDirection(Vect3 s, Vect3 vo, Vect3 vi, int eps) {
    return losr_trk_iter_dir(s.vect2(),vo.vect2(),vi.vect2(),Units.from("deg",1), eps);
  }

  // Compute a new ground speed only vector that is one step to the dir of vo
  // dir = 1 is increasing. Velocity vo is non-zero.
  static Vect2 incr_gs_vect(Vect2 vo, double step, int dir) {
    double normvo = vo.norm();
    return vo.Scal((normvo+dir*step)/normvo);
  }

  public static int losr_gs_iter_dir(Vect2 s, Vect2 vo, Vect2 vi, double mings, double maxgs, double step, int epsh) {
    if (vo.norm() + step <= maxgs && horizontal_repulsive_criterion(s,vo,vi,incr_gs_vect(vo,step,1),epsh))
      return 1;
    if (vo.norm() - step >= mings && horizontal_repulsive_criterion(s,vo,vi,incr_gs_vect(vo,step,-1),epsh))
      return -1;
    return 0;
  }  

  /** Return an absolute repulsive ground speed search direction (or none)
   *  Assumes Velocity vo is non-zero.
   * @return +1 search is increasing, -1 search is decreasing
   */
  public static int gsSearchDirection(Vect3 s, Vect3 vo, Vect3 vi, int epsh) {
    double mings = 0; // Units.from("kn",150);
    double maxgs = Double.MAX_VALUE; // Units.from("kn",700);
    return losr_gs_iter_dir(s.vect2(),vo.vect2(),vi.vect2(),mings, maxgs, Units.from("kn",1), epsh);
  }


  /** Return an absolute repulsive vertical speed search direction (or none)
   *  Assumes Velocity vo is non-zero.
   * @return +1 search is increasing, -1 search is decreasing
   */
  public static int vsSearchDirection(int epsv) {
    return epsv;
  }

  /** Return the horizontal epsilon that corresponds to the direction the traffic aircraft is currently turning indicated by sign of trackRate
   * 
   * @param s             relative position
   * @param vo            velocity of ownship
   * @param vi            velocity of intruder
   * @param epsh          horizontal epsilon
   * @param trackRate     trackRate of traffic
   * @return
   */
  public static int dataVsRateEpsilon(Vect3 s, Velocity vo, Velocity vi, int epsv, double vsRate){
    int trafSrchDir = vsSearchDirection(epsv);
    int absDir = -1;
    if (vsRate >= 0) absDir = 1;
    if (absDir == trafSrchDir) return epsv;
    else return -epsv;
  }

  /** Return the horizontal epsilon that corresponds to the direction the traffic aircraft is currently turning indicated by sign of trackRate
   * 
   * @param s             relative position
   * @param vo            velocity of ownship
   * @param vi            velocity of intruder
   * @param epsh          horizontal epsilon
   * @param trackRate     trackRate of traffic
   * @return
   */
  public static int dataTurnEpsilon(Vect3 s, Velocity vo, Velocity vi, int epsh, double trackRate){
    int trafSrchDir = trkSearchDirection(s.Neg(), vi, vo, epsh);
    int absDir = -1;
    if (trackRate >= 0) absDir = 1;
    if (absDir == trafSrchDir) return epsh;
    else return -epsh;
  }

}

