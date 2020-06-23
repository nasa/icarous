/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusIntegerBands.h"
#include "CriteriaCore.h"
#include "TCASTable.h"
#include "Util.h"
#include <vector>
#include <string>

namespace larcfm {

bool DaidalusIntegerBands::LOS_at(const Detection3D* det, bool trajdir, double tsk,
   const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic) const {
  std::pair<Vect3,Velocity> sovot = trajectory(parameters,ownship,tsk,trajdir);
  Vect3 sot = sovot.first;
  Velocity vot = sovot.second;
  Vect3 sat = vot.ScalAdd(-tsk,sot);
  TrafficState own = ownship;
  own.setPosition(Position(sat));
  own.setAirVelocity(vot);
  return det->violationAtWithTrafficState(own,traffic,tsk);
}

// In PVS: int_bands@first_los_step

int DaidalusIntegerBands::first_los_step(const Detection3D* det, double tstep, bool trajdir,
    int min, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic) const {
  for (int k=min; k<=max; ++k) {
    if (LOS_at(det,trajdir,k*tstep,parameters,ownship,traffic)) {
      return k;
    }
  }
  return -1;
}

// In PVS: kinematic_bands@first_los_search_index
int DaidalusIntegerBands::first_los_search_index(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2, bool trajdir, int max,
   const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic) const {
  int FirstLosK = (int)std::ceil(B/tstep); // first k such that k*ts>=B
  int FirstLosN = Util::min((int)std::floor(T/tstep),max); // last k<=MaxN such that k*ts<=T
  int FirstLosK2 = (int)std::ceil(B2/tstep);
  int FirstLosN2 = Util::min((int)std::floor(T2/tstep),max);
  int FirstLosInit = recovery_det != NULL ? first_los_step(recovery_det,tstep,trajdir,FirstLosK2,FirstLosN2,parameters,ownship,traffic) : -1;
  int FirstLos = first_los_step(conflict_det,tstep,trajdir,FirstLosK,FirstLosN,parameters,ownship,traffic);
  int LosInitIndex = FirstLosInit < 0 ? max+1 : FirstLosInit;
  int LosIndex = FirstLos < 0 ? max+1 : FirstLos;
  return Util::min(LosInitIndex,LosIndex);
}

// In PVS: kinematic_bands@bands_search_index
// epsh == epsv == 0, if traffic is not the repulsive aircraft
int DaidalusIntegerBands::bands_search_index(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) const {
  bool usehcrit = epsh != 0;
  bool usevcrit = epsv != 0;
  int FirstLos = first_los_search_index(conflict_det,recovery_det,tstep,B,T,B2,T2,trajdir,max,parameters,ownship,traffic);
  int FirstNonHRep = !usehcrit || FirstLos == 0 ? FirstLos :
      first_nonrepulsive_step(tstep,trajdir,FirstLos-1,parameters,ownship,traffic,epsh);
  int FirstProbHcrit = FirstNonHRep < 0 ? max+1 : FirstNonHRep;
  int FirstProbHL = Util::min(FirstLos,FirstProbHcrit);
  int FirstNonVRep = !usevcrit || FirstProbHL == 0 ? FirstProbHL :
      first_nonvert_repul_step(tstep,trajdir,FirstProbHL-1,parameters,ownship,traffic,epsv);
  int FirstProbVcrit = FirstNonVRep < 0 ? max+1 : FirstNonVRep;
  return Util::min(FirstProbHL,FirstProbVcrit);
}

/** In PVS:
 * NOT CD_future_traj(CD,B,T,traj,kts,si,vi)) AND (NOT (useLOS2 AND CD_future_traj(CD2,B2,T2,traj,kts,si,vi)
 */

bool DaidalusIntegerBands::no_CD_future_traj(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2,
    double T2, bool trajdir, double tsk,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic) const {
  return !CD_future_traj(conflict_det,B,T,trajdir,tsk,parameters,ownship,traffic) &&
      !(recovery_det != NULL && CD_future_traj(recovery_det,B2,T2,trajdir,tsk,parameters,ownship,traffic));
}

// In PVS: int_bands@traj_conflict_only_band, int_bands@nat_bands, and int_bands@nat_bands_rec

void DaidalusIntegerBands::traj_conflict_only_bands(std::vector<Integerval>& l,
    const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep, double B, double T, double B2, double T2,
    bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic) const {
  int d = -1; // Set to the first index with no conflict
  for (int k = 0; k <= max; ++k) {
    double tsk = tstep*k;
    if (d >=0 && no_CD_future_traj(conflict_det,recovery_det,B,T,B2,T2,trajdir,tsk,parameters,ownship,traffic)) {
      continue;
    } else if (d >=0) {
      l.push_back( Integerval(d,k-1));
      d = -1;
    } else if (no_CD_future_traj(conflict_det,recovery_det,B,T,B2,T2,trajdir,tsk,parameters,ownship,traffic)) {
      d = k;
    }
  }
  if (d >= 0 && d != max) {
    l.push_back( Integerval(d,max));
  }
}

// In PVS: kinematic_bands@kinematic_bands
void DaidalusIntegerBands::kinematic_bands(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) const {
  l.clear();
  int bsi = bands_search_index(conflict_det,recovery_det,tstep,B,T,B2,T2,trajdir,max,parameters,ownship,traffic,epsh,epsv);
  if  (bsi != 0) {
    traj_conflict_only_bands(l,conflict_det,recovery_det,tstep,B,T,B2,T2,trajdir,bsi-1,parameters,ownship,traffic);
  }
}


void DaidalusIntegerBands::append_intband(std::vector<Integerval>& l, std::vector<Integerval>& r) {
  // Append in place
  int last = l.size()-1;
  if (!l.empty() && !r.empty() && r[0].lb-l[last].ub <= 1) {
    l[last].ub = r[0].ub;
    r.erase(r.begin()); // side effect
  }
  l.insert(l.end(), r.begin(), r.end());
}

void DaidalusIntegerBands::neg(std::vector<Integerval>& l) {
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
// In PVS: combine_bands@kinematic_bands_combine
void DaidalusIntegerBands::kinematic_bands_combine(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    int maxl, int maxr,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) const {
  kinematic_bands(l,conflict_det,recovery_det,tstep,B,T,B2,T2,false,maxl,parameters,ownship,traffic,epsh,epsv);
  std::vector<Integerval> r;
  kinematic_bands(r,conflict_det,recovery_det,tstep,B,T,B2,T2,true,maxr,parameters,ownship,traffic,epsh,epsv);
  neg(l);
  append_intband(l,r);
}

// INTERFACE FUNCTION
// In PVS: kinematic_bands_exist@first_green
int DaidalusIntegerBands::first_green(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) const {
  bool usehcrit = epsh != 0;
  bool usevcrit = epsv != 0;
  for (int k=0; k <= max; ++k) {
    double tsk = tstep*k;
    if ((tsk >= B && tsk <= T && LOS_at(conflict_det,trajdir,tsk,parameters,ownship,traffic)) ||
        (recovery_det != NULL && tsk >= B2 && tsk <= T2 &&
            LOS_at(recovery_det,trajdir,tsk,parameters,ownship,traffic)) ||
            (usehcrit && !repulsive_at(tstep,trajdir,k,parameters,ownship,traffic,epsh)) ||
            (usevcrit && !vert_repul_at(tstep,trajdir,k,parameters,ownship,traffic,epsv))) {
      return -1;
    } else if (no_CD_future_traj(conflict_det,recovery_det,B,T,B2,T2,trajdir,tsk,parameters,ownship,traffic)) {
      return k;
    }
  }
  return -1;
}

bool DaidalusIntegerBands::all_int_red(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    int maxl, int maxr,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv, int dir) const {
  bool leftans = dir > 0 || first_green(conflict_det,recovery_det,tstep,B,T,B2,T2,false,maxl,parameters,ownship,traffic,epsh,epsv) < 0;
  bool rightans = dir < 0 || first_green(conflict_det,recovery_det,tstep,B,T,B2,T2,true,maxr,parameters,ownship,traffic,epsh,epsv) < 0;
  return leftans && rightans;
}


int DaidalusIntegerBands::first_instantaneous_green(const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) {
  for (int k = 0; k <= max; ++k) {
    j_step_ = k;
    if (no_instantaneous_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,parameters,ownship,traffic,epsh,epsv)) {
      return k;
    }
  }
  return -1;
}


bool DaidalusIntegerBands::all_instantaneous_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    int maxl, int maxr,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv, int dir) {
  bool leftans = dir > 0 || first_instantaneous_green(conflict_det,recovery_det,B,T,B2,T2,false,maxl,parameters,ownship,traffic,epsh,epsv) < 0;
  bool rightans = dir < 0 || first_instantaneous_green(conflict_det,recovery_det,B,T,B2,T2,true,maxr,parameters,ownship,traffic,epsh,epsv) < 0;
  return leftans && rightans;
}


