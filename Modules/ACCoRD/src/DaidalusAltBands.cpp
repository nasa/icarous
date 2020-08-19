/*
 * Copyright (c) 2015-2020 United States Government as represented by
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

DaidalusAltBands::DaidalusAltBands() {}

DaidalusAltBands::DaidalusAltBands(const DaidalusAltBands& b) : DaidalusRealBands(b) {}

bool DaidalusAltBands::get_recovery(const DaidalusParameters& parameters) const {
  return parameters.isEnabledRecoveryAltitudeBands();
}

double DaidalusAltBands::get_step(const DaidalusParameters& parameters) const {
  return parameters.getAltitudeStep();
}

double DaidalusAltBands::get_min(const DaidalusParameters& parameters) const {
  return parameters.getMinAltitude();
}

double DaidalusAltBands::get_max(const DaidalusParameters& parameters) const {
  return parameters.getMaxAltitude();
}

double DaidalusAltBands::get_min_rel(const DaidalusParameters& parameters) const {
  return parameters.getBelowRelativeAltitude();
}

double DaidalusAltBands::get_max_rel(const DaidalusParameters& parameters) const {
  return parameters.getAboveRelativeAltitude();
}

void DaidalusAltBands::set_special_configuration(const DaidalusParameters& parameters, int dta_status) {
  if (dta_status > 0) {
    set_min_max_rel(0,-1);
  }
}

bool DaidalusAltBands::instantaneous_bands(const DaidalusParameters& parameters) const {
  return parameters.getVerticalRate() == 0 ||
      parameters.getVerticalAcceleration() == 0;
}

double DaidalusAltBands::own_val(const TrafficState& ownship) const {
  return ownship.positionXYZ().alt();
}

double DaidalusAltBands::time_step(const DaidalusParameters& parameters, const TrafficState& ownship) const {
  return 1;
}

std::pair<Vect3, Velocity> DaidalusAltBands::trajectory(const DaidalusParameters& parameters, const TrafficState& ownship, double time, bool dir, int target_step, bool instantaneous) const {
  double target_alt = get_min_val_()+target_step*get_step(parameters);
  std::pair<Position,Velocity> posvel;
  if (instantaneous) {
    posvel = std::pair<Position, Velocity>(ownship.positionXYZ().mkZ(target_alt),ownship.velocityXYZ().mkVs(0));
  } else {
    double tsqj = ProjectedKinematics::vsLevelOutTime(ownship.positionXYZ(),ownship.velocityXYZ(),parameters.getVerticalRate(),
        target_alt,parameters.getVerticalAcceleration())+time_step(parameters,ownship);
    if (time <= tsqj) {
      posvel = ProjectedKinematics::vsLevelOut(ownship.positionXYZ(), ownship.velocityXYZ(), time, parameters.getVerticalRate(), target_alt, parameters.getVerticalAcceleration());
    } else {
      Position npo = ownship.positionXYZ().linear(ownship.velocityXYZ(),time);
      posvel = std::pair<Position, Velocity>(npo.mkZ(target_alt),ownship.velocityXYZ().mkVs(0));
    }
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

// In PVS: alt_bands@conflict_free_traj_step
bool DaidalusAltBands::conflict_free_traj_step(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T,
    const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, int target_step, bool instantaneous) const {
  bool trajdir = true;
  if (instantaneous) {
    return no_CD_future_traj(conflict_det,recovery_det,B,T,trajdir,0.0,parameters,ownship,traffic,target_step,instantaneous);
  } else {
    double tstep = time_step(parameters,ownship);
    double target_alt = get_min_val_()+target_step*get_step(parameters);
    Tuple5<double,double,double,double,double> tsqj = Kinematics::vsLevelOutTimes(ownship.positionXYZ().alt(),ownship.velocityXYZ().vs(),
        parameters.getVerticalRate(),target_alt,parameters.getVerticalAcceleration(),-parameters.getVerticalAcceleration(),true);
    double tsqj1 = tsqj.first;
    double tsqj2 = tsqj.second;
    double tsqj3 = tsqj.third+tstep;
    for (int i=0; i<=std::floor(tsqj1/tstep);++i) {
      double tsi = i*tstep;
      if ((B <= tsi && LOS_at(conflict_det,trajdir,tsi,parameters,ownship,traffic,target_step,instantaneous)) ||
          (recovery_det != NULL && 0 <= tsi && tsi <= B &&
              LOS_at(recovery_det,trajdir,tsi,parameters,ownship,traffic,target_step,instantaneous))) {
        return false;
      }
    }
    double tsk1 = Util::max(tsqj1,0.0);
    if ((tsqj2 >= B &&
        CD_future_traj(conflict_det,B,std::min(T+tsk1,tsqj2),trajdir,tsk1,parameters,ownship,traffic,target_step,instantaneous)) ||
        (recovery_det != NULL && tsqj2 >= 0 &&
            CD_future_traj(recovery_det,0,Util::min(B,tsqj2),trajdir,tsk1,parameters,ownship,traffic,target_step,instantaneous))) {
      return false;
    }
    for (int i=(int)std::ceil(tsqj2/tstep); i<=std::floor(tsqj3/tstep);++i) {
      double tsi = i*tstep;
      if ((B <= tsi && LOS_at(conflict_det,trajdir,tsi,parameters,ownship,traffic,target_step,instantaneous)) ||
          (recovery_det != NULL && 0 <= tsi && tsi <= B &&
              LOS_at(recovery_det,trajdir,tsi,parameters,ownship,traffic,target_step,instantaneous))) {
        return false;
      }
    }
    double tsk3 = Util::max(tsqj3,0.0);
    return no_CD_future_traj(conflict_det,recovery_det,B,T+tsk3,trajdir,tsk3,parameters,ownship,traffic,target_step,instantaneous);
  }
}

// In PVS: alt_bands@alt_bands_generic
void DaidalusAltBands::alt_bands_generic(std::vector<Integerval>& l,
    const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T,
    int maxup, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, bool instantaneous) const {
  int d = -1; // Set to the first index with no conflict
  for (int k = 0; k <= maxup; ++k) {
    if (d >=0 && conflict_free_traj_step(conflict_det,recovery_det,B,T,parameters,ownship,traffic,k,instantaneous)) {
      continue;
    } else if (d >=0) {
      l.push_back(Integerval(d,k-1));
      d = -1;
    } else if (conflict_free_traj_step(conflict_det,recovery_det,B,T,parameters,ownship,traffic,k,instantaneous)) {
      d = k;
    }
  }
  if (d >= 0) {
    l.push_back(Integerval(d,maxup));
  }
}

void DaidalusAltBands::none_bands(IntervalSet& noneset, const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) const {
  int maxup = (int)std::floor((get_max_val_()-get_min_val_())/get_step(parameters))+1;
  std::vector<Integerval> altint;
  alt_bands_generic(altint,conflict_det,recovery_det,B,T,maxup,parameters,ownship,traffic,instantaneous_bands(parameters));
  toIntervalSet(noneset,altint,get_step(parameters),get_min_val_());
}

bool DaidalusAltBands::any_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) const {
  return first_band_alt_generic(conflict_det,recovery_det,B,T,parameters,ownship,traffic,true,false,instantaneous_bands(parameters)) >= 0 ||
      first_band_alt_generic(conflict_det,recovery_det,B,T,parameters,ownship,traffic,false,false,instantaneous_bands(parameters)) >= 0;
}

bool DaidalusAltBands::all_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) const {
  return first_band_alt_generic(conflict_det,recovery_det,B,T,parameters,ownship,traffic,true,true,instantaneous_bands(parameters)) < 0 &&
      first_band_alt_generic(conflict_det,recovery_det,B,T,parameters,ownship,traffic,false,true,instantaneous_bands(parameters)) < 0;
}

int DaidalusAltBands::first_nat(int mini, int maxi, bool dir, const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T,  const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, bool green, bool instantaneous) const {
  while (mini <= maxi) {
    if (dir && green == conflict_free_traj_step(conflict_det,recovery_det,B,T,parameters,ownship,traffic,mini,instantaneous)) {
      return mini;
    } else if (dir) {
      ++mini;
    } else {
      if (green == conflict_free_traj_step(conflict_det,recovery_det,B,T,parameters,ownship,traffic,maxi,instantaneous)) {
        return maxi;
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
    double B, double T,
    const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, bool dir, bool green, bool instantaneous) const {
  int upper = (int)(dir ? std::floor((get_max_val_()-get_min_val_())/get_step(parameters))+1 :
      std::floor((ownship.positionXYZ().alt()-get_min_val_())/get_step(parameters)));
  int lower = dir ? (int)(std::ceil(ownship.positionXYZ().alt()-get_min_val_())/get_step(parameters)) : 0;
  if (ownship.positionXYZ().alt() < get_min_val_() || ownship.positionXYZ().alt() > get_max_val_()) {
    return -1;
  } else {
    return first_nat(lower,upper,dir,conflict_det,recovery_det,B,T,parameters,ownship,traffic,green,instantaneous);
  }
}

double DaidalusAltBands::max_delta_resolution(const DaidalusParameters& parameters) const {
  return parameters.getPersistencePreferredAltitudeResolution();
}

}

