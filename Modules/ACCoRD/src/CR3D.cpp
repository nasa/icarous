/*
 * CR3D.cpp
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * CR3D is an algorithm for 3-D conflict *resolution* that provides
 * avoidance maneuvers for the ownship
 *
 * Unit Convention
 * ---------------
 * All units in this file are *internal*:
 * - Units of distance are denoted [d]
 * - Units of time are denoted     [t]
 * - Units of speed are denoted    [d/t]
 * - Units of turn are radians     [rad]
 *
 * REMARK: X points to East, Y points to North. Angles are in
 *         True North/clockwise convention.
 *
 * Naming Convention
 * -----------------
 * The intruder is supposed to be fixed at the origin of the coordinate
 * system.
 * D  : Diameter of the protected zone [d]
 * H  : Height of the protected zone [d]
 * T  : Lookahead time [t]
 * s  : Relative position of the ownship [d,d,d]
 * vo : Ownship velocity vector  [d/t,d/t,d/t]
 * vi : Intruder velocity vector [d/t,d/t,d/t]
 *
 * Output class variables
 * ----------------------
 * trk [d/t,d/t] (track resolution)
 * gs  [d/t,d/t] (ground speed resolution)
 * opt [d/t,d/t] (optimal horizontal resolution)
 * vs  [d/t]     (vertical speed resolution)
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CR3D.h"
#include "format.h"
//#include "Repulsion.h"
#include "CriteriaCore.h"
#include "Kinematics.h"
#include "VectFuns.h"
#include "LossData.h"

namespace larcfm {


CR3D::CR3D() {
  trk = gs = opt = Horizontal::NoHorizontalSolution();
  vs = Vertical::NoVerticalSolution();
}
bool CR3D::cr(const Vect3& s, const Vect3& vo, const Vect3& vi,
    const double D, const double H, int epsh, int epsv) {
  Vect2 s2 = s.vect2();
  //Vect3 v  = vo-vi;
  //Vect2 v2 = v.vect2();
  //    Vect3 ns = -s;

  trk = gs = opt = Horizontal::NoHorizontalSolution();
  vs = Vertical::NoVerticalSolution();

  //    int epsh = horizontalCoordination(s2,v2);
  //    int epsv = verticalCoordination(s,v,D,ownship,traffic);
  TangentLine nv = TangentLine(s2,D,epsh);
  //    TangentLine nv_i = TangentLine(nv);
  //    nv_i.neg();

  // Find track only solution
  trk = Horizontal::trk_only(nv,s,vo,vi,epsv,D,H);
  //    Horizontal trk_i = trk_only(nv_i,ns,vi,vo,-epsv,D,H);
  //    if (trk.undef() || trk_i.undef()) {
  //      TangentLine nvn = TangentLine(s2,D,-epsh);
  //      TangentLine nvn_i = TangentLine(nvn);
  //      nvn_i.neg();
  //      Horizontal trk_o = trk_only(nvn,s,vo,vi,epsv,D,H);
  //      trk_i = trk_only(nvn_i,ns,vi,vo,-epsv,D,H);
  //      if (!trk_o.undef() && !trk_i.undef()) {
  //   	trk = trk_o;
  //  	nv = nvn;
  //      }
  //}

  // Find ground speed only solution
  gs = Horizontal::gs_only(nv,s,vo,vi,epsv,D,H);

  // Find optimal horizontal only solution
  opt = Horizontal::opt_trk_gs(nv,s,vo,vi,epsv,D,H);

  // Find vertical only solution
  vs = Vertical::vs_circle(s,vo,vi,epsv,D,H);

  return !trk.undef() || !gs.undef() || !opt.undef() || !vs.undef();
}

bool CR3D::losr_repulsive(const Vect3& s, const Velocity& vo, const Velocity& vi, double NMAC_D, double NMAC_H,
    double minHorizExitSpeedLoS, double minVertExitSpeedLoS,
    double minGs, double maxGs, double maxVs, int epsh, int epsv) {
  Vect2 s2  = s.vect2();
  trk = gs = opt = Horizontal::NoHorizontalSolution();
  vs = Vertical::NoVerticalSolution();
  //fpln(" ## CR3D::losr_repulsive:  maxVs = "+Units::str("fpm",maxVs));
  if (!s2.isZero()) {
    trk = losr_trk_iter(s,vo,vi,minHorizExitSpeedLoS, M_PI/2, Units::from("deg",1.0),epsh);
    //f.pln(" @@@@ Horizontal: trk = "+f.vStr2(trk));
    //gs = Horizontal::repulsiveGs(s,vo,vi,minHorizExitSpeedLoS,minGs,maxGs);
    //fpln("cr3d 143: "+Fm12(maxGs));
    gs = losr_gs_iter(s,vo,vi,minHorizExitSpeedLoS, minGs, maxGs , Units::from("kn",10.0),epsh);
  }
  vs = losr_vs_new(s,vo, vi, minVertExitSpeedLoS, maxVs, NMAC_D, NMAC_H, epsv);
  return !trk.undef() || !gs.undef() || !opt.undef() || !vs.undef();
}

int CR3D::cr3d_repulsive(const Vect3& s, const Velocity& vo, const Velocity& vi,
    const double D, const double H,
    const double minHorizExitSpeedLoS, double minVertExitSpeedLoS,
    const double minGs, const double maxGs,double maxVs, int epsh, int epsv) {

  int resolution = larcfm::None;
  //fpln(" ## CR3D::cr3d_old:  maxVs = "+Units::str("fpm",maxVs));
  trk = gs = opt = Horizontal::NoHorizontalSolution();
  vs = Vertical::NoVerticalSolution();

  //Vect2 s2 = s.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();

  if (vo2.isZero() || vi2.isZero())
    return resolution;
  CDCylinder cd = CDCylinder();
  LossData ld = cd.detection(s,vo,vi,D,H);
  if (ld.conflict()) {
    if (Util::almost_equals(ld.getTimeIn(),0)) {
      resolution = cr3d_repulsive_los(s,vo,vi,ACCoRDConfig::NMAC_D, ACCoRDConfig::NMAC_H,minHorizExitSpeedLoS,minVertExitSpeedLoS,minGs,maxGs,maxVs,epsh,epsv);
    } else {
      if (cr(s,vo,vi,D,H,epsh,epsv))
        resolution = larcfm::Conflict;
    }
  } else {
    resolution = larcfm::Unnecessary;
    trk = gs = opt = Horizontal(vo2);
    vs = Vertical(vo.z);
  }
  return resolution;
}

int CR3D::cr3d_repulsive_los(Vect3 s, Velocity vo, Velocity vi, double NMAC_D, double NMAC_H,
    double minHorizExitSpeedLoS, double minVertExitSpeedLoS, double minGs, double maxGs, double maxVs,
    int epsh, int epsv) {
  int resolution = larcfm::None;
  if (losr_repulsive(s,vo,vi,NMAC_D, NMAC_H,minHorizExitSpeedLoS,minVertExitSpeedLoS,minGs,maxGs,maxVs,epsh,epsv))  {
    bool divergent = s.dot(vo.Sub(vi)) > 0;
    if (divergent) resolution = larcfm::LoSDivg;
    else resolution = larcfm::LoSConv;
  }
  return resolution;
}

// Compute a new track only vector that is one step to the dir of nvo
Vect2 CR3D::incr_vect(const Vect2& nvo, double step, int dir) {
  return nvo.Scal(cos(dir*step)).Add(nvo.PerpR().Scal(sin(dir*step)));
}

/**
 * Iteratively compute an ABSOLUTE track-only loss of separation recover maneuver for the ownship
 *    @param s        relative position of the ownship
 *    @param vo       velocity of the ownship aircraft
 *    @param vi       velocity of the traffic aircraft
 *    @param minrelgs minimum relative ground speed
 *    @param maxtrk   maximum track [rad]
 *    @param step     step [rad]
 *
 */
