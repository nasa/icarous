/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusAltBands.h"
#include "Velocity.h"
#include "Position.h"
#include "Detection3D.h"
#include "DaidalusRealBands.h"
#include "IntervalSet.h"
#include "Tuple5.h"
#include "Kinematics.h"
#include "ProjectedKinematics.h"
#include <vector>

namespace larcfm {

DaidalusAltBands::DaidalusAltBands(DaidalusParameters& parameters) {
  setDaidalusParameters(parameters);
}

DaidalusAltBands::DaidalusAltBands(const DaidalusAltBands& b) {
  super_DaidalusRealBands(&b);
  vertical_rate_ = b.vertical_rate_;
  vertical_accel_ = b.vertical_accel_;
}

/**
 * Set DaidalusParmaeters
 */
void DaidalusAltBands::setDaidalusParameters(const DaidalusParameters& parameters) {
  set_step(parameters.getAltitudeStep());
  set_recovery(parameters.isEnabledRecoveryAltitudeBands());
  set_min_rel(parameters.getBelowRelativeAltitude());
  set_max_rel(parameters.getAboveRelativeAltitude());
  set_min_nomod(parameters.getMinAltitude());
  set_max_nomod(parameters.getMaxAltitude());
  set_vertical_rate(parameters.getVerticalRate());
  set_vertical_accel(parameters.getVerticalAcceleration());
}

bool DaidalusAltBands::instantaneous_bands() const {
  return vertical_rate_ == 0 || vertical_accel_ == 0;
}

double DaidalusAltBands::get_vertical_rate() const {
  return vertical_rate_;
}

void DaidalusAltBands::set_vertical_rate(double val) {
  if (val != vertical_rate_) {
    vertical_rate_ = val;
    stale(true);
  }
}

double DaidalusAltBands::get_vertical_accel() const {
  return vertical_accel_;
}

void DaidalusAltBands::set_vertical_accel(double val) {
  if (val != vertical_accel_) {
    vertical_accel_ = val;
    stale(true);
  }
}

double DaidalusAltBands::own_val(const TrafficState& ownship) const {
  return ownship.positionXYZ().alt();
}

double DaidalusAltBands::time_step(const TrafficState& ownship) const {
  return 1;
}

std::pair<Vect3, Velocity> DaidalusAltBands::trajectory(const TrafficState& ownship, double time, bool dir) const {
  double target_alt = get_min_val__()+j_step_*get_step();
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands()) {
    posvel = std::pair<Position, Velocity>(ownship.positionXYZ().mkZ(target_alt),ownship.velocityXYZ().mkVs(0));
  } else {
    double tsqj = ProjectedKinematics::vsLevelOutTime(ownship.positionXYZ(),ownship.velocityXYZ(),vertical_rate_,
        target_alt,vertical_accel_)+time_step(ownship);
    if (time <= tsqj) {
      posvel = ProjectedKinematics::vsLevelOut(ownship.positionXYZ(), ownship.velocityXYZ(), time, vertical_rate_, target_alt, vertical_accel_);
    } else {
      Position npo = ownship.positionXYZ().linear(ownship.velocityXYZ(),time);
      posvel = std::pair<Position, Velocity>(npo.mkZ(target_alt),ownship.velocityXYZ().mkVs(0));
    }
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

// In PVS: alt_bands@conflict_free_traj_step
bool DaidalusAltBands::conflict_free_traj_step(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
    const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) const {
  bool trajdir = true;
  if (instantaneous_bands()) {
    return no_CD_future_traj(conflict_det,recovery_det,B,T,B2,T2,trajdir,0.0,ownship,traffic,parameters);
  } else {
    double tstep = time_step(ownship);
    double target_alt = get_min_val__()+j_step_*get_step();
    Tuple5<double,double,double,double,double> tsqj = Kinematics::vsLevelOutTimes(ownship.positionXYZ().alt(),ownship.velocityXYZ().vs(),
        vertical_rate_,target_alt,vertical_accel_,-vertical_accel_,true);
    double tsqj1 = tsqj.first+0.0;
    double tsqj2 = tsqj.second+0.0;
    double tsqj3 = tsqj.third+tstep;
    for (int i=0; i<=std::floor(tsqj1/tstep);++i) {
      double tsi = i*tstep;
      if ((B<=tsi && tsi<=T && LOS_at(conflict_det,trajdir,tsi,ownship,traffic,parameters)) ||
          (recovery_det != NULL && B2 <= tsi && tsi <= T2 &&
              LOS_at(recovery_det,trajdir,tsi,ownship,traffic,parameters))) {
        return false;
      }
    }
    if ((tsqj2>=B &&
        CD_future_traj(conflict_det,B,std::min(T,tsqj2),trajdir,std::max(tsqj1,0.0),ownship,traffic,parameters)) ||
        (recovery_det != NULL && tsqj2>=B2 &&
            CD_future_traj(recovery_det,B2,std::min(T2,tsqj2),trajdir,std::max(tsqj1,0.0),ownship,traffic,parameters))) {
      return false;
    }
    for (int i=(int)std::ceil(tsqj2/tstep); i<=std::floor(tsqj3/tstep);++i) {
      double tsi = i*tstep;
      if ((B<=tsi && tsi<=T && LOS_at(conflict_det,trajdir,tsi,ownship,traffic,parameters)) ||
          (recovery_det != NULL && B2 <= tsi && tsi <= T2 &&
              LOS_at(recovery_det,trajdir,tsi,ownship,traffic,parameters))) {
        return false;
      }
    }
    return no_CD_future_traj(conflict_det,recovery_det,B,T,B2,T2,trajdir,std::max(tsqj3,0.0),ownship,traffic,parameters);
  }
}

// In PVS: alt_bands@alt_bands_generic
void DaidalusAltBands::alt_bands_generic(std::vector<Integerval>& l,
    const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
    int maxup, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) {
  int d = -1; // Set to the first index with no conflict
  for (int k = 0; k <= maxup; ++k) {
    j_step_ = k;
    if (d >=0 && conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic,parameters)) {
      continue;
    } else if (d >=0) {
      l.push_back(Integerval(d,k-1));
      d = -1;
    } else if (conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic,parameters)) {
      d = k;
    }
  }
  if (d >= 0) {
    l.push_back(Integerval(d,maxup));
  }
}

