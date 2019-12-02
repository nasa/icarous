/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DAIDALUSHSBANDS_H_
#define DAIDALUSHSBANDS_H_

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

class DaidalusHsBands : public DaidalusRealBands {


private:
  double horizontal_accel_; // Horizontal acceleration


public:
  DaidalusHsBands(DaidalusParameters& parameters);

  DaidalusHsBands(const DaidalusHsBands& b);

  /**
   * Set DaidalusParmaeters
   */
  virtual void setDaidalusParameters(const DaidalusParameters& parameters);

  virtual bool instantaneous_bands() const;

  double get_horizontal_accel() const;

  void set_horizontal_accel(double val);

  virtual double own_val(const TrafficState& ownship) const;

  virtual double time_step(const TrafficState& ownship) const;

  virtual std::pair<Vect3, Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const;

  virtual double max_delta_resolution(const DaidalusParameters& parameters) const;

};

}

#endif
