/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DAIDALUSVSBANDS_H_
#define DAIDALUSVSBANDS_H_

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

class DaidalusVsBands : public DaidalusRealBands {


private:
  double vertical_accel_; // Climb/descend acceleration

public:
  DaidalusVsBands(DaidalusParameters& parameters);

  DaidalusVsBands(const DaidalusVsBands& b);

  /**
   * Set DaidalusParmaeters
   */
  virtual void setDaidalusParameters(const DaidalusParameters& parameters);

  virtual bool instantaneous_bands() const;

  double get_vertical_accel() const;

  void set_vertical_accel(double val);

  virtual double own_val(const TrafficState& ownship) const;

  virtual double time_step(const TrafficState& ownship) const;

  virtual std::pair<Vect3, Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const;

  virtual double max_delta_resolution(const DaidalusParameters& parameters) const;

};

}

#endif
