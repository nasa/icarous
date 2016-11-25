/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "KinematicGsBands.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Position.h"
#include "Interval.h"
#include "BandsRegion.h"
#include "Integerval.h"
#include "KinematicBandsParameters.h"
#include "ProjectedKinematics.h"

namespace larcfm {

KinematicGsBands::KinematicGsBands(const KinematicBandsParameters& parameters) : KinematicRealBands(
    parameters.getMinGroundSpeed(),
    parameters.getMaxGroundSpeed(),
    parameters.getGroundSpeedStep(),
    parameters.isEnabledRecoveryGroundSpeedBands()) {
  horizontal_accel_ = parameters.getHorizontalAcceleration();
}

KinematicGsBands::KinematicGsBands(const KinematicGsBands& b) : KinematicRealBands(
    b.get_min(),b.get_max(),b.get_rel(),b.get_mod(),b.get_step(),b.get_recovery()){
  horizontal_accel_ = b.horizontal_accel_;
}

bool KinematicGsBands::instantaneous_bands() const {
  return horizontal_accel_ == 0;
}

double KinematicGsBands::get_horizontal_accel() const {
  return horizontal_accel_;
}

void KinematicGsBands::set_horizontal_accel(double val) {
  if (val != horizontal_accel_) {
    horizontal_accel_ = val;
    reset();
  }
}

double KinematicGsBands::own_val(const TrafficState& ownship) const {
  return ownship.groundSpeed();
}

double KinematicGsBands::time_step(const TrafficState& ownship) const {
  return get_step()/horizontal_accel_;
}

std::pair<Vect3, Velocity> KinematicGsBands::trajectory(const TrafficState& ownship, double time, bool dir) const {
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands()) {
    double gs = ownship.getVelocity().gs()+(dir?1:-1)*j_step_*get_step();
    posvel = std::pair<Position,Velocity>(ownship.getPosition(),ownship.getVelocity().mkGs(gs));
  } else {
    posvel = ProjectedKinematics::gsAccel(ownship.getPosition(),ownship.getVelocity(),time,
        (dir?1:-1)*horizontal_accel_);
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

}

