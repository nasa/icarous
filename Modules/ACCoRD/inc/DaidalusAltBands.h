/*
 * Copyright (c) 2015-2018 United States Government as represented by
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


  private:
  double vertical_rate_;  // Climb/descend rate for altitude band
  double vertical_accel_; // Climb/descend acceleration

  public:
  DaidalusAltBands(DaidalusParameters& parameters);

  DaidalusAltBands(const DaidalusAltBands& b);

  /**
   * Set DaidalusParmaeters
   */
  virtual void setDaidalusParameters(const DaidalusParameters& parameters);

  virtual bool instantaneous_bands() const;

  double get_vertical_rate() const;

  void set_vertical_rate(double val);

  double get_vertical_accel() const;

  void set_vertical_accel(double val);

  virtual double own_val(const TrafficState& ownship) const;

  virtual double time_step(const TrafficState& ownship) const;

  virtual std::pair<Vect3, Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const;

  // In PVS: alt_bands@conflict_free_traj_step
  private:
  bool conflict_free_traj_step(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
          const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters) const ;

  // In PVS: alt_bands@alt_bands_generic
  void alt_bands_generic(std::vector<Integerval>& l,
          const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
          int maxup, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters);

  public:
  virtual void none_bands(IntervalSet& noneset, const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters);

  virtual bool any_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters);

  virtual bool all_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters);

  int first_nat(int mini, int maxi, bool dir, const Detection3D* conflict_det, const Detection3D* recovery_det,
          double B, double T, double B2, double T2, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters, bool green);

  int first_band_alt_generic(const Detection3D* conflict_det, const Detection3D* recovery_det, double B, double T, double B2, double T2,
          const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters, bool dir, bool green);

  // dir=false is down, dir=true is up
 virtual  double resolution(const Detection3D* conflict_det, const Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic, const DaidalusParameters& parameters,
      bool dir);

  virtual double max_delta_resolution(const DaidalusParameters& parameters) const;




};

}

#endif