Vect3 DaidalusIntegerBands::linvel(const DaidalusParameters& parameters, const TrafficState& ownship, double tstep, bool trajdir, int k) const {
  Vect3 s1 = trajectory(parameters,ownship,(k+1)*tstep,trajdir).first;
  Vect3 s0 = trajectory(parameters,ownship,k*tstep,trajdir).first;
  return s1.Sub(s0).Scal(1/tstep);
}

// In PVS: int_bands@repulsive_at
bool DaidalusIntegerBands::repulsive_at(double tstep, bool trajdir, int k,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic, int epsh) const {
  // k >= 0
  if (k==0) {
    return true;
  }
  std::pair<Vect3,Velocity> sovo = trajectory(parameters,ownship,0,trajdir);
  Vect2 so = sovo.first.vect2();
  Vect2 vo = sovo.second.vect2();
  Vect2 si = traffic.get_s().vect2();
  Vect2 vi = traffic.get_v().vect2();
  bool rep = true;
  if (k==1) {
    rep = CriteriaCore::horizontal_new_repulsive_criterion(so.Sub(si), vo, vi, linvel(parameters,ownship,tstep,trajdir,0).vect2(), epsh);
  }
  if (rep) {
    std::pair<Vect3,Velocity> sovot = trajectory(parameters,ownship,k*tstep,trajdir);
    Vect2 sot = sovot.first.vect2();
    Vect2 vot = sovot.second.vect2();
    Vect2 sit = vi.ScalAdd(k*tstep,si);
    Vect2 st = sot.Sub(sit);
    Vect2 vop = linvel(parameters,ownship,tstep,trajdir,k-1).vect2();
    Vect2 vok = linvel(parameters,ownship,tstep,trajdir,k).vect2();
    return CriteriaCore::horizontal_new_repulsive_criterion(st,vop,vi,vot,epsh) &&
        CriteriaCore::horizontal_new_repulsive_criterion(st,vot,vi,vok,epsh) &&
        CriteriaCore::horizontal_new_repulsive_criterion(st,vop,vi,vok,epsh);
  }
  return false;
}

