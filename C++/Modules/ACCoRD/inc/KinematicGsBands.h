/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICGSBANDS_H_
#define KINEMATICGSBANDS_H_

#include "KinematicRealBands.h"
#include "Detection3D.h"
#include "TrafficState.h"
#include "IntervalSet.h"

namespace larcfm {


class KinematicGsBands : public KinematicRealBands {

private:
  double horizontal_accel_; // Climb/descend acceleration

public:
  KinematicGsBands(const KinematicBandsParameters& parameters);

  KinematicGsBands(const KinematicGsBands& b);

  bool instantaneous_bands() const;

  double get_horizontal_accel() const;

  void set_horizontal_accel(double val);

  double own_val(const TrafficState& ownship) const;

  double time_step(const TrafficState& ownship) const;

  std::pair<Vect3, Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const;

};

}

#endif
