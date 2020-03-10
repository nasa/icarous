/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DAIDALUSDIRBANDS_H_
#define DAIDALUSDIRBANDS_H_

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

class DaidalusDirBands : public DaidalusRealBands {


private:
  double turn_rate_;
  double bank_angle_; // Only used when turn_rate is set to 0

public:
  DaidalusDirBands(DaidalusParameters& parameters);

  DaidalusDirBands(const DaidalusDirBands& b);

  /**
   * Set DaidalusParmaeters
   */
  virtual void setDaidalusParameters(const DaidalusParameters& parameters);

  virtual bool instantaneous_bands() const;

  double get_turn_rate() const;

  void set_turn_rate(double val);

  double get_bank_angle() const;

  void set_bank_angle(double val);

  virtual double own_val(const TrafficState& ownship) const;

  virtual double time_step(const TrafficState& ownship) const;

  virtual std::pair<Vect3, Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const;

  virtual double max_delta_resolution(const DaidalusParameters& parameters) const;

};

}

#endif
