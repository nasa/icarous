/*
 * Copyright (c) 2015-2019 United States Government as represented by
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

public:
  DaidalusVsBands();

  DaidalusVsBands(const DaidalusVsBands& b);

  virtual bool get_recovery(const DaidalusParameters& parameters) const;

  virtual double get_step(const DaidalusParameters& parameters) const;

  virtual double get_min(const DaidalusParameters& parameters) const;

  virtual double get_max(const DaidalusParameters& parameters) const;

  virtual double get_min_rel(const DaidalusParameters& parameters) const;

  virtual double get_max_rel(const DaidalusParameters& parameters) const;

  virtual void set_special_configuration(const DaidalusParameters& parameters, int dta_status);

  virtual bool instantaneous_bands(const DaidalusParameters& parameters) const;

  virtual double own_val(const TrafficState& ownship) const;

  virtual double time_step(const DaidalusParameters& parameters, const TrafficState& ownship) const;

  virtual std::pair<Vect3, Velocity> trajectory(const DaidalusParameters& parameters, const TrafficState& ownship, double time, bool dir) const;

  virtual double max_delta_resolution(const DaidalusParameters& parameters) const;

};

}

#endif
