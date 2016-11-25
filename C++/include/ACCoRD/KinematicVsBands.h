/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICVSBANDS_H_
#define KINEMATICVSBANDS_H_

#include "KinematicRealBands.h"
#include "Detection3D.h"
#include "TrafficState.h"
#include "IntervalSet.h"

namespace larcfm {


class KinematicVsBands : public KinematicRealBands {

private:
  double vertical_accel_; // Climb/descend acceleration

public:
  KinematicVsBands(const KinematicBandsParameters& parameters);

  KinematicVsBands(const KinematicVsBands& b);

  bool instantaneous_bands() const;

  double get_vertical_accel() const;

  void set_vertical_accel(double val);

  double own_val(const TrafficState& ownship) const;

  double time_step(const TrafficState& ownship) const;

  std::pair<Vect3, Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const;

};

}

#endif
