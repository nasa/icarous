/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusDirBands.h"
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

DaidalusDirBands::DaidalusDirBands() : DaidalusRealBands(2*Pi) {}

DaidalusDirBands::DaidalusDirBands(const DaidalusDirBands& b) : DaidalusRealBands(b) {}

bool DaidalusDirBands::get_recovery(const DaidalusParameters& parameters) const {
  return parameters.isEnabledRecoveryHorizontalDirectionBands();
}

double DaidalusDirBands::get_step(const DaidalusParameters& parameters) const {
  return parameters.getHorizontalDirectionStep();
}

double DaidalusDirBands::get_min(const DaidalusParameters& parameters) const {
  return 0;
}

double DaidalusDirBands::get_max(const DaidalusParameters& parameters) const {
  return get_mod();
}

double DaidalusDirBands::get_min_rel(const DaidalusParameters& parameters) const {
  return parameters.getLeftHorizontalDirection();
}

double DaidalusDirBands::get_max_rel(const DaidalusParameters& parameters) const {
  return parameters.getRightHorizontalDirection();
}

bool DaidalusDirBands::saturate_corrective_bands(const DaidalusParameters& parameters, int dta_status) const {
  return dta_status > 0 && parameters.getDTALogic() < 0;
}

bool DaidalusDirBands::instantaneous_bands(const DaidalusParameters& parameters) const {
  return parameters.getTurnRate() == 0 && parameters.getBankAngle() == 0;
}

double DaidalusDirBands::own_val(const TrafficState& ownship) const {
  return ownship.velocityXYZ().compassAngle();
}

double DaidalusDirBands::time_step(const DaidalusParameters& parameters, const TrafficState& ownship) const {
  double gso = ownship.velocityXYZ().gs();
  double omega = parameters.getTurnRate() == 0 ? Kinematics::turnRate(gso,parameters.getBankAngle()) : parameters.getTurnRate();
  return get_step(parameters)/omega;
}

std::pair<Vect3, Velocity> DaidalusDirBands::trajectory(const DaidalusParameters& parameters, const TrafficState& ownship, double time, bool dir) const {
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands(parameters)) {
    double trk = ownship.velocityXYZ().compassAngle()+(dir?1:-1)*j_step_*get_step(parameters);
    posvel = std::pair<Position, Velocity>(ownship.positionXYZ(),ownship.velocityXYZ().mkTrk(trk));
  } else {
    double gso = ownship.velocityXYZ().gs();
    double bank = parameters.getTurnRate() == 0 ? parameters.getBankAngle() : std::abs(Kinematics::bankAngle(gso,parameters.getTurnRate()));
    double R = Kinematics::turnRadius(gso,bank);
    posvel = ProjectedKinematics::turn(ownship.positionXYZ(),ownship.velocityXYZ(),time,R,dir);
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

double DaidalusDirBands::max_delta_resolution(const DaidalusParameters& parameters) const {
  return parameters.getPersistencePreferredHorizontalDirectionResolution();
}

}

