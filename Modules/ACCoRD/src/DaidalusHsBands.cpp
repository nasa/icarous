/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusHsBands.h"
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

DaidalusHsBands::DaidalusHsBands(DaidalusParameters& parameters) {
  setDaidalusParameters(parameters);
}

DaidalusHsBands::DaidalusHsBands(const DaidalusHsBands& b) {
  super_DaidalusRealBands(&b);
  horizontal_accel_ = b.horizontal_accel_;
}

/**
 * Set DaidalusParmaeters
 */
void DaidalusHsBands::setDaidalusParameters(const DaidalusParameters& parameters) {
  set_step(parameters.getHorizontalSpeedStep());
  set_recovery(parameters.isEnabledRecoveryHorizontalSpeedBands());
  set_min_rel(parameters.getBelowRelativeHorizontalSpeed());
  set_max_rel(parameters.getAboveRelativeHorizontalSpeed());
  set_min_nomod(parameters.getMinHorizontalSpeed());
  set_max_nomod(parameters.getMaxHorizontalSpeed());
  set_horizontal_accel(parameters.getHorizontalAcceleration());
}

bool DaidalusHsBands::instantaneous_bands() const {
  return horizontal_accel_ == 0;
}

double DaidalusHsBands::get_horizontal_accel() const {
  return horizontal_accel_;
}

void DaidalusHsBands::set_horizontal_accel(double val) {
  if (val != horizontal_accel_) {
    horizontal_accel_ = val;
    stale(true);
  }
}


double DaidalusHsBands::own_val(const TrafficState& ownship) const {
  return ownship.velocityXYZ().gs();
}

double DaidalusHsBands::time_step(const TrafficState& ownship) const {
  return get_step()/horizontal_accel_;
}

std::pair<Vect3, Velocity> DaidalusHsBands::trajectory(const TrafficState& ownship, double time, bool dir) const {
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands()) {
    double gs = ownship.velocityXYZ().gs()+(dir?1:-1)*j_step_*get_step();
    posvel = std::pair<Position, Velocity>(ownship.positionXYZ(),ownship.velocityXYZ().mkGs(gs));
  } else {
    posvel = ProjectedKinematics::gsAccel(ownship.positionXYZ(),ownship.velocityXYZ(),time,
            (dir?1:-1)*horizontal_accel_);
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}


double DaidalusHsBands::max_delta_resolution(const DaidalusParameters& parameters) const {
  return parameters.getPersistencePreferredHorizontalSpeedResolution();
}





}

