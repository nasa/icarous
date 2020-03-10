/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusVsBands.h"
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

DaidalusVsBands::DaidalusVsBands(DaidalusParameters& parameters) {
  setDaidalusParameters(parameters);
}

DaidalusVsBands::DaidalusVsBands(const DaidalusVsBands& b) : DaidalusRealBands(b) {
  vertical_accel_ = b.vertical_accel_;
}

/**
 * Set DaidalusParmaeters
 */
void DaidalusVsBands::setDaidalusParameters(const DaidalusParameters& parameters) {
  set_step(parameters.getVerticalSpeedStep());
  set_recovery(parameters.isEnabledRecoveryVerticalSpeedBands());
  set_min_rel(parameters.getBelowRelativeVerticalSpeed());
  setmax_rel(parameters.getAboveRelativeVerticalSpeed());
  set_min_nomod(parameters.getMinVerticalSpeed());
  setmax_nomod(parameters.getMaxVerticalSpeed());
  set_vertical_accel(parameters.getVerticalAcceleration());
}

bool DaidalusVsBands::instantaneous_bands() const {
  return vertical_accel_ == 0;
}

double DaidalusVsBands::get_vertical_accel() const {
  return vertical_accel_;
}

void DaidalusVsBands::set_vertical_accel(double val) {
  if (val != vertical_accel_) {
    vertical_accel_ = val;
    stale(true);
  }
}

double DaidalusVsBands::own_val(const TrafficState& ownship) const {
  return ownship.velocityXYZ().vs();
}

double DaidalusVsBands::time_step(const TrafficState& ownship) const {
  return get_step()/vertical_accel_;
}

std::pair<Vect3, Velocity> DaidalusVsBands::trajectory(const TrafficState& ownship, double time, bool dir) const {
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands()) {
    double vs = ownship.velocityXYZ().vs()+(dir?1:-1)*j_step_*get_step();
    posvel = std::pair<Position, Velocity>(ownship.positionXYZ(),ownship.velocityXYZ().mkVs(vs));
  } else {
    posvel = ProjectedKinematics::vsAccel(ownship.positionXYZ(),ownship.velocityXYZ(),time,(dir?1:-1)*vertical_accel_);
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}


double DaidalusVsBands::max_delta_resolution(const DaidalusParameters& parameters) const {
  return parameters.getPersistencePreferredVerticalSpeedResolution();
}

}

