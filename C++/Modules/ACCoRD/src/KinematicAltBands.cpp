/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "KinematicAltBands.h"
#include "Vect3.h"
#include "Util.h"
#include "Velocity.h"
#include "Position.h"
#include "Interval.h"
#include "BandsRegion.h"
#include "Integerval.h"
#include "KinematicBandsParameters.h"
#include "Triple.h"
#include "ProjectedKinematics.h"

namespace larcfm {

KinematicAltBands::KinematicAltBands(const KinematicBandsParameters& parameters) : KinematicRealBands(
    parameters.getMinAltitude(),
    parameters.getMaxAltitude(),
    parameters.getAltitudeStep(),
    parameters.isEnabledRecoveryAltitudeBands()) {
  vertical_rate_ = parameters.getVerticalRate();
  vertical_accel_ = parameters.getVerticalAcceleration();
  j_step_ = 0;
}

KinematicAltBands::KinematicAltBands(const KinematicAltBands& b) : KinematicRealBands(
    b.get_min(),b.get_max(),b.get_rel(),b.get_mod(),b.get_step(),b.get_recovery()) {
  vertical_rate_ = b.vertical_rate_;
  vertical_accel_ = b.vertical_accel_;
  j_step_ = 0;
}

bool KinematicAltBands::instantaneous_bands() const {
  return vertical_rate_ == 0 || vertical_accel_ == 0;
}


double KinematicAltBands::get_vertical_rate() const {
  return vertical_rate_;
}

void KinematicAltBands::set_vertical_rate(double val) {
  if (val != vertical_rate_) {
    vertical_rate_ = val;
    reset();
  }
}

double KinematicAltBands::get_vertical_accel() const {
  return vertical_accel_;
}

void KinematicAltBands::set_vertical_accel(double val) {
  if (val != vertical_accel_) {
    vertical_accel_ = val;
    reset();
  }
}

double KinematicAltBands::own_val(const TrafficState& ownship) const {
  return ownship.getPositionXYZ().alt();
}

double KinematicAltBands::time_step(const TrafficState& ownship) const {
  return 1;
}

std::pair<Vect3, Velocity> KinematicAltBands::trajectory(const TrafficState& ownship, double time, bool dir) const {
  double target_alt = min_val(ownship)+j_step_*get_step();
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands()) {
    posvel = std::pair<Position,Velocity>(ownship.getPositionXYZ().mkZ(target_alt),ownship.getVelocityXYZ().mkVs(0));
  } else {
    double tsqj = ProjectedKinematics::vsLevelOutTime(ownship.getPositionXYZ(),ownship.getVelocityXYZ(),vertical_rate_,
        target_alt,vertical_accel_)+time_step(ownship);
    if (time <= tsqj) {
      posvel = ProjectedKinematics::vsLevelOut(ownship.getPositionXYZ(), ownship.getVelocityXYZ(), time, vertical_rate_, target_alt, vertical_accel_);
    } else {
      Position npo = ownship.getPositionXYZ().linear(ownship.getVelocityXYZ(),time);
      posvel = std::pair<Position,Velocity>(npo.mkZ(target_alt),ownship.getVelocityXYZ().mkVs(0));
    }
  }
  return std::pair<Vect3,Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

bool KinematicAltBands::any_red(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  return first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,true,false) >= 0 ||
      first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,false,false) >= 0;
}

bool KinematicAltBands::all_red(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  return first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,true,true) < 0 &&
      first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,false,true) < 0;
}

void KinematicAltBands::none_bands(IntervalSet& noneset, Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  std::vector<Integerval> altint = std::vector<Integerval>();
  alt_bands_generic(altint,conflict_det,recovery_det,B,T,0,B,ownship,traffic);
  toIntervalSet(noneset,altint,get_step(),min_val(ownship),min_val(ownship),max_val(ownship));
}

