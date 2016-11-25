/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "KinematicVsBands.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Position.h"
#include "Interval.h"
#include "BandsRegion.h"
#include "Integerval.h"
#include "ProjectedKinematics.h"
#include <cmath>
#include "KinematicBandsParameters.h"


namespace larcfm {

KinematicVsBands::KinematicVsBands(const KinematicBandsParameters& parameters) : KinematicRealBands(
    parameters.getMinVerticalSpeed(),
    parameters.getMaxVerticalSpeed(),
    parameters.getVerticalSpeedStep(),
    parameters.isEnabledRecoveryVerticalSpeedBands()) {
  vertical_accel_ = parameters.getVerticalAcceleration();
}

KinematicVsBands::KinematicVsBands(const KinematicVsBands& b)  : KinematicRealBands(
    b.get_min(),b.get_max(),b.get_rel(),b.get_mod(),b.get_step(),b.get_recovery()) {
  vertical_accel_ = b.vertical_accel_;
}

bool KinematicVsBands::instantaneous_bands() const {
  return vertical_accel_ == 0;
}

double KinematicVsBands::get_vertical_accel() const {
  return vertical_accel_;
}


void KinematicVsBands::set_vertical_accel(double val) {
  if (val != vertical_accel_) {
    vertical_accel_ = val;
    reset();
  }
}

double KinematicVsBands::own_val(const TrafficState& ownship) const {
  return ownship.verticalSpeed();
}

double KinematicVsBands::time_step(const TrafficState& ownship) const {
  return get_step()/vertical_accel_;
}

std::pair<Vect3, Velocity> KinematicVsBands::trajectory(const TrafficState& ownship, double time, bool dir) const {
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands()) {
    double vs = ownship.getVelocity().vs()+(dir?1:-1)*j_step_*get_step();
    posvel = std::pair<Position,Velocity>(ownship.getPosition(),ownship.getVelocity().mkVs(vs));
  } else {
    posvel = ProjectedKinematics::vsAccel(ownship.getPosition(),ownship.getVelocity(),time,
        (dir?1:-1)*vertical_accel_);
  }
  return std::pair<Vect3,Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

}
