/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICINTEGERBANDS_H_
#define KINEMATICINTEGERBANDS_H_

#include "Velocity.h"
#include "Position.h"
#include "Detection3D.h"
#include "TrafficState.h"
#include "Integerval.h"
#include "IntervalSet.h"

#include <vector>
#include <string>

namespace larcfm {


class KinematicIntegerBands {

  // This class computes NONE bands

public:
  virtual std::pair<Vect3,Velocity> trajectory(const TrafficState& ownship, double time, bool dir) const = 0;
  virtual ~KinematicIntegerBands() {}

protected:
  /* Used in definition of kinematic trajectory */
  int j_step_;

private:

  static bool conflict(Detection3D* det, const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      double B, double T);

  int first_los_step(Detection3D* det, double tstep,bool trajdir,
      int min, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic) const;

  int first_los_search_index(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2, bool trajdir, int max,
      const TrafficState& ownship, const std::vector<TrafficState>& traffic) const;

  int bands_search_index(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2, 
      bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
      int epsh, int epsv) const;

  void traj_conflict_only_bands(std::vector<Integerval>& l,
      Detection3D* conflict_det, Detection3D* recovery_det, double tstep, double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic) const;

  void kinematic_bands(std::vector<Integerval>& l, Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2, 
      bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
      int epsh, int epsv) const;

public:

  bool no_conflict(Detection3D* conflict_det, Detection3D* recovery_det, double B, double T, double B2, double T2,
      bool trajdir, double tsk, const TrafficState& ownship, const std::vector<TrafficState>& traffic) const;

  static void append_intband(std::vector<Integerval>& l, std::vector<Integerval>& r);

  static void neg(std::vector<Integerval>& l);

  // INTERFACE FUNCTION
  void kinematic_bands_combine(std::vector<Integerval>& l, Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
      int epsh, int epsv) const;

  bool any_los_aircraft(Detection3D* det, bool trajdir, double tsk,
      const TrafficState& ownship, const std::vector<TrafficState>& traffic) const;

  // trajdir: false is left
  int first_green(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
      int epsh, int epsv) const;

  bool all_int_red(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
      int epsh, int epsv, int dir) const;

  bool any_conflict_aircraft(Detection3D* det, double B, double T, bool trajdir, double tsk,
      const TrafficState& ownship, const std::vector<TrafficState>& traffic) const;

  void instantaneous_bands_combine(std::vector<Integerval>& l, Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
      const TrafficState& repac,
      int epsh, int epsv);

  bool red_band_exist(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
      int epsh, int epsv) const;

  // INTERFACE FUNCTION
  bool any_int_red(Detection3D* conflict_det, Detection3D* recovery_det, double tstep,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic, const TrafficState& repac,
      int epsh, int epsv, int dir) const;

  bool all_instantaneous_red(Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
      const TrafficState& repac,
      int epsh, int epsv, int dir);

  bool any_instantaneous_red(Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      int maxl, int maxr, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
      const TrafficState& repac,
      int epsh, int epsv, int dir);

private:
  Vect3 linvel(const TrafficState& ownship, double tstep, bool trajdir, int k) const;

  bool repulsive_at(double tstep, bool trajdir, int k, const TrafficState& ownship, const TrafficState& repac, int epsh) const;

  int first_nonrepulsive_step(double tstep, bool trajdir, int max, const TrafficState& ownship, const TrafficState& repac, int epsh) const;

  bool vert_repul_at(double tstep, bool trajdir, int k, const TrafficState& ownship, const TrafficState& repac, int epsv) const;

  int first_nonvert_repul_step(double tstep, bool trajdir, int max, const TrafficState& ownship, const TrafficState& repac, int epsv) const;

  bool cd_future_traj(Detection3D* det, double B, double T, bool trajdir, double t,
      const TrafficState& ownship, const TrafficState& ac) const;

  bool any_conflict_step(Detection3D* det, double tstep, double B, double T, bool trajdir, int max,
      const TrafficState& ownship, const std::vector<TrafficState>& traffic) const;

  bool no_instantaneous_conflict(Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      bool trajdir, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
      const TrafficState& repac,
      int epsh, int epsv);

  void instantaneous_bands(std::vector<Integerval>& l, Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
      const TrafficState& repac,
      int epsh, int epsv);

  int first_instantaneous_green(Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
      const TrafficState& repac,
      int epsh, int epsv);

  bool instantaneous_red_band_exist(Detection3D* conflict_det, Detection3D* recovery_det,
      double B, double T, double B2, double T2,
      bool trajdir, int max, const TrafficState& ownship, const std::vector<TrafficState>& traffic,
      const TrafficState& repac,
      int epsh, int epsv);

};

}

#endif