// In PVS: int_bands@first_nonrepulsive_step
int DaidalusIntegerBands::first_nonrepulsive_step(double tstep, bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic, int epsh) const {
  for (int k=0; k <= max; ++k) {
    if (!repulsive_at(tstep,trajdir,k,parameters,ownship,traffic,epsh)) {
      return k;
    }
  }
  return -1;
}

// In PVS: int_bands@vert_repul_at
bool DaidalusIntegerBands::vert_repul_at(double tstep, bool trajdir, int k,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic, int epsv) const {
  // traffic is valid and k >= 0
  if (k==0) {
    return true;
  }
  std::pair<Vect3,Velocity> sovo = trajectory(parameters,ownship,0,trajdir);
  Vect3 so = sovo.first;
  Vect3 vo = sovo.second;
  Vect3 si = traffic.get_s();
  Vect3 vi = traffic.get_v();
  bool rep = true;
  if (k==1) {
    rep = CriteriaCore::vertical_new_repulsive_criterion(so.Sub(si),vo,vi,linvel(parameters,ownship,tstep,trajdir,0),epsv);
  }
  if (rep) {
    std::pair<Vect3,Velocity> sovot = trajectory(parameters,ownship,k*tstep,trajdir);
    Vect3 sot = sovot.first;
    Vect3 vot = sovot.second;
    Vect3 sit = vi.ScalAdd(k*tstep,si);
    Vect3 st = sot.Sub(sit);
    Vect3 vop = linvel(parameters,ownship,tstep,trajdir,k-1);
    Vect3 vok = linvel(parameters,ownship,tstep,trajdir,k);
    return CriteriaCore::vertical_new_repulsive_criterion(st,vop,vi,vot,epsv) &&
        CriteriaCore::vertical_new_repulsive_criterion(st,vot,vi,vok,epsv) &&
        CriteriaCore::vertical_new_repulsive_criterion(st,vop,vi,vok,epsv);
  }
  return false;
}

