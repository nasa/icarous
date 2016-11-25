/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "KinematicIntegerBands.h"
#include "CriteriaCore.h"
#include "TCASTable.h"
#include "Util.h"
#include <vector>
#include <string>

namespace larcfm {

int KinematicIntegerBands::first_los_step(Detection3D* det, double tstep,bool trajdir,
    int min, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic) const {
  for (int k=min; k<=max; ++k) {
    if (any_los_aircraft(det,trajdir,k*tstep,ownship,traffic)) {
      return k;
    }
  }
  return -1;
}

int KinematicIntegerBands::first_los_search_index(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2, bool trajdir, int max,
    const TrafficState& ownship, const std::vector<TrafficState>& traffic) const {
  int FirstLosK = (int)std::ceil(B/tstep); // first k such that k*ts>=B
  int FirstLosN = std::min((int)std::floor(T/tstep),max); // last k<=MaxN such that k*ts<=T
  int FirstLosK2 = (int)std::ceil(B2/tstep);
  int FirstLosN2 = std::min((int)std::floor(T2/tstep),max);
  int FirstLosInit = recovery_det != NULL ? first_los_step(recovery_det,tstep,trajdir,FirstLosK2,FirstLosN2,ownship,traffic) : -1;
  int FirstLos = first_los_step(conflict_det,tstep,trajdir,FirstLosK,FirstLosN,ownship,traffic);
  int LosInitIndex = FirstLosInit < 0 ? max+1 : FirstLosInit;
  int LosIndex = FirstLos < 0 ? max+1 : FirstLos;
  return std::min(LosInitIndex,LosIndex);
}

int KinematicIntegerBands::bands_search_index(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
    int epsh, int epsv) const {
  bool usehcrit = repac.isValid() && epsh != 0;
  bool usevcrit = repac.isValid() && epsv != 0;
  int FirstLos = first_los_search_index(conflict_det,recovery_det,tstep,B,T,B2,T2,trajdir,max,ownship,traffic);
  int FirstNonHRep = !usehcrit || FirstLos == 0 ? FirstLos :
      first_nonrepulsive_step(tstep,trajdir,FirstLos-1,ownship,repac,epsh);
  int FirstProbHcrit = FirstNonHRep < 0 ? max+1 : FirstNonHRep;
  int FirstProbHL = std::min(FirstLos,FirstProbHcrit);
  int FirstNonVRep = !usevcrit || FirstProbHL == 0 ? FirstProbHL :
      first_nonvert_repul_step(tstep,trajdir,FirstProbHL-1,ownship,repac,epsv);
  int FirstProbVcrit = FirstNonVRep < 0 ? max+1 : FirstNonVRep;
  return std::min(FirstProbHL,FirstProbVcrit);
}

bool KinematicIntegerBands::no_conflict(Detection3D* conflict_det, Detection3D* recovery_det, double B, double T, double B2, double T2,
    bool trajdir, double tsk, const TrafficState& ownship, const std::vector<TrafficState>& traffic) const {
  return
      !any_conflict_aircraft(conflict_det,B,T,trajdir,tsk,ownship,traffic) &&
      !(recovery_det != NULL && any_conflict_aircraft(recovery_det,B2,T2,trajdir,tsk,ownship,traffic));
}

void KinematicIntegerBands::traj_conflict_only_bands(std::vector<Integerval>& l,
    Detection3D* conflict_det, Detection3D* recovery_det, double tstep, double B, double T, double B2, double T2,
    bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic) const {
  int d = -1; // Set to the first index with no conflict
  for (int k = 0; k <= max; ++k) {
    double tsk = tstep*k;
    if (d >=0 && no_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,tsk,ownship,traffic)) {
      continue;
    } else if (d >=0) {
      l.push_back(Integerval(d,k-1));
      d = -1;
    } else if (no_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,tsk,ownship,traffic)) {
      d = k;
    }
  }
  if (d >= 0) {
    l.push_back(Integerval(d,max));
  }
}