Horizontal CR3D::losr_trk_iter(const Vect3& s, const Vect3& vo, const Vect3& vi, double minrelgs, double maxtrk, double step, int epsh) {
  Vect2 s2 = s.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();
  //Vect2 v2 = vo2.Sub(vi2);
  //int epsh = horizontalCoordination(s2,v2);
  //int dir = CR3D::trk_los_iter_dir(s2,vo2,vi2,step,epsh);
  int dir = CriteriaCore::losr_trk_iter_dir(s2,vo2,vi2,step,epsh); // ,step,epsh);
  if (dir == 0)
    return Horizontal::NoHorizontalSolution();
  else {
    Vect2 nvo = CR3D::incr_vect(vo2,step,dir);
    if (CriteriaCore::horizontal_los_criterion(s2,vo2,vi2,nvo,epsh)) {
      for (int i = 1;;i++) {
        Vect2 nvop = CR3D::incr_vect(nvo,step,dir);
        if (i*step >= maxtrk || !CriteriaCore::horizontal_los_criterion(s2,nvo,vi2,nvop,epsh))
          return Horizontal(nvo);
        else if (VectFuns::divergentHorizGt(s2,nvop,vi2,minrelgs))
          return Horizontal(nvop);
        nvo = nvop;
      }
    } else {
      return Horizontal::NoHorizontalSolution();
    }
  }
}