// In PVS: int_bands@first_nonvert_repul_step
int DaidalusIntegerBands::first_nonvert_repul_step(double tstep, bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic, int epsv) const {
  for (int k=0; k <= max; ++k) {
    if (!vert_repul_at(tstep,trajdir,k,parameters,ownship,traffic,epsv)) {
      return k;
    }
  }
  return -1;
}

// In PVS: int_bands@CD_future_traj
bool DaidalusIntegerBands::CD_future_traj(const Detection3D* det, double B, double T, bool trajdir, double tsk,
   const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic) const {
  if (tsk > T || B > T) return false;
  std::pair<Vect3,Velocity> sovot = trajectory(parameters,ownship,tsk,trajdir);
  Vect3 sot = sovot.first;
  Velocity vot = sovot.second;
  Vect3 sat = tsk == 0.0 ? sot : vot.ScalAdd(-tsk,sot);
  TrafficState own = ownship;
  own.setPosition(Position(sat));
  own.setAirVelocity(vot);
  return det->conflictWithTrafficState(own,traffic,Util::max(B,tsk),T);
}

// In PVS: first_conflict_step(CD,B,T,traj,0,ts,si,vi,MaxN) >= 0
bool DaidalusIntegerBands::any_conflict_step(const Detection3D* det, double tstep, double B, double T, bool trajdir, int max,
   const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic) const {
  for (int k=0; k <= max; ++k) {
    if (CD_future_traj(det,B,T,trajdir,tstep*k,parameters,ownship,traffic)) {
      return true;
    }
  }
  return false;
}

// In PVS: kinematic_bands_exist@red_band_exist
bool DaidalusIntegerBands::red_band_exist(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) const {
  bool usehcrit = epsh != 0;
  bool usevcrit = epsv != 0;
  return (usehcrit && first_nonrepulsive_step(tstep,trajdir,max,parameters,ownship,traffic,epsh) >= 0) ||
      (usevcrit && first_nonvert_repul_step(tstep,trajdir,max,parameters,ownship,traffic,epsv) >= 0) ||
      any_conflict_step(conflict_det,tstep,B,T,trajdir,max,parameters,ownship,traffic) ||
      (recovery_det != NULL && any_conflict_step(recovery_det,tstep,B2,T2,trajdir,max,parameters,ownship,traffic));
}

bool DaidalusIntegerBands::instantaneous_red_band_exist(const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) {
  for (int k = 0; k <= max; ++k) {
    j_step_ = k;
    if (!no_instantaneous_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,parameters,ownship,traffic,epsh,epsv)) {
      return true;
    }
  }
  return false;
}