void DaidalusAltBands::none_bands(IntervalSet& noneset, const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) {
  int maxup = (int)std::floor((get_max_val__()-get_min_val__())/get_step())+1;
  std::vector<Integerval> altint;
  alt_bands_generic(altint,conflict_det,recovery_det,B,T,0.0,B,maxup,ownship,traffic,parameters);
  toIntervalSet(noneset,altint,get_step(),get_min_val__());
}

bool DaidalusAltBands::any_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) {
  return first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,parameters,true,false) >= 0 ||
      first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,parameters,false,false) >= 0;
}

bool DaidalusAltBands::all_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) {
  return first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,parameters,true,true) < 0 &&
      first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,parameters,false,true) < 0;
}

int DaidalusAltBands::first_nat(int mini, int maxi, bool dir, const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T, double B2, double T2, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters, bool green) {
  while (mini <= maxi) {
    j_step_ = mini;
    if (dir && green == conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic,parameters)) {
      return j_step_;
    } else if (dir) {
      ++mini;
    } else {
      j_step_ = maxi;
      if (green == conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic,parameters)) {
        return j_step_;
      } else if (maxi == 0) {
        return -1;
      } else {
        --maxi;
      }
    }
  }
  return -1;
}

int DaidalusAltBands::first_band_alt_generic(const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters, bool dir, bool green) {
  int upper = (int)(dir ? std::floor((get_max_val__()-get_min_val__())/get_step())+1 :
      std::floor((ownship.positionXYZ().alt()-get_min_val__())/get_step()));
  int lower = dir ? (int)(std::ceil(ownship.positionXYZ().alt()-get_min_val__())/get_step()) : 0;
  if (ownship.positionXYZ().alt() < get_min_val__() || ownship.positionXYZ().alt() > get_max_val__()) {
    return -1;
  } else {
    return first_nat(lower,upper,dir,conflict_det,recovery_det,B,T,B2,T2,ownship,traffic,parameters,green);
  }
}

// dir=false is down, dir=true is up
double DaidalusAltBands::resolution(const Detection3D* conflict_det, const Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
    bool dir) {
  int ires = first_band_alt_generic(conflict_det,recovery_det,B,T,0.0,B,ownship,traffic,parameters,dir,true);
  if (ires < 0) {
    return (dir ? 1 : -1)*PINFINITY;
  } else {
    return get_min_val__()+ires*get_step();
  }
}

double DaidalusAltBands::max_delta_resolution(const DaidalusParameters& parameters) const {
  return parameters.getPersistencePreferredAltitudeResolution();
}





}