void KinematicIntegerBands::kinematic_bands(std::vector<Integerval>& l, Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
    int epsh, int epsv) const {
  l.clear();
  int bsi = bands_search_index(conflict_det,recovery_det,tstep,B,T,B2,T2,trajdir,max,ownship,traffic,repac,epsh,epsv);
  if  (bsi != 0) {
    traj_conflict_only_bands(l,conflict_det,recovery_det,tstep,B,T,B2,T2,trajdir,bsi-1,ownship,traffic);
  }
}

void KinematicIntegerBands::append_intband(std::vector<Integerval>& l, std::vector<Integerval>& r) {
  // Append in place
  int last = l.size()-1;
  if (!l.empty() && !r.empty() && r[0].lb-l[last].ub <= 1) {
    l[last].ub = r[0].ub;
    r.erase(r.begin());
  }
  l.insert(l.end(),r.begin(),r.end());
}

void KinematicIntegerBands::neg(std::vector<Integerval>& l) {
  // Negate, flip, and reverse in place
  if (l.empty()) return;
  int mid = (l.size()-1)/2;
  bool odd = l.size() % 2 != 0;
  for (int i=0; i <= mid; ++i) {
    if (i == mid && odd) {
      int x = l[i].lb;
      l[i].lb = -l[i].ub;
      l[i].ub = -x;
    } else {
      int i2 = l.size()-i-1;
      int x = l[i].lb;
      l[i].lb = -l[i2].ub;
      l[i2].ub = -x;
      x = l[i].ub;
      l[i].ub = -l[i2].lb;
      l[i2].lb = -x;
    }
  }
}

// INTERFACE FUNCTION
void KinematicIntegerBands::kinematic_bands_combine(std::vector<Integerval>& l,
    Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
    int epsh, int epsv) const {
  kinematic_bands(l,conflict_det,recovery_det,tstep,B,T,B2,T2,false,maxl,ownship,traffic,repac,epsh,epsv);
  std::vector<Integerval> r = std::vector<Integerval>();
  kinematic_bands(r,conflict_det,recovery_det,tstep,B,T,B2,T2,true,maxr,ownship,traffic,repac,epsh,epsv);
  neg(l);
  append_intband(l,r);
}

bool KinematicIntegerBands::any_los_aircraft(Detection3D* det, bool trajdir, double tsk,
    const TrafficState& ownship, const std::vector<TrafficState>& traffic) const {
  for (TrafficState::nat i=0; i < traffic.size(); ++i) {
    TrafficState ac = traffic[i];
    std::pair<Vect3,Velocity> sovot = trajectory(ownship,tsk,trajdir);
    Vect3 sot = sovot.first;
    Velocity vot = sovot.second;
    Vect3 si = ac.get_s();
    Velocity vi = ac.get_v();
    Vect3 sit = vi.ScalAdd(tsk,si);
    if (det->violation(sot, vot, sit, vi))
      return true;
  }
  return false;
}

// INTERFACE FUNCTION

// trajdir: false is left
int KinematicIntegerBands::first_green(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
    int epsh, int epsv) const {
  bool usehcrit = repac.isValid() && epsh != 0;
  bool usevcrit = repac.isValid() && epsv != 0;
  for (int k=0; k <= max; ++k) {
    double tsk = tstep*k;
    if ((tsk >= B && tsk <= T && any_los_aircraft(conflict_det,trajdir,tsk,ownship,traffic)) ||
        (recovery_det != NULL && tsk >= B2 && tsk <= T2 &&
            any_los_aircraft(recovery_det,trajdir,tsk,ownship,traffic)) ||
            (usehcrit && !repulsive_at(tstep,trajdir,k,ownship,repac,epsh)) ||
            (usevcrit && !vert_repul_at(tstep,trajdir,k,ownship,repac,epsv))) {
      return -1;
    } else if (!any_conflict_aircraft(conflict_det,B,T,trajdir,tsk,ownship,traffic) &&
        !(recovery_det != NULL &&
            any_conflict_aircraft(recovery_det,B2,T2,trajdir,tsk,ownship,traffic)))
      return k;
  }
  return -1;
}

