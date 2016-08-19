/*
 * CR3D.java 
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
import gov.nasa.larcfm.Util.LossData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.VectFuns;
import gov.nasa.larcfm.Util.f;

/**
 * CR3D is an algorithm for 3-D conflict *resolution* that provides
 * avoidance maneuvers for the ownship<p>
 *
 * Unit Convention<br>
 *
 * All units in this file are *internal*:
 * <ul>
 * <li> Units of distance are denoted [d]
 * <li> Units of time are denoted     [t]
 * <li> Units of speed are denoted    [d/t]
 * <li> Units of turn are radians     [rad]
 * </ul>
 *
 * REMARK: X points to East, Y points to North. Angles are in
 *         True North/clockwise convention.
 *
 * Naming Convention<br>
 * The intruder is supposed to be fixed at the origin of the coordinate
 * system.
 *<ul>
 *<li> D  : Diameter of the protected zone [d]
 *<li> H  : Height of the protected zone [d]
 *<li> s  : Relative position of the ownship [d,d,d]
 *<li> vo : Ownship velocity vector  [d/t,d/t,d/t]
 *<li> vi : Intruder velocity vector [d/t,d/t,d/t]
 *</ul>
 *
 * Output class variables<br>
 * <ul>
 * <li>trk [d/t,d/t] (track resolution)
 * <li>gs  [d/t,d/t] (ground speed resolution)
 * <li>opt [d/t,d/t] (optimal horizontal resolution)
 * <li>vs  [d/t]     (vertical speed resolution)
 * </ul>
 *
 */
public class CR3D {

  /** Track only resolution */
  public Horizontal trk;  
  /** Ground speed only resolution */
  public Horizontal gs;  
  /** Optimal track/ground speed resolution */
  public Horizontal opt;  

  /** Vertical speed resolution */
  public Vertical vs;            

  /** Horizontal and Vertical speed only resolution*/
  public HorizontalAndVertical vel;


  /**
   * Instantiates a new CR3D object.
   */
  public CR3D() {
    trk = gs = opt = Horizontal.NoHorizontalSolution;
    vs = Vertical.NoVerticalSolution;
    vel = HorizontalAndVertical.NoHorizontalAndVerticalSolution;
  }

  /** Copy constructor */
  public CR3D(CR3D cr3d) {
    this.trk = cr3d.trk;
    this.gs = cr3d.gs;
    this.opt = cr3d.opt;
    this.vs = cr3d.vs;
    this.vel = cr3d.vel;
  }

  /**
   * Conflict resolution algorithm
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * If trk is not zero, trk satisfies
   *   || trk || = || vo ||, and
   * trk is a coordinated track only resolution for the ownship.
   *    
   * If gs is not zero, gs satisfies
   *   exists l > 0 such that l*gs = vo, and
   * gs is a coordinated ground speed only resolution for the ownship.
   *   
   * If opt is not zero, opt is a coordinated optimal horizontal resolution for the ownship.
   *
   * vs is defined ==>
   *   vs is a coordinated vertical only resolution for the ownship.
   * 
   * @return true, if at least one resolution was found.
   */
  public boolean cr(Vect3 s, Vect3 vo, Vect3 vi, double D, double H, int epsh, int epsv) {
    Vect2 s2 = s.vect2();
    //    Vect3 v  = vo.Sub(vi);
    //    Vect2 v2 = v.vect2();
    //    Vect3 ns = s.Neg();
    trk = gs = opt = Horizontal.NoHorizontalSolution;
    vs = Vertical.NoVerticalSolution;
    vel = HorizontalAndVertical.NoHorizontalAndVerticalSolution;
    //int epsh = horizontalCoordination(s2,v2);
    //int epsv = verticalCoordination(s,v,D,ownship,traffic);
    TangentLine nv = new TangentLine(s2,D,epsh);
    //    TangentLine nv_i = new TangentLine(nv);
    //    nv_i.neg();

    // Find track only solution
    trk = Horizontal.trk_only(nv,s,vo,vi,epsv,D,H);
    //    Horizontal trk_i = Horizontal.trk_only(nv_i,ns,vi,vo,-epsv,D,H);
    //    if (trk.undef() || trk_i.undef()) {
    //      TangentLine nvn = new TangentLine(s2,D,-epsh);
    //      TangentLine nvn_i = new TangentLine(nvn);
    //      nvn_i.neg();
    //      Horizontal trk_o = Horizontal.trk_only(nvn,s,vo,vi,epsv,D,H);
    //      trk_i = Horizontal.trk_only(nvn_i,ns,vi,vo,-epsv,D,H);
    //      if (!trk_o.undef() && !trk_i.undef()) {
    //        trk = trk_o;
    //        nv = nvn; 
    //      }
    //    }

    // Find ground speed only solution
    gs = Horizontal.gs_only(nv,s,vo,vi,epsv,D,H);

    // Find optimal horizontal only solution
    opt = Horizontal.opt_trk_gs(nv,s,vo,vi,epsv,D,H);

    // Speed only solution
    vel = HorizontalAndVertical.speed_only(nv, vo, vi.vect2());

    // Find vertical only solution
    vs = Vertical.vs_circle(s,vo,vi,epsv,D,H);
    //f.pln(">>>>>>>>>>>>>> CR3D.cr: vs.z = "+vs.z+" epsv = "+epsv);
    return !trk.undef() || !gs.undef() || !opt.undef() || !vs.undef() || (ACCoRDConfig.allowSpeedOnlyResolutions && !vel.undef());
  }

