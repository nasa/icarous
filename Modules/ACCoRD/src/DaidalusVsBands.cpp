/*
 * Copyright (c) 2015-2020 United States Government as represented by
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

DaidalusVsBands::DaidalusVsBands() {}

DaidalusVsBands::DaidalusVsBands(const DaidalusVsBands& b) : DaidalusRealBands(b) {}

bool DaidalusVsBands::get_recovery(const DaidalusParameters& parameters) const {
  return parameters.isEnabledRecoveryVerticalSpeedBands();
}

double DaidalusVsBands::get_step(const DaidalusParameters& parameters) const {
  return parameters.getVerticalSpeedStep();
}

double DaidalusVsBands::get_min(const DaidalusParameters& parameters) const {
  return parameters.getMinVerticalSpeed();
}

double DaidalusVsBands::get_max(const DaidalusParameters& parameters) const {
  return parameters.getMaxVerticalSpeed();
}

double DaidalusVsBands::get_min_rel(const DaidalusParameters& parameters) const {
  return parameters.getBelowRelativeVerticalSpeed();
}

double DaidalusVsBands::get_max_rel(const DaidalusParameters& parameters) const {
  return parameters.getAboveRelativeVerticalSpeed();
}

bool DaidalusVsBands::instantaneous_bands(const DaidalusParameters& parameters) const {
  return parameters.getVerticalAcceleration() == 0;
}

void DaidalusVsBands::set_special_configuration(const DaidalusParameters& parameters, int dta_status) {
  if (dta_status > 0) {
    set_min_max_rel(0,-1);
  }
}

double DaidalusVsBands::own_val(const TrafficState& ownship) const {
  return ownship.velocityXYZ().vs();
}

double DaidalusVsBands::time_step(const DaidalusParameters& parameters, const TrafficState& ownship) const {
  return get_step(parameters)/parameters.getVerticalAcceleration();
}

std::pair<Vect3, Velocity> DaidalusVsBands::trajectory(const DaidalusParameters& parameters, const TrafficState& ownship, double time, bool dir, int target_step, bool instantaneous) const {
  std::pair<Position,Velocity> posvel;
  if (time == 0 && target_step == 0) {
    return std::pair<Vect3, Velocity>(ownship.get_s(),ownship.get_v());
  } else if (instantaneous_bands(parameters)) {
    double vs = ownship.velocityXYZ().vs()+(dir?1:-1)*target_step*get_step(parameters);
    posvel = std::pair<Position, Velocity>(ownship.positionXYZ(),ownship.velocityXYZ().mkVs(vs));
  } else {
    posvel = ProjectedKinematics::vsAccel(ownship.positionXYZ(),ownship.velocityXYZ(),time,(dir?1:-1)*parameters.getVerticalAcceleration());
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

double DaidalusVsBands::max_delta_resolution(const DaidalusParameters& parameters) const {
  return parameters.getPersistencePreferredVerticalSpeedResolution();
}

}

