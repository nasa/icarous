/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICTRKBANDS_H_
#define KINEMATICTRKBANDS_H_

#include "KinematicRealBands.h"
#include "Detection3D.h"
#include "TrafficState.h"
#include "IntervalSet.h"

namespace larcfm {


class KinematicTrkBands : public KinematicRealBands {

private:
  double turn_rate_;
  double bank_angle_;  // Only used when turn_rate is set to 0

public:
  KinematicTrkBands(const KinematicBandsParameters& parameters);

  KinematicTrkBands(const KinematicTrkBands& b);

  bool instantaneous_bands() const;

  double get_turn_rate() const;

  void set_turn_rate(double val);

  double get_bank_angle() const;

  void set_bank_angle(double val);

  double own_val(const TrafficState& ownship) const;

  double time_step(const TrafficState& ownship) const;

  std::pair<Vect3, Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const;

};

}

#endif