  /**
   * Loss of separation recovery algorithm
   * 
   * 
   * If trk is not zero, trk satisfies
   *   || trk || = || vo ||, and
   * trk is a coordinated track only LoS recovery maneuver for the ownship.
   *    
   * If gs is not zero, gs satisfies
   *   exists l > 0 such that l*gs = vo, and
   * gs is a coordinated ground speed only LoS recovery maneuver for the ownship.
   *   
   * No optimal solution
   *
   * vs is defined ==>
   *   vs is a coordinated vertical only LoS recovery maneuver for the ownship.
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param H the minimum vertical distance
   * 
   * @return true, if at least one recovery maneuver was found.
   */
  public boolean losr_repulsive(Vect3 s, Velocity vo, Velocity vi, double NMAC_D, double NMAC_H, 
      double minHorizExitSpeedLoS, double minVertExitSpeedLoS, 
      double minGs, double maxGs, double maxVs, int epsh, int epsv){
    Vect2 s2  = s.vect2();
    trk = gs = opt = Horizontal.NoHorizontalSolution;
    vs = Vertical.NoVerticalSolution;
    vel = HorizontalAndVertical.NoHorizontalAndVerticalSolution;
    if (!s2.isZero()) {
      trk = losr_trk_iter(s,vo,vi,minHorizExitSpeedLoS, Math.PI/2, Units.from("deg",1.0),epsh);
      gs = losr_gs_iter(s,vo,vi,minHorizExitSpeedLoS, minGs, maxGs , Units.from("kn",10.0),epsh);
    }
    vs = losr_vs_new(s,vo, vi, minVertExitSpeedLoS, maxVs, NMAC_D, NMAC_H, epsv);
    return !trk.undef() || !gs.undef() || !opt.undef() || !vs.undef(); 
  }


  /**
   * Conflict resolution algorithm including loss of separation recovery maneuvers 
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   *
   * @return the type of resolution maneuver:
   * <li> LoSDivg (3)-- Loss of separation resolution
   * <li> LoSConv (2) -- Loss of separation resolution
   * <li> None (-1)-- No resolution available
   * <li> Conflict (1) -- Conflict resolution
   * <li> Unnecessary (0) -- Conflict resolution is not needed
   */
  public int cr3d_repulsive(Vect3 s, Velocity vo, Velocity vi,double D, double H,
      double minHorizExitSpeedLoS, double minVertExitSpeedLoS, double minGs, double maxGs, double maxVs,
      int epsh, int epsv) {
    int resolution = None;
    trk = gs = opt = Horizontal.NoHorizontalSolution;
    vs = Vertical.NoVerticalSolution;
    vel = HorizontalAndVertical.NoHorizontalAndVerticalSolution;
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    if (vo2.isZero() || vi2.isZero())
      return resolution;
    CDCylinder cd = new CDCylinder();
    LossData ld = cd.detection(s,vo,vi,D,H); 
    if (ld.conflict()) {
      if (Util.almost_equals(ld.getTimeIn(),0)) {
        if (losr_repulsive(s,vo,vi,ACCoRDConfig.NMAC_D, ACCoRDConfig.NMAC_H,minHorizExitSpeedLoS,minVertExitSpeedLoS,minGs,maxGs,maxVs,epsh,epsv))  {
          boolean divergent = s.dot(vo.Sub(vi)) > 0;
          if (divergent) resolution = LoSDivg;
          else resolution = LoSConv;
        }
      } else {
        if (cr(s,vo,vi,D,H,epsh,epsv)) 
          resolution = Conflict;
      }
    } else {
      //f.pln(" $$$$$$$$$$$ cr3d_repulsive: Unnecessary");
      resolution = Unnecessary;
      trk = gs = opt = new Horizontal(vo2);
      vs = new Vertical(vo.z);
    }
    return resolution;
  }