bool KinematicAltBands::conflict_free_traj_step(Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    const TrafficState& ownship, const std::vector<TrafficState>& traffic) const {
  bool trajdir = true;
  if (instantaneous_bands()) {
    return no_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,0,ownship,traffic);
  } else {
    double tstep = time_step(ownship);
    double target_alt = min_val(ownship)+j_step_*get_step();
    Tuple5<double,double,double,double,double> tsqj = Kinematics::vsLevelOutTimes(ownship.getPositionXYZ().alt(),ownship.getVelocityXYZ().vs(),
        vertical_rate_,target_alt,vertical_accel_,-vertical_accel_,true);
    double tsqj1 = tsqj.first+0;
    double tsqj2 = tsqj.second+0;
    double tsqj3 = tsqj.third+tstep;
    for (int i=0; i<=std::floor(tsqj1/tstep);++i) {
      double tsi = i*tstep;
      if ((B<=tsi && tsi<=T && any_los_aircraft(conflict_det,trajdir,tsi,ownship,traffic)) ||
          (recovery_det != NULL && B2 <= tsi && tsi <= T2 &&
              any_los_aircraft(recovery_det,trajdir,tsi,ownship,traffic))) {
        return false;
      }
    }
    if ((tsqj2>=B &&
        any_conflict_aircraft(conflict_det,B,Util::min(T,tsqj2),trajdir,Util::max(tsqj1,0.0),ownship,traffic)) ||
        (recovery_det != NULL && tsqj2>=B2 &&
            any_conflict_aircraft(recovery_det,B2,Util::min(T2,tsqj2),trajdir,Util::max(tsqj1,0.0),ownship,traffic))) {
      return false;
    }
    for (int i=(int)std::ceil(tsqj2/tstep); i<=std::floor(tsqj3/tstep);++i) {
      double tsi = i*tstep;
      if ((B<=tsi && tsi<=T && any_los_aircraft(conflict_det,trajdir,tsi,ownship,traffic)) ||
          (recovery_det != NULL && B2 <= tsi && tsi <= T2 &&
              any_los_aircraft(recovery_det,trajdir,tsi,ownship,traffic))) {
        return false;
      }
    }
    return no_conflict(conflict_det,recovery_det,B,T,B2,T2,trajdir,Util::max(tsqj3,0.0),ownship,traffic);
  }
}

void KinematicAltBands::alt_bands_generic(std::vector<Integerval>& l,
    Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  int max_step = (int)std::floor((max_val(ownship)-min_val(ownship))/get_step())+1;
  int d = -1; // Set to the first index with no conflict
  for (int k = 0; k <= max_step; ++k) {
    j_step_ = k;
    if (d >=0 && conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic))  {
      continue;
    } else if (d >=0) {
      l.push_back(Integerval(d,k-1));
      d = -1;
    } else if (conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic)) {
      d = k;
    }
  }
  if (d >= 0) {
    l.push_back(Integerval(d,max_step));
  }
}

int KinematicAltBands::first_nat(int mini, int maxi, bool dir, Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    bool green) {
  while (mini <= maxi) {
    j_step_ = mini;
    if (dir && green == conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic)) {
      return j_step_;
    } else if (dir) {
      ++mini;
    } else {
      j_step_ = maxi;
      if (green == conflict_free_traj_step(conflict_det,recovery_det,B,T,B2,T2,ownship,traffic)) {
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

int KinematicAltBands::first_band_alt_generic(Detection3D* conflict_det, Detection3D* recovery_det,
    double B, double T, double B2, double T2,
    const TrafficState& ownship, const std::vector<TrafficState>& traffic, bool dir, bool green) {
  int upper = (int)(dir ? std::floor((max_val(ownship)-min_val(ownship))/get_step())+1 :
      std::floor((ownship.getPositionXYZ().alt()-min_val(ownship))/get_step()));
  int lower = dir ? (int)(std::ceil(ownship.getPositionXYZ().alt()-min_val(ownship))/get_step()) : 0;
  if (ownship.getPositionXYZ().alt() < min_val(ownship) || ownship.getPositionXYZ().alt() > max_val(ownship)) {
    return -1;
  } else {
    return first_nat(lower,upper,dir,conflict_det,recovery_det,B,T,B2,T2,ownship,traffic,green);
  }
}

// dir=false is down, dir=true is up. Return NaN if there is not a resolution
double KinematicAltBands::resolution(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
    bool dir) {
  int ires = first_band_alt_generic(conflict_det,recovery_det,B,T,0,B,ownship,traffic,dir,true);
  if (ires < 0) {
    return (dir ? 1 : -1)*PINFINITY;
  } else {
    return min_val(ownship)+ires*get_step();
  }
}

}


