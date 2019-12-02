/*
 * Copyright (c) 2015-2018 United States Government as represented by
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

DaidalusDirBands::DaidalusDirBands(DaidalusParameters& parameters) {
  super_mod(2*Pi);
  setDaidalusParameters(parameters);
}

DaidalusDirBands::DaidalusDirBands(const DaidalusDirBands& b) {
  super_DaidalusRealBands(&b);
  turn_rate_ = b.turn_rate_;
  bank_angle_ = b.bank_angle_;
}

/**
 * Set DaidalusParmaeters
 */
void DaidalusDirBands::setDaidalusParameters(const DaidalusParameters& parameters) {
  set_step(parameters.getHorizontalDirectionStep());
  set_recovery(parameters.isEnabledRecoveryHorizontalDirectionBands());
  set_min_rel(parameters.getLeftHorizontalDirection());
  set_max_rel(parameters.getRightHorizontalDirection());
  set_turn_rate(parameters.getTurnRate());
  set_bank_angle(parameters.getBankAngle());
}

bool DaidalusDirBands::instantaneous_bands() const {
  return turn_rate_ == 0 && bank_angle_ == 0;
}

double DaidalusDirBands::get_turn_rate() const {
  return turn_rate_;
}

void DaidalusDirBands::set_turn_rate(double val) {
  if (val != turn_rate_) {
    turn_rate_ = val;
    stale(true);
  }
}

double DaidalusDirBands::get_bank_angle() const {
  return bank_angle_;
}

void DaidalusDirBands::set_bank_angle(double val) {
  if (val != bank_angle_) {
    bank_angle_ = val;
    stale(true);
  }
}

double DaidalusDirBands::own_val(const TrafficState& ownship) const {
  return ownship.velocityXYZ().compassAngle();
}

double DaidalusDirBands::time_step(const TrafficState& ownship) const {
  double gso = ownship.velocityXYZ().gs();
  double omega = turn_rate_ == 0 ? Kinematics::turnRate(gso,bank_angle_) : turn_rate_;
  return get_step()/omega;
}

std::pair<Vect3, Velocity> DaidalusDirBands::trajectory(const TrafficState& ownship, double time, bool dir) const {
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands()) {
    double trk = ownship.velocityXYZ().compassAngle()+(dir?1:-1)*j_step_*get_step();
    posvel = std::pair<Position, Velocity>(ownship.positionXYZ(),ownship.velocityXYZ().mkTrk(trk));
  } else {
    double gso = ownship.velocityXYZ().gs();
    double bank = turn_rate_ == 0 ? bank_angle_ : std::abs(Kinematics::bankAngle(gso,turn_rate_));
    double R = Kinematics::turnRadius(gso,bank);
    posvel = ProjectedKinematics::turn(ownship.positionXYZ(),ownship.velocityXYZ(),time,R,dir);
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

double DaidalusDirBands::max_delta_resolution(const DaidalusParameters& parameters) const {
  return parameters.getPersistencePreferredHorizontalDirectionResolution();
}

}