  public int cr3d_repulsive_los(Vect3 s, Velocity vo, Velocity vi, double NMAC_D, double NMAC_H,
      double minHorizExitSpeedLoS, double minVertExitSpeedLoS, double minGs, double maxGs, double maxVs,
      int epsh, int epsv) {
    int resolution = None;
    if (losr_repulsive(s,vo,vi,NMAC_D, NMAC_H,minHorizExitSpeedLoS,minVertExitSpeedLoS,minGs,maxGs,maxVs,epsh,epsv))  {
      boolean divergent = s.dot(vo.Sub(vi)) > 0;
      if (divergent) resolution = LoSDivg;
      else resolution = LoSConv;
    }
    return resolution;
  }

  /**
   * Iteratively compute an ABSOLUTE track-only loss of separation recovery maneuver for the ownship
   *    @param s        relative position of the ownship
   *    @param vo       velocity of the ownship aircraft
   *    @param vi       velocity of the traffic aircraft
   *    @param minrelgs minimum relative ground speed
   *    @param maxtrk   maximum track [rad]
   *    @param step     track step [rad] (positive)
   *    @param epsh     horizontal epsilon
   *    
   */
  public static Horizontal losr_trk_iter(Vect3 s, Vect3 vo, Vect3 vi, 
      double minrelgs, double maxtrk, double step, int epsh) {
    Vect2 s2 = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    int dir = CriteriaCore.losr_trk_iter_dir(s2,vo2,vi2,step,epsh);  
    if (dir == 0) 
      return Horizontal.NoHorizontalSolution;
    else {
      Vect2 nvo = CriteriaCore.incr_trk_vect(vo2,step,dir);
      if (CriteriaCore.horizontal_repulsive_criterion(s2,vo2,vi2,nvo,epsh)) {
        for (int i = 1;;i++) {
          Vect2 nvop = CriteriaCore.incr_trk_vect(nvo,step,dir);
          boolean repCrit2D = CriteriaCore.horizontal_repulsive_criterion(s2,nvo,vi2,nvop,epsh);
          if (i*step >= maxtrk || !repCrit2D) {
            return new Horizontal(nvo);
          }
          if (VectFuns.divergentHorizGt(s2,nvop,vi2,minrelgs)) {
            return new Horizontal(nvop);
          } nvo = nvop; 
        }
      } else {
        return Horizontal.NoHorizontalSolution; 
      }
    }
  }  

  static Horizontal losr_gs_iter_aux(Vect3 s, Vect3 vo, Vect3 vi, 
      double minrelgs, double mings, double maxgs, double step, int epsh) {
    Vect2 s2 = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    int dir = CriteriaCore.losr_gs_iter_dir(s2,vo2,vi2,mings,maxgs,step,epsh);  
    if (dir == 0) 
      return Horizontal.NoHorizontalSolution;
    else {
      Vect2 nvo = CriteriaCore.incr_gs_vect(vo2,step,dir);
      if (CriteriaCore.horizontal_repulsive_criterion(s2,vo2,vi2,nvo,epsh)) {
        //        for (int i = 1;;i++) { // replace with while(true) to avoid warnings
        while (true) {
          Vect2 nvop = CriteriaCore.incr_gs_vect(nvo,step,dir);
          double nnorm = nvo.norm()+dir*step;
          boolean repCrit2D = CriteriaCore.horizontal_repulsive_criterion(s2,nvo,vi2,nvop,epsh);
          if (nnorm > maxgs || nnorm < mings || !repCrit2D) {
            //f.pln("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ losr_gs_iter: EXIT 0");
            return new Horizontal(nvo);
          }
          if (VectFuns.divergentHorizGt(s2,nvop,vi2,minrelgs)) {
            //f.pln("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ losr_gs_iter: EXIT 0");
            return new Horizontal(nvop);
          } 
          nvo = nvop; 
        }  
      } else {
        return Horizontal.NoHorizontalSolution; 
      }
    }
  }  

