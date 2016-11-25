/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICALTBANDS_H_
#define KINEMATICALTBANDS_H_

#include "KinematicRealBands.h"
#include "Detection3D.h"
#include "TrafficState.h"
#include "IntervalSet.h"

#include <vector>

namespace larcfm {


class KinematicAltBands : public KinematicRealBands {

private:
  double vertical_rate_;  // Climb/descend rate for altitude band
  /* When vertical_rate = 0, instantaneous climb/descend is assumed */
  double vertical_accel_; // Climb/descend acceleration

public:
  KinematicAltBands(const KinematicBandsParameters& parameters);

  KinematicAltBands(const KinematicAltBands& b);

  bool instantaneous_bands() const;

  double get_vertical_rate() const;

  void set_vertical_rate(double val);

  double get_vertical_accel() const;

  void set_vertical_accel(double val);

  double own_val(const TrafficState& ownship) const;

  double time_step(const TrafficState& ownship) const;

  std::pair<Vect3, Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const;

  void none_bands(IntervalSet& noneset, Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  bool any_red(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  bool all_red(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  // dir=false is down, dir=true is up. Return NaN if there is not a resolution
  double resolution(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic, bool dir);

private:
  bool conflict_free_traj_step(Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      const TrafficState& ownship, const std::vector<TrafficState>& traffic) const;

  void alt_bands_generic(std::vector<Integerval>& l,
      Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  int first_nat(int mini, int maxi, bool dir, Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
      bool green);

  int first_band_alt_generic(Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      const TrafficState& ownship, const std::vector<TrafficState>& traffic, bool dir, bool green);
};

}
#endif