Horizontal losr_gs_iter_aux(const Vect3& s, const Vect3& vo, const Vect3& vi,
    double minrelgs, double mings, double maxgs, double step, int epsh) {
  Vect2 s2 = s.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();
  int dir = CriteriaCore::losr_gs_iter_dir(s2,vo2,vi2,mings,maxgs,step,epsh);
  if (dir == 0)
    return Horizontal::NoHorizontalSolution();
  else {
    Vect2 nvo = CriteriaCore::incr_gs_vect(vo2,step,dir);
    if (CriteriaCore::horizontal_los_criterion(s2,vo2,vi2,nvo,epsh)) {
      for (int i = 1;;i++) {
        Vect2 nvop = CriteriaCore::incr_gs_vect(nvo,step,dir);
        double nnorm = nvo.norm()+dir*step;
        bool repCrit2D = CriteriaCore::horizontal_los_criterion(s2,nvo,vi2,nvop,epsh);
        if (nnorm > maxgs || nnorm < mings || !repCrit2D) {
          //f.pln("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ losr_gs_iter: EXIT 0");
          return Horizontal(nvo);
        }
        if (VectFuns::divergentHorizGt(s2,nvop,vi2,minrelgs)) {
          //f.pln("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ losr_gs_iter: EXIT 0");
          return Horizontal(nvop);
        }
        nvo = nvop;
      }
    } else {
      return Horizontal::NoHorizontalSolution();
    }
  }
}

/**
 * Iteratively compute an ABSOLUTE ground speed-only loss of separation recovery maneuver for the ownship
 *    @param s        relative position of the ownship
 *    @param vo       velocity of the ownship aircraft
 *    @param vi       velocity of the traffic aircraft
 *    @param minrelgs minimum relative ground speed
 *    @param mings    minimum ground speed
 *    @param maxgs    maximum ground speed
 *    @param step     ground speed step (postive)
 *    @param epsh     horizontal epsilon
 *
 */
Horizontal CR3D::losr_gs_iter(const Vect3& s, const Vect3& vo, const Vect3& vi,
    double minrelgs, double min_gs, double max_gs, double step, int epsh) {

  const double gs_los_factor = 2.0;
  double minGs = Util::max(vo.norm()/gs_los_factor,min_gs);
  //fpln("cr3d 323: "+Fm16(max_gs));
  double maxGs = Util::min(gs_los_factor*vo.norm(),max_gs);
  //fpln("cr3d 325: "+Fm16(maxGs));
  Horizontal nvo = losr_gs_iter_aux(s,vo,vi,minrelgs,minGs,maxGs,step,epsh);
  if (!nvo.undef())  {
    Vect3 nvo3 = Vect3(nvo,vo.z);
    double tau = VectFuns::tau(s,nvo3,vi);
    double distAtTau = VectFuns::distAtTau(s,nvo3,vi,true);
    if (tau <= 0 ||  distAtTau > ACCoRDConfig::gsSearchLosDiscard)  {    // $$$$$$$$$$$$$$$ PARAMETER $$$$$$$$$$$$$$$$$$$$
      //f.pln("@@@@ losr_gs_iter: Gs Resolution PASSED FINAL TEST rtn = "+f.vStr2(nvo)+" distAtTau = "+Units.str("nm",distAtTau));
      return nvo;
    } else {
      //f.pln("@@@@ losr_gs_iter: Gs Resolution FAILED FINAL TEST ***** tau = "+tau+" distAtTau = "+Units.str("nm",distAtTau));
      return Horizontal::NoHorizontalSolution();
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
 *    @param minrelv  minimum relative vertical speed
 *    @param caD      diameter of collision avoidance area, e.g., 1000 ft
 *    @param caH      height of collision avoidance area, e.g., 200ft
 *    @param minrelvs minimum relative vertical speed
 *
 */
Vertical CR3D::losr_vs_new(const Vect3& s, const Velocity& vo, const Velocity& vi, double minrelvs, double maxvs,
    double caD, double caH, int epsv) {
  //   int eps = losr_vs_dir(s,vo,vi,caD,caH,ownship,traffic);
  Vect3 v = vo.Sub(vi);
  double nvz;
  if (epsv*v.z <= 0)
    nvz = epsv*minrelvs;
  else
    nvz = epsv*Util::max(minrelvs,std::abs(v.z));
  double voz = nvz+vi.z;
  voz = Util::sign(voz)*Util::min(std::abs(voz),maxvs);
  double algInnerFactor = 2.0;
  //fpln(" losr_vs_new: voz = "+Units::str("fpm",voz)+" maxvs = "+Units::str("fpm",maxvs)+" eps = "+Fmi(eps));
  if (CD3D::cd3d(s,vo,vi,caD,algInnerFactor*caH)) {
    if (CD3D::LoS(s,caD,caH))
      return Vertical(epsv*maxvs);
    Vertical vso = Vertical::vs_circle(s,vo,vi,epsv,caD,algInnerFactor*caH);
    if (vso.undef())
      return Vertical(epsv*maxvs);
    if (std::abs(vso.z) > maxvs)
      return Vertical(Util::sign(vso.z)*maxvs);
    if (std::abs(vso.z-vi.z) <= minrelvs)
      return Vertical(voz);
    return vso;
  }
  return Vertical(voz);
}

}