bool KinematicIntegerBands::all_int_red(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
    int epsh, int epsv, int dir) const {
  bool leftans = dir > 0 || first_green(conflict_det,recovery_det,tstep,B,T,B2,T2,false,maxl,ownship,traffic,repac,epsh,epsv) < 0;
  bool rightans = dir < 0 || first_green(conflict_det,recovery_det,tstep,B,T,B2,T2,true,maxr,ownship,traffic,repac,epsh,epsv) < 0;
  return leftans && rightans;
}

int KinematicIntegerBands::first_instantaneous_green(Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    const TrafficState& repac,
    int epsh, int epsv) {
  for (int k = 0; k <= max; ++k) {
    j_step_ = k;
    if (no_instantaneous_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,ownship,traffic,repac,epsh,epsv)) {
      return k;
    }
  }
  return -1;
}

bool KinematicIntegerBands::all_instantaneous_red(Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    const TrafficState& repac,
    int epsh, int epsv, int dir) {
  bool leftans = dir > 0 || first_instantaneous_green(conflict_det,recovery_det,B,T,B2,T2,false,maxl,ownship,traffic,repac,epsh,epsv) < 0;
  bool rightans = dir < 0 || first_instantaneous_green(conflict_det,recovery_det,B,T,B2,T2,true,maxr,ownship,traffic,repac,epsh,epsv) < 0;
  return leftans && rightans;
}

Vect3 KinematicIntegerBands::linvel(const TrafficState& ownship, double tstep, bool trajdir, int k) const {
  Vect3 s1 = trajectory(ownship,(k+1)*tstep,trajdir).first;
  Vect3 s0 = trajectory(ownship,k*tstep,trajdir).first;
  return s1.Sub(s0).Scal(1/tstep);
}

bool KinematicIntegerBands::repulsive_at(double tstep, bool trajdir, int k, const TrafficState& ownship, const TrafficState& repac, int epsh) const {
  // repac is not NULL at this point and k >= 0
  if (k==0) {
    return true;
  }
  std::pair<Vect3,Velocity> sovo = trajectory(ownship,0,trajdir);
  Vect3 so = sovo.first;
  Vect3 vo = sovo.second;
  Vect3 si = repac.get_s();
  Vect3 vi = repac.get_v();
  bool rep = true;
  if (k==1) {
    rep = CriteriaCore::horizontal_new_repulsive_criterion(so.Sub(si),vo,vi,linvel(ownship,tstep,trajdir,0),epsh);
  }
  if (rep) {
    std::pair<Vect3,Velocity> sovot = trajectory(ownship,k*tstep,trajdir);
    Vect3 sot = sovot.first;
    Vect3 vot = sovot.second;
    Vect3 sit = vi.ScalAdd(k*tstep,si);
    Vect3 st = sot.Sub(sit);
    Vect3 vop = linvel(ownship,tstep,trajdir,k-1);
    Vect3 vok = linvel(ownship,tstep,trajdir,k);
    return CriteriaCore::horizontal_new_repulsive_criterion(st,vop,vi,vot,epsh) &&
        CriteriaCore::horizontal_new_repulsive_criterion(st,vot,vi,vok,epsh) &&
        CriteriaCore::horizontal_new_repulsive_criterion(st,vop,vi,vok,epsh);
  }
  return false;
}

int KinematicIntegerBands::first_nonrepulsive_step(double tstep, bool trajdir, int max,
    const TrafficState& ownship, const TrafficState& repac, int epsh) const {
  for (int k=0; k <= max; ++k) {
    if (!repulsive_at(tstep,trajdir,k,ownship,repac,epsh)) {
      return k;
    }
  }
  return -1;
}

