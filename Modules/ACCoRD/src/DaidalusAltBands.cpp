/*
 * Copyright (c) 2015-2019 United States Government as represented by
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

std::pair<Vect3, Velocity> DaidalusAltBands::trajectory(const DaidalusParameters& parameters, const TrafficState& ownship, double time, bool dir) const {
  double target_alt = get_min_val_()+j_step_*get_step(parameters);
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands(parameters)) {
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
bool DaidalusAltBands::conflict_free_traj_step(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
    const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) const {
  bool trajdir = true;
  if (instantaneous_bands(parameters)) {
    return no_CD_future_traj(conflict_det,recovery_det,B,T,B2,T2,trajdir,0.0,parameters,ownship,traffic);
  } else {
    double tstep = time_step(parameters,ownship);
    double target_alt = get_min_val_()+j_step_*get_step(parameters);
    Tuple5<double,double,double,double,double> tsqj = Kinematics::vsLevelOutTimes(ownship.positionXYZ().alt(),ownship.velocityXYZ().vs(),
        parameters.getVerticalRate(),target_alt,parameters.getVerticalAcceleration(),-parameters.getVerticalAcceleration(),true);
    double tsqj1 = tsqj.first+0.0;
    double tsqj2 = tsqj.second+0.0;
    double tsqj3 = tsqj.third+tstep;
    for (int i=0; i<=std::floor(tsqj1/tstep);++i) {
      double tsi = i*tstep;
      if ((B<=tsi && tsi<=T && LOS_at(conflict_det,trajdir,tsi,parameters,ownship,traffic)) ||
          (recovery_det != NULL && B2 <= tsi && tsi <= T2 &&
              LOS_at(recovery_det,trajdir,tsi,parameters,ownship,traffic))) {
        return false;
      }
    }
    if ((tsqj2>=B &&
        CD_future_traj(conflict_det,B,std::min(T,tsqj2),trajdir,std::max(tsqj1,0.0),parameters,ownship,traffic)) ||
        (recovery_det != NULL && tsqj2>=B2 &&
            CD_future_traj(recovery_det,B2,std::min(T2,tsqj2),trajdir,std::max(tsqj1,0.0),parameters,ownship,traffic))) {
      return false;
    }
    for (int i=(int)std::ceil(tsqj2/tstep); i<=std::floor(tsqj3/tstep);++i) {
      double tsi = i*tstep;
      if ((B<=tsi && tsi<=T && LOS_at(conflict_det,trajdir,tsi,parameters,ownship,traffic)) ||
          (recovery_det != NULL && B2 <= tsi && tsi <= T2 &&
              LOS_at(recovery_det,trajdir,tsi,parameters,ownship,traffic))) {
        return false;
      }
    }
    return no_CD_future_traj(conflict_det,recovery_det,B,T,B2,T2,trajdir,std::max(tsqj3,0.0),parameters,ownship,traffic);
  }
}

// In PVS: alt_bands@alt_bands_generic
void DaidalusAltBands::alt_bands_generic(std::vector<Integerval>& l,
    const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
    int maxup, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  int d = -1; // Set to the first index with no conflict
  for (int k = 0; k <= maxup; ++k) {
    j_step_ = k;
    if (d >=0 && conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,parameters,ownship,traffic)) {
      continue;
    } else if (d >=0) {
      l.push_back(Integerval(d,k-1));
      d = -1;
    } else if (conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,parameters,ownship,traffic)) {
      d = k;
    }
  }
  if (d >= 0) {
    l.push_back(Integerval(d,maxup));
  }
}

void DaidalusAltBands::none_bands(IntervalSet& noneset, const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  int maxup = (int)std::floor((get_max_val_()-get_min_val_())/get_step(parameters))+1;
  std::vector<Integerval> altint;
  alt_bands_generic(altint,conflict_det,recovery_det,B,T,0.0,B,maxup,parameters,ownship,traffic);
  toIntervalSet(noneset,altint,get_step(parameters),get_min_val_());
}

bool DaidalusAltBands::any_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  return first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,parameters,ownship,traffic,true,false) >= 0 ||
      first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,parameters,ownship,traffic,false,false) >= 0;
}

bool DaidalusAltBands::all_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  return first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,parameters,ownship,traffic,true,true) < 0 &&
      first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,parameters,ownship,traffic,false,true) < 0;
}

int DaidalusAltBands::first_nat(int mini, int maxi, bool dir, const Detection3D* conflict_det, const Detection3D* recovery_det,
    double B, double T, double B2, double T2, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, bool green) {
  while (mini <= maxi) {
    j_step_ = mini;
    if (dir && green == conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,parameters,ownship,traffic)) {
      return j_step_;
    } else if (dir) {
      ++mini;
    } else {
      j_step_ = maxi;
      if (green == conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,parameters,ownship,traffic)) {
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
    const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, bool dir, bool green) {
  int upper = (int)(dir ? std::floor((get_max_val_()-get_min_val_())/get_step(parameters))+1 :
      std::floor((ownship.positionXYZ().alt()-get_min_val_())/get_step(parameters)));
  int lower = dir ? (int)(std::ceil(ownship.positionXYZ().alt()-get_min_val_())/get_step(parameters)) : 0;
  if (ownship.positionXYZ().alt() < get_min_val_() || ownship.positionXYZ().alt() > get_max_val_()) {
    return -1;
  } else {
    return first_nat(lower,upper,dir,conflict_det,recovery_det,B,T,B2,T2,parameters,ownship,traffic,green);
  }
}

// dir=false is down, dir=true is up
double DaidalusAltBands::resolution(const Detection3D* conflict_det, const Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
    bool dir) {
  int ires = first_band_alt_generic(conflict_det,recovery_det,B,T,0.0,B,parameters,ownship,traffic,dir,true);
  if (ires < 0) {
    return (dir ? 1 : -1)*PINFINITY;
  } else {
    return get_min_val_()+ires*get_step(parameters);
  }
}

double DaidalusAltBands::max_delta_resolution(const DaidalusParameters& parameters) const {
  return parameters.getPersistencePreferredAltitudeResolution();
}





}

