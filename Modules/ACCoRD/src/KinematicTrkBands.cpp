/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "KinematicTrkBands.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Position.h"
#include "Detection3D.h"
#include "Interval.h"
#include "BandsRegion.h"
#include "Integerval.h"
#include "ProjectedKinematics.h"
#include <cmath>
#include "KinematicBandsParameters.h"

namespace larcfm {

KinematicTrkBands::KinematicTrkBands(const KinematicBandsParameters& parameters) : KinematicRealBands(
    -parameters.getLeftTrack(),
    parameters.getRightTrack(),
    true,2*Pi,
    parameters.getTrackStep(),
    parameters.isEnabledRecoveryTrackBands()) {
  turn_rate_ = parameters.getTurnRate();
  bank_angle_ = parameters.getBankAngle();
}

KinematicTrkBands::KinematicTrkBands(const KinematicTrkBands& b) : KinematicRealBands(
    b.get_min(),b.get_max(),b.get_rel(),b.get_mod(),b.get_step(),b.get_recovery()) {
  turn_rate_ = b.turn_rate_;
  bank_angle_ = b.bank_angle_;
}

bool KinematicTrkBands::instantaneous_bands() const {
  return turn_rate_ == 0 && bank_angle_ == 0;
}

double KinematicTrkBands::get_turn_rate() const {
  return turn_rate_;
}

void KinematicTrkBands::set_turn_rate(double val) {
  if (val != turn_rate_) {
    turn_rate_ = val;
    reset();
  }
}

double KinematicTrkBands::get_bank_angle() const {
  return bank_angle_;
}

void KinematicTrkBands::set_bank_angle(double val) {
  if (val != bank_angle_) {
    bank_angle_ = val;
    reset();
  }
}

double KinematicTrkBands::own_val(const TrafficState& ownship) const {
  return ownship.getVelocityXYZ().compassAngle();
}

double KinematicTrkBands::time_step(const TrafficState& ownship) const {
  double gso = ownship.getVelocityXYZ().gs();
  double omega = turn_rate_ == 0 ? Kinematics::turnRate(gso,bank_angle_) : turn_rate_;
  return get_step()/omega;
}

std::pair<Vect3, Velocity> KinematicTrkBands::trajectory(const TrafficState& ownship, double time, bool dir) const {
  std::pair<Position,Velocity> posvel;
  if (instantaneous_bands()) {
    double trk = ownship.getVelocityXYZ().compassAngle()+(dir?1:-1)*j_step_*get_step();
    posvel = std::pair<Position,Velocity>(ownship.getPositionXYZ(),ownship.getVelocityXYZ().mkTrk(trk));
  } else {
    double gso = ownship.getVelocityXYZ().gs();
    double bank = turn_rate_ == 0 ? bank_angle_ : std::abs(Kinematics::bankAngle(gso,turn_rate_));
    double R = Kinematics::turnRadius(ownship.get_v().gs(), bank);
    posvel = ProjectedKinematics::turn(ownship.getPositionXYZ(),ownship.getVelocityXYZ(),time,R,dir);
  }
  return std::pair<Vect3, Velocity>(ownship.pos_to_s(posvel.first),ownship.vel_to_v(posvel.first,posvel.second));
}

}
