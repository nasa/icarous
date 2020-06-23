/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DAIDALUSALTBANDS_H_
#define DAIDALUSALTBANDS_H_

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

class DaidalusAltBands : public DaidalusRealBands {


public:
  DaidalusAltBands();

  DaidalusAltBands(const DaidalusAltBands& b);

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

  // In PVS: alt_bands@conflict_free_traj_step
private:
  bool conflict_free_traj_step(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
      const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) const ;

  // In PVS: alt_bands@alt_bands_generic
  void alt_bands_generic(std::vector<Integerval>& l,
      const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
      int maxup, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic);

public:
  virtual void none_bands(IntervalSet& noneset, const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic);

  virtual bool any_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic);

  virtual bool all_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic);

  int first_nat(int mini, int maxi, bool dir, const Detection3D* conflict_det, const Detection3D* recovery_det,
      double B, double T, double B2, double T2, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, bool green);

  int first_band_alt_generic(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
      const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic, bool dir, bool green);

  // dir=false is down, dir=true is up
  virtual  double resolution(const Detection3D* conflict_det, const Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
      bool dir);

  virtual double max_delta_resolution(const DaidalusParameters& parameters) const;

};

}

#endif