bool KinematicIntegerBands::vert_repul_at(double tstep, bool trajdir, int k, const TrafficState& ownship,
    const TrafficState& repac, int epsv) const {
  // repac is not NULL at this point and k >= 0
  if (k==0) {
    return true;
  }
  std::pair<Vect3,Velocity> sovo = trajectory(ownship,0,trajdir);
  Vect3 so = sovo.first;
  Vect3 vo = sovo.second;
  Vect3 si = repac.get_s();
  Vect3 vi = repac.get_v();
  bool rep = true;
  if (k==1) {
    rep = CriteriaCore::vertical_new_repulsive_criterion(so.Sub(si),vo,vi,linvel(ownship,tstep,trajdir,0),epsv);
  }
  if (rep) {
    std::pair<Vect3,Velocity> sovot = trajectory(ownship,k*tstep,trajdir);
    Vect3 sot = sovot.first;
    Vect3 vot = sovot.second;
    Vect3 sit = vi.ScalAdd(k*tstep,si);
    Vect3 st = sot.Sub(sit);
    Vect3 vop = linvel(ownship,tstep,trajdir,k-1);
    Vect3 vok = linvel(ownship,tstep,trajdir,k);
    return CriteriaCore::vertical_new_repulsive_criterion(st,vop,vi,vot,epsv) &&
        CriteriaCore::vertical_new_repulsive_criterion(st,vot,vi,vok,epsv) &&
        CriteriaCore::vertical_new_repulsive_criterion(st,vop,vi,vok,epsv);
  }
  return false;
}

int KinematicIntegerBands::first_nonvert_repul_step(double tstep, bool trajdir, int max,
    const TrafficState& ownship, const TrafficState& repac, int epsv) const {
  for (int k=0; k <= max; ++k) {
    if (!vert_repul_at(tstep,trajdir,k,ownship,repac,epsv)) {
      return k;
    }
  }
  return -1;
}

bool KinematicIntegerBands::conflict(Detection3D* det, const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
    double B, double T) {
  if (Util::almost_equals(B,T)) {
    Vect3 sot = vo.ScalAdd(B,so);
    Vect3 sit = vi.ScalAdd(B,si);
    return det->violation(sot,vo,sit,vi);
  }
  return det->conflict(so,vo,si,vi,B,T);
}

bool KinematicIntegerBands::cd_future_traj(Detection3D* det, double B, double T, bool trajdir, double t,
    const TrafficState& ownship, const TrafficState& ac) const {
  if (t > T || B > T) return false;
  std::pair<Vect3,Velocity> sovot = trajectory(ownship,t,trajdir);
  Vect3 sot = sovot.first;
  Velocity vot = sovot.second;
  Vect3 si = ac.get_s();
  Velocity vi = ac.get_v();
  Vect3 sit = vi.ScalAdd(t,si);
  if (B > t) {
    return conflict(det, sot, vot, sit, vi, B-t, T-t);
  }
  return conflict(det, sot, vot, sit, vi, 0, T-t);
}

bool KinematicIntegerBands::any_conflict_aircraft(Detection3D* det, double B, double T, bool trajdir, double tsk,
    const TrafficState& ownship, const std::vector<TrafficState>& traffic) const {
  for (TrafficState::nat i=0; i < traffic.size(); ++i) {
    TrafficState ac = traffic[i];
    if (cd_future_traj(det, B, T, trajdir, tsk, ownship, ac))
      return true;
  }
  return false;
}

bool KinematicIntegerBands::any_conflict_step(Detection3D* det, double tstep, double B, double T, bool trajdir, int max,
    const TrafficState& ownship, const std::vector<TrafficState>& traffic) const {
  for (int k=0; k <= max; ++k) {
    if (any_conflict_aircraft(det,B,T,trajdir,tstep*k,ownship,traffic)) {
      return true;
    }
  }
  return false;
}

// trajdir: false is left
bool KinematicIntegerBands::red_band_exist(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
    int epsh, int epsv) const {
  bool usehcrit = repac.isValid() && epsh != 0;
  bool usevcrit = repac.isValid() && epsv != 0;
  return (usehcrit && first_nonrepulsive_step(tstep,trajdir,max,ownship,repac,epsh) >= 0) ||
      (usevcrit && first_nonvert_repul_step(tstep,trajdir,max,ownship,repac,epsv) >= 0) ||
      any_conflict_step(conflict_det,tstep,B,T,trajdir,max,ownship,traffic) ||
      (recovery_det != NULL && any_conflict_step(recovery_det,tstep,B2,T2,trajdir,max,ownship,traffic));
}