  /**
   * Iteratively compute an ABSOLUTE ground speed-only loss of separation recovery maneuver for the ownship
   *    @param s        relative position of the ownship
   *    @param vo       velocity of the ownship aircraft
   *    @param vi       velocity of the traffic aircraft
   *    @param minrelgs minimum relative ground speed
   *    @param min_gs    minimum ground speed 
   *    @param max_gs    maximum ground speed  
   *    @param step     ground speed step (postive)
   *    @param epsh     horizontal epsilon
   *    
   */
  public static Horizontal losr_gs_iter(Vect3 s, Vect3 vo, Vect3 vi, 
      double minrelgs, double min_gs, double max_gs, double step, int epsh) {

    final double gs_los_factor = 2.0;
    double minGs = Math.max(vo.norm()/gs_los_factor,min_gs);
    double maxGs = Math.min(gs_los_factor*vo.norm(),max_gs);

    Horizontal nvo = losr_gs_iter_aux(s,vo,vi,minrelgs,minGs,maxGs,step,epsh);
    if (!nvo.undef())  {  
      Vect3 nvo3 = new Vect3(nvo,vo.z);
      double tau = VectFuns.tau(s,nvo3,vi);
      double distAtTau = VectFuns.distAtTau(s,nvo3,vi,true);
      if (tau <= 0 ||  distAtTau > ACCoRDConfig.gsSearchLosDiscard)  {    // $$$$$$$$$$$$$$$ PARAMETER $$$$$$$$$$$$$$$$$$$$
        return nvo;
      } else {
        return Horizontal.NoHorizontalSolution;
      }
    } else {
      return nvo;
    }
  }

  /**
   * Compute an ABSOLUTE vertical-only loss of separation recovery maneuver for the ownship
   *    @param s        relative position of the ownship
   *    @param vo       velocity of the ownship aircraft
   *    @param vi       velocity of the traffic aircraft
   *    @param H        height of protected zone, e.g., 1000ft
   *    @param minrelvs minimum relative vertical speed
   *    @param maxvs    maximum absolute vertical speed
   *    @param caD      diameter of collision avoidance area, e.g., 1000 ft
   *    @param caH      height of collision avoidance area, e.g., 200ft
   *    @param epsv     The epsilon for vertical maneuvers (+1 or -1)
   *    
   */
  public static Vertical losr_vs_new(Vect3 s, Velocity vo, Velocity vi, double minrelvs, double maxvs, double caD, double caH, int epsv) {
    Vect3 v = vo.Sub(vi);
    double nvz;        
    if (epsv*v.z <= 0)
      nvz = epsv*minrelvs;
    else
      nvz = epsv*Math.max(minrelvs,Math.abs(v.z));
    double voz = nvz+vi.z;
    voz = Util.sign(voz)*Math.min(Math.abs(voz),maxvs);
    double algInnerFactor = 2.0;
    //f.pln(" losr_vs_new: voz = "+Units.str("fpm",voz)+" maxvs = "+Units.str("fpm",maxvs)+" eps = "+f.Fm0(eps));
    if (CD3D.cd3d(s,vo,vi,caD,algInnerFactor*caH)) {
      if (CD3D.LoS(s,caD,caH)) 
        return new Vertical(epsv*maxvs);
      Vertical vso = Vertical.vs_circle(s,vo,vi,epsv,caD,algInnerFactor*caH);
      if (vso.undef()) 
        return new Vertical(epsv*maxvs);
      if (Math.abs(vso.z) > maxvs)
        return new Vertical(Util.sign(vso.z)*maxvs);
      if (Math.abs(vso.z-vi.z) <= minrelvs)
        return new Vertical(voz);
      return vso;        
    }
    return new Vertical(voz);
  }

  public int cr3d_repulsive(Vect3 sub, Velocity vo, Velocity vi, double d,
      double h, double d2, double h2, double nMAC_D, double nMAC_H,
      double minHorizExitSpeedLoS, double minVertExitSpeedLoS, double minGs,
      double maxGs, double maxVs, int epsh, int epsv) {
    // TODO Auto-generated method stub
    return 0;
  }  

}
