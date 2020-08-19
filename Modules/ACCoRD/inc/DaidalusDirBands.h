/*
 * Copyright (c) 2015-2020 United States Government as represented by
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

public:
  DaidalusDirBands();

  DaidalusDirBands(const DaidalusDirBands& b);

  virtual bool get_recovery(const DaidalusParameters& parameters) const;

  virtual double get_step(const DaidalusParameters& parameters) const;

  virtual double get_min(const DaidalusParameters& parameters) const;

  virtual double get_max(const DaidalusParameters& parameters) const;

  virtual double get_min_rel(const DaidalusParameters& parameters) const;

  virtual double get_max_rel(const DaidalusParameters& parameters) const;

  virtual bool saturate_corrective_bands(const DaidalusParameters& parameters, int dta_status) const;

  virtual bool instantaneous_bands(const DaidalusParameters& parameters) const;

  virtual double own_val(const TrafficState& ownship) const;

  virtual double time_step(const DaidalusParameters& parameters, const TrafficState& ownship) const;

  virtual std::pair<Vect3, Velocity> trajectory(const DaidalusParameters& parameters, const TrafficState& ownship, double time, bool dir, int target_step, bool instantaneous) const;

  virtual double max_delta_resolution(const DaidalusParameters& parameters) const;

};

}

#endif