bool KinematicIntegerBands::instantaneous_red_band_exist(Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    const TrafficState& repac,
    int epsh, int epsv) {
  for (int k = 0; k <= max; ++k) {
    j_step_ = k;
    if (!no_instantaneous_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,ownship,traffic,repac,epsh,epsv)) {
      return true;
    }
  }
  return false;
}

// INTERFACE FUNCTION
bool KinematicIntegerBands::any_int_red(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
    int epsh, int epsv, int dir) const {
  bool leftred = dir <= 0 && red_band_exist(conflict_det,recovery_det,tstep,B,T,B2,T2,false,maxl,ownship,traffic,repac,epsh,epsv);
  bool rightred = dir >= 0 && red_band_exist(conflict_det,recovery_det,tstep,B,T,B2,T2,true,maxr,ownship,traffic,repac,epsh,epsv);
  return leftred || rightred;
}

bool KinematicIntegerBands::any_instantaneous_red(Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    const TrafficState& repac,
    int epsh, int epsv, int dir) {
  bool leftred = dir <= 0 && instantaneous_red_band_exist(conflict_det,recovery_det,B,T,B2,T2,false,maxl,ownship,traffic,repac,epsh,epsv);
  bool rightred = dir >= 0 && instantaneous_red_band_exist(conflict_det,recovery_det,B,T,B2,T2,true,maxr,ownship,traffic,repac,epsh,epsv);
  return leftred || rightred;
}

void KinematicIntegerBands::instantaneous_bands_combine(std::vector<Integerval>& l, Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    const TrafficState& repac,
    int epsh, int epsv) {
  instantaneous_bands(l,conflict_det,recovery_det,B,T,B2,T2,false,maxl,ownship,traffic,repac,epsh,epsv);
  std::vector<Integerval> r = std::vector<Integerval>();
  instantaneous_bands(r,conflict_det,recovery_det,B,T,B2,T2,true,maxr,ownship,traffic,repac,epsh,epsv);
  neg(l);
  append_intband(l,r);
}

bool KinematicIntegerBands::no_instantaneous_conflict(Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    bool trajdir, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    const TrafficState& repac,
    int epsh, int epsv) {
  bool usehcrit = repac.isValid() && epsh != 0;
  bool usevcrit = repac.isValid() && epsv != 0;
  std::pair<Vect3,Velocity> nsovo = trajectory(ownship,0,trajdir);
  Vect3 so = ownship.get_s();
  Vect3 vo = ownship.get_v();
  Vect3 si = repac.get_s();
  Vect3 vi = repac.get_v();
  Vect3 nvo = nsovo.second;
  Vect3 s = so.Sub(si);
  return
      (!usehcrit || CriteriaCore::horizontal_new_repulsive_criterion(s,vo,vi,nvo,epsh)) &&
      (!usevcrit || CriteriaCore::vertical_new_repulsive_criterion(s,vo,vi,nvo,epsv)) &&
      no_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,0,ownship,traffic);
}

void KinematicIntegerBands::instantaneous_bands(std::vector<Integerval>& l, Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    const TrafficState& repac,
    int epsh, int epsv) {
  int d = -1; // Set to the first index with no conflict
  for (int k = 0; k <= max; ++k) {
    j_step_ = k;
    if (d >=0 && no_instantaneous_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,ownship,traffic,repac,epsh,epsv)) {
      continue;
    } else if (d >=0) {
      l.push_back(Integerval(d,k-1));
      d = -1;
    } else if (no_instantaneous_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,ownship,traffic,repac,epsh,epsv)) {
      d = k;
    }
  }
  if (d >= 0) {
    l.push_back(Integerval(d,max));
  }
}

}