// INTERFACE FUNCTION
bool DaidalusIntegerBands::any_int_red(const Detection3D* conflict_det, const Detection3D* recovery_det, double tstep,
    double B, double T, double B2, double T2,
    int maxl, int maxr,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv, int dir) const {
  bool leftred = dir <= 0 && red_band_exist(conflict_det,recovery_det,tstep,B,T,B2,T2,false,maxl,parameters,ownship,traffic,epsh,epsv);
  bool rightred = dir >= 0 && red_band_exist(conflict_det,recovery_det,tstep,B,T,B2,T2,true,maxr,parameters,ownship,traffic,epsh,epsv);
  return leftred || rightred;
}

bool DaidalusIntegerBands::any_instantaneous_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    int maxl, int maxr,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv, int dir) {
  bool leftred = dir <= 0 && instantaneous_red_band_exist(conflict_det,recovery_det,B,T,B2,T2,false,maxl,parameters,ownship,traffic,epsh,epsv);
  bool rightred = dir >= 0 && instantaneous_red_band_exist(conflict_det,recovery_det,B,T,B2,T2,true,maxr,parameters,ownship,traffic,epsh,epsv);
  return leftred || rightred;
}

// INTERFACE FUNCTION
// In PVS: inst_bands@instant_track_bands, inst_bands@instant_gs_bands, inst_bands@instant_vs_bands
void DaidalusIntegerBands::instantaneous_bands_combine(std::vector<Integerval>& l, const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    int maxl, int maxr,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) {
  instantaneous_bands(l,conflict_det,recovery_det,B,T,B2,T2,false,maxl,parameters,ownship,traffic,epsh,epsv);
  std::vector<Integerval> r;
  instantaneous_bands(r,conflict_det,recovery_det,B,T,B2,T2,true,maxr,parameters,ownship,traffic,epsh,epsv);
  neg(l);
  append_intband(l,r);
}

//In PVS: inst_bands@conflict_free_track_step, inst_bands@conflict_free_gs_step, inst_bands@conflict_free_vs_step
bool DaidalusIntegerBands::no_instantaneous_conflict(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
    bool trajdir,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) const {
  bool usehcrit = epsh != 0;
  bool usevcrit = epsv != 0;
  std::pair<Vect3,Velocity> nsovo = trajectory(parameters,ownship,0,trajdir);
  Vect3 so = ownship.get_s();
  Velocity vo = ownship.get_v();
  Vect3 si = traffic.get_s();
  Velocity vi = traffic.get_v();
  Velocity nvo = nsovo.second;
  Vect3 s = so.Sub(si);
  return
      (!usehcrit || CriteriaCore::horizontal_new_repulsive_criterion(s.vect2(),vo.vect2(),vi.vect2(),nvo.vect2(),epsh)) &&
      (!usevcrit || CriteriaCore::vertical_new_repulsive_criterion(s,vo,vi,nvo,epsv)) &&
      no_CD_future_traj(conflict_det,recovery_det,B,T,B2,T2,trajdir,0.0,parameters,ownship,traffic);
}

//In PVS: int_bands@nat_bands, int_bands@nat_bands_rec
void DaidalusIntegerBands::instantaneous_bands(std::vector<Integerval>& l,
    const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
    bool trajdir, int max,const DaidalusParameters& parameters,  const TrafficState& ownship, const TrafficState& traffic,
    int epsh, int epsv) {
  int d = -1; // Set to the first index with no conflict
  for (int k = 0; k <= max; ++k) {
    j_step_ = k;
    if (d >=0 && no_instantaneous_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,parameters,ownship,traffic,epsh,epsv)) {
      continue;
    } else if (d >=0) {
      Integerval iv = Integerval(d,k-1);
      l.push_back(iv);
      d = -1;
    } else if (no_instantaneous_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,parameters,ownship,traffic,epsh,epsv)) {
      d = k;
    }
  }
  if (d >= 0 && d != max) {
    Integerval iv = Integerval(d,max);
    l.push_back(iv);
  }
}

}
